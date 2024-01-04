#ifndef WINPCAPDNSHIJACK_H_H_H
#define WINPCAPDNSHIJACK_H_H_H



#include "..\\include\\pcap.h"
#include "..\\include\\pcap\\pcap.h"

#include "Packet.h"



class PacketProcess {
public:
	static WORD PacketProcess::checksum(WORD *buffer, int size);

	static USHORT PacketProcess::subPackChecksum(char * lpCheckSumData, WORD wCheckSumSize, DWORD dwSrcIP, DWORD dwDstIP, unsigned short wProtocol);
	static int __stdcall Sniffer(pcap_t * pcapt);

};





#endif