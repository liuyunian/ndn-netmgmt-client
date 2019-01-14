#include <arpa/inet.h>
#include <net/ethernet.h>
#include <pcap/sll.h>
#include <iomanip>
#include <sstream>

#include <ndn-cxx/lp/nack.hpp>
#include <ndn-cxx/lp/packet.hpp>
#include <ndn-cxx/net/ethernet.hpp>
#include <ndn-cxx/util/string-helper.hpp>

#include <boost/endian/conversion.hpp>

#include "ndn_capture.hpp"

NdnCapture::NdnCapture(std::shared_ptr<std::list<std::string>> packetListPtr, std::shared_ptr<std::mutex> mutex):
    pcapFilter("(ether proto 0x8624)"),
    wantPromisc(true),
    m_pcap(nullptr),
    m_dataLinkType(-1),
    m_packetListPtr(packetListPtr),
    m_mutex(mutex)
{}

NdnCapture::~NdnCapture()
{
    if (m_pcap)
        pcap_close(m_pcap);
}

void NdnCapture::run()
{
    char errbuf[PCAP_ERRBUF_SIZE] = {};

    if (interface.empty())
    {
        const char *defaultDevice = pcap_lookupdev(errbuf);
        if (defaultDevice == nullptr)
        {
            std::cerr << errbuf << std::endl;
        }
        interface = defaultDevice;
    }

    std::string action;
    m_pcap = pcap_open_live(interface.data(), 65535, wantPromisc, 1000, errbuf);
    if (m_pcap == nullptr)
    {
        std::cerr << "Cannot open interface " << interface << ": " << errbuf << std::endl;
    }
    action = "listening on " + interface;

    m_dataLinkType = pcap_datalink(m_pcap);
    const char *dltName = pcap_datalink_val_to_name(m_dataLinkType);
    const char *dltDesc = pcap_datalink_val_to_description(m_dataLinkType);
    std::string formattedDlt = dltName ? dltName : std::to_string(m_dataLinkType);
    if (dltDesc)
    {
        formattedDlt += "(" + std::string(dltDesc) + ")";
    }

    std::cout << "ndndump: " << action << ", link-type " << formattedDlt << std::endl;

    if (!pcapFilter.empty())
    {
        bpf_program program;
        int res = pcap_compile(m_pcap, &program, pcapFilter.data(), 1, PCAP_NETMASK_UNKNOWN);
        if (res < 0)
        {
            std::cerr << "Cannot compile pcap filter '" << pcapFilter << "': " << pcap_geterr(m_pcap) << std::endl;
        }

        res = pcap_setfilter(m_pcap, &program);
        pcap_freecode(&program);
        if (res < 0)
        {
            std::cerr << "Cannot set pcap filter: " << pcap_geterr(m_pcap) << std::endl;
        }
    }

    auto callback = [](uint8_t *user, const pcap_pkthdr *pkthdr, const uint8_t *payload) {
        reinterpret_cast<const NdnCapture *>(user)->printPacket(pkthdr, payload);
    };

    if (pcap_loop(m_pcap, -1, callback, reinterpret_cast<uint8_t *>(this)) < 0)
    {
        std::cerr << "pcap_loop: " << pcap_geterr(m_pcap) << std::endl;
    }
}

void NdnCapture::printPacket(const pcap_pkthdr *pkthdr, const uint8_t *payload) const
{
    // sanity checks
    if (pkthdr->caplen == 0)
    {
        std::cout << "[Invalid header: caplen=0]" << std::endl;
        return;
    }
    if (pkthdr->len == 0)
    {
        std::cout << "[Invalid header: len=0]" << std::endl;
        return;
    }
    else if (pkthdr->len < pkthdr->caplen)
    {
        std::cout << "[Invalid header: len(" << pkthdr->len
                  << ") < caplen(" << pkthdr->caplen << ")]" << std::endl;
        return;
    }

    std::string packetInformation;
    packetInformation.append(std::to_string(pkthdr->ts.tv_sec) + "." + std::to_string(pkthdr->ts.tv_usec) + ", ");

    bool shouldPrint = false;
    switch (m_dataLinkType)
    {
        case DLT_EN10MB:
            shouldPrint = printEther(payload, pkthdr->len, packetInformation);
            break;
        default:
            BOOST_ASSERT(false);
            return;
    }

    if (shouldPrint)
    {
        // std::cout << packetInformation << std::endl;
        m_mutex->lock();
        m_packetListPtr->push_back(packetInformation);
        m_mutex->unlock();
    }
}

bool NdnCapture::dispatchByEtherType(const uint8_t *pkt, size_t len, uint16_t etherType, std::string & packetInformation) const
{
    switch (etherType)
    {
    case ndn::ethernet::ETHERTYPE_NDN:
    case 0x7777: // NDN ethertype used in ndnSIM
        packetInformation.append("Ethernet, ");
        return printNdn(pkt, len, packetInformation);
    default:
        std::cout << "[Unsupported ethertype " << ndn::AsHex{etherType} << "]" << std::endl;
        return false;
    }
}

bool NdnCapture::printEther(const uint8_t *pkt, size_t len, std::string & packetInformation) const
{
    // IEEE 802.3 Ethernet

    if (len < ndn::ethernet::HDR_LEN)
    {
        std::cout << "Truncated Ethernet frame, length " << len << std::endl;
        return false;
    }

    auto ether = reinterpret_cast<const ether_header *>(pkt);
    pkt += ndn::ethernet::HDR_LEN;
    len -= ndn::ethernet::HDR_LEN;

    return dispatchByEtherType(pkt, len, boost::endian::big_to_native(ether->ether_type), packetInformation);
}

bool NdnCapture::printNdn(const uint8_t *pkt, size_t len, std::string & packetInformation) const
{
    if (len == 0)
    {
        return false;
    }

    bool isOk = false;
    ndn::Block block;
    std::tie(isOk, block) = ndn::Block::fromBuffer(pkt, len);
    if (!isOk)
    {
        // if packet is incomplete, we will not be able to process it
        std::cout << "NDN truncated packet, length " << len << std::endl;
        return false;
    }

    ndn::lp::Packet lpPacket;
    ndn::Block netPacket;

    if (block.type() == ndn::lp::tlv::LpPacket)
    {
        try
        {
            lpPacket.wireDecode(block);
        }
        catch (const ndn::tlv::Error &e)
        {
            std::cout << " invalid packet: " << e.what() << std::endl;
            return false;
        }

        ndn::Buffer::const_iterator begin, end;
        if (lpPacket.has<ndn::lp::FragmentField>())
        {
            std::tie(begin, end) = lpPacket.get<ndn::lp::FragmentField>();
        }
        else
        {
            std::cout << " idle" << std::endl;
            return false;
        }

        bool isOk = false;
        std::tie(isOk, netPacket) = ndn::Block::fromBuffer(&*begin, std::distance(begin, end));
        if (!isOk)
        {
            // if network packet is fragmented, we will not be able to process it
            std::cout << " fragment" << std::endl;
            return false;
        }
    }
    else
    {
        netPacket = std::move(block);
    }

    try
    {
        switch (netPacket.type())
        {
            case ndn::tlv::Interest:
            {
                ndn::Interest interest(netPacket);

                if (lpPacket.has<ndn::lp::NackField>())
                {
                    ndn::lp::Nack nack(interest);
                    nack.setHeader(lpPacket.get<ndn::lp::NackField>());
                    packetInformation.append("NACK: " + interest.getName().toUri());
                }
                else
                {
                    packetInformation.append("INTEREST: " + interest.getName().toUri());
                }
                return true;
                break;
            }
            case ndn::tlv::Data:
            {
                ndn::Data data(netPacket);

                packetInformation.append("DATA: " + data.getName().toUri());\
                return true;
                break;
                
            }
            default:
            {
                std::cout << "[Unsupported NDN packet type " << netPacket.type() << "]" << std::endl;
                return false;
                break;
            }
        }
    }
    catch (const ndn::tlv::Error &e)
    {
        std::cout << "invalid network packet: " << e.what() << std::endl;
    }
}