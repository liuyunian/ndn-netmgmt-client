#ifndef NDN_CAPTURE_H_
#define NDN_CAPTURE_H_

#include <pcap.h>
#include <iostream>

class OutputFormatter;

class NdnCapture
{
  public:
    NdnCapture();
    NdnCapture(const NdnCapture & capture) = delete; //不可拷贝复制
    NdnCapture & operator = (const NdnCapture &) = delete; //不可等号复制

	~NdnCapture();

	void run();

	void printPacket(const pcap_pkthdr *pkthdr, const uint8_t *payload) const;

private:
	void
	printTimestamp(std::ostream &os, const timeval &tv) const;

	bool
	dispatchByEtherType(OutputFormatter &out, const uint8_t *pkt, size_t len, uint16_t etherType) const;

	bool
	printEther(OutputFormatter &out, const uint8_t *pkt, size_t len) const;

	bool
	printNdn(OutputFormatter &out, const uint8_t *pkt, size_t len) const;

public:
	std::string interface;
	std::string pcapFilter;
	bool wantPromisc;
	bool wantTimestamp;
	pcap_t * m_pcap;
	int m_dataLinkType;
};

#endif