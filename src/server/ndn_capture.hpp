#ifndef NDN_CAPTURE_H_
#define NDN_CAPTURE_H_

#include <pcap.h>
#include <iostream>
#include <mutex>

class NdnCapture{

public:
    NdnCapture(std::shared_ptr<std::list<std::string>> packetListPtr, std::shared_ptr<std::mutex> mutex);
    NdnCapture(const NdnCapture & capture) = delete; //不可拷贝复制
    NdnCapture & operator = (const NdnCapture &) = delete; //不可等号复制

	~NdnCapture();

	void run();

	void printPacket(const pcap_pkthdr *pkthdr, const uint8_t *payload) const;

private:

	bool dispatchByEtherType(const uint8_t *pkt, size_t len, uint16_t etherType, std::string & packetInformation) const;

	bool printEther(const uint8_t *pkt, size_t len, std::string & packetInformation) const;

	bool printNdn(const uint8_t *pkt, size_t len, std::string & packetInformation) const;

private:
	std::string interface;
	std::string pcapFilter;
	bool wantPromisc;
	pcap_t * m_pcap;
	int m_dataLinkType;

	std::shared_ptr<std::list<std::string>> m_packetListPtr;
	std::shared_ptr<std::mutex> m_mutex;
};

#endif
