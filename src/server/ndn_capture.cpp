#include "ndn_capture.hpp"

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

class OutputFormatter
{
  public:
    OutputFormatter(std::ostream &os, std::string d)
        : m_os(os), m_delim(std::move(d))
    {
    }

    OutputFormatter(const OutputFormatter & capture) = delete; //不可拷贝复制
    OutputFormatter & operator = (const OutputFormatter &) = delete; //不可等号复制

    OutputFormatter & addDelimiter()
    {
        if (!m_isEmpty)
        {
            m_wantDelim = true;
        }
        return *this;
    }

  private:
    std::ostream &m_os;
    std::string m_delim;
    bool m_isEmpty = true;
    bool m_wantDelim = false;

    template <typename T>
    friend OutputFormatter &operator<<(OutputFormatter &, const T &);
};

template <typename T> OutputFormatter & operator<<(OutputFormatter &out, const T &val) //重载<<运算符
{
    if (out.m_wantDelim)
    {
        out.m_os << out.m_delim;
        out.m_wantDelim = false;
    }
    out.m_os << val;
    out.m_isEmpty = false;
    return out;
}

NdnCapture::NdnCapture():
    pcapFilter("(ether proto 0x8624)"),
    wantPromisc(true),
    wantTimestamp(true),
    m_pcap(nullptr),
    m_dataLinkType(-1)
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
    if (!interface.empty())
    {
        m_pcap = pcap_open_live(interface.data(), 65535, wantPromisc, 1000, errbuf);
        if (m_pcap == nullptr)
        {
            std::cerr << "Cannot open interface " << interface << ": " << errbuf << std::endl;
        }
        action = "listening on " + interface;
    }

    m_dataLinkType = pcap_datalink(m_pcap);
    const char *dltName = pcap_datalink_val_to_name(m_dataLinkType);
    const char *dltDesc = pcap_datalink_val_to_description(m_dataLinkType);
    std::string formattedDlt = dltName ? dltName : std::to_string(m_dataLinkType);
    if (dltDesc)
    {
        formattedDlt += "(" + std::string(dltDesc) + ")";
    }

    std::cerr << "ndndump: " << action << ", link-type " << formattedDlt << std::endl;

    switch (m_dataLinkType)
    {
    case DLT_EN10MB:
    case DLT_LINUX_SLL:
    case DLT_PPP:
        // we know how to handle these
        break;
    default:
        std::cerr << "Unsupported link-layer header type " << formattedDlt << std::endl;
    }

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

    std::ostringstream os;
    OutputFormatter out(os, ", ");

    bool shouldPrint = false;
    switch (m_dataLinkType)
    {
    case DLT_EN10MB:
        shouldPrint = printEther(out, payload, pkthdr->len);
        break;
    default:
        BOOST_ASSERT(false);
        return;
    }

    if (shouldPrint)
    {
        if (wantTimestamp)
        {
            printTimestamp(std::cout, pkthdr->ts);
        }
        std::cout << os.str() << std::endl;
    }
}

void NdnCapture::printTimestamp(std::ostream &os, const timeval &tv) const
{
    /// \todo Add more timestamp formats (time since previous packet, time since first packet, ...)
    os << tv.tv_sec
       << "."
       << std::setfill('0') << std::setw(6) << tv.tv_usec
       << " ";
}

bool NdnCapture::dispatchByEtherType(OutputFormatter &out, const uint8_t *pkt, size_t len, uint16_t etherType) const
{
    out.addDelimiter();

    switch (etherType)
    {
    case ndn::ethernet::ETHERTYPE_NDN:
    case 0x7777: // NDN ethertype used in ndnSIM
        out << "Ethernet";
        return printNdn(out, pkt, len);
    default:
        out << "[Unsupported ethertype " << ndn::AsHex{etherType} << "]";
        return true;
    }
}

bool NdnCapture::printEther(OutputFormatter &out, const uint8_t *pkt, size_t len) const
{
    // IEEE 802.3 Ethernet

    if (len < ndn::ethernet::HDR_LEN)
    {
        out << "Truncated Ethernet frame, length " << len;
        return true;
    }

    auto ether = reinterpret_cast<const ether_header *>(pkt);
    pkt += ndn::ethernet::HDR_LEN;
    len -= ndn::ethernet::HDR_LEN;

    return dispatchByEtherType(out, pkt, len, boost::endian::big_to_native(ether->ether_type));
}

bool NdnCapture::printNdn(OutputFormatter &out, const uint8_t *pkt, size_t len) const
{
    if (len == 0)
    {
        return false;
    }
    out.addDelimiter();

    bool isOk = false;
    ndn::Block block;
    std::tie(isOk, block) = ndn::Block::fromBuffer(pkt, len);
    if (!isOk)
    {
        // if packet is incomplete, we will not be able to process it
        out << "NDN truncated packet, length " << len;
        return true;
    }

    ndn::lp::Packet lpPacket;
    ndn::Block netPacket;

    if (block.type() == ndn::lp::tlv::LpPacket)
    {
        out << "NDNLPv2";
        try
        {
            lpPacket.wireDecode(block);
        }
        catch (const ndn::tlv::Error &e)
        {
            out << " invalid packet: " << e.what();
            return true;
        }

        ndn::Buffer::const_iterator begin, end;
        if (lpPacket.has<ndn::lp::FragmentField>())
        {
            std::tie(begin, end) = lpPacket.get<ndn::lp::FragmentField>();
        }
        else
        {
            out << " idle";
            return true;
        }

        bool isOk = false;
        std::tie(isOk, netPacket) = ndn::Block::fromBuffer(&*begin, std::distance(begin, end));
        if (!isOk)
        {
            // if network packet is fragmented, we will not be able to process it
            out << " fragment";
            return true;
        }
    }
    else
    {
        netPacket = std::move(block);
    }
    out.addDelimiter();

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
                out << "NACK (" << nack.getReason() << "): " << interest;
            }
            else
            {
                out << "INTEREST: " << interest;
            }
            break;
        }
        case ndn::tlv::Data:
        {
            ndn::Data data(netPacket);

            out << "DATA: " << data.getName();
            break;
        }
        default:
        {
            out << "[Unsupported NDN packet type " << netPacket.type() << "]";
            break;
        }
        }
    }
    catch (const ndn::tlv::Error &e)
    {
        out << "invalid network packet: " << e.what();
    }

    return true;
}