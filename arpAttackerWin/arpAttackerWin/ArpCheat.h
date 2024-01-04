#pragma once

#ifndef ARPCHEAT_H
#define ARPCHEAT_H
#include "..\\include\\pcap.h"
#include "..\\include\\pcap\\pcap.h"
#include "PublicUtils.h"

class ArpCheat {
public:
	static int __stdcall ArpCheatProc(pcap_t * param);
	static int sendRarps(pcap_t *pt);

	static int ArpCheat::makeFakeClient(pcap_t * pcapt, unsigned int ip, unsigned char mac[MAC_ADDRESS_SIZE]);

	static int ArpCheat::broadcastArp(pcap_t * pcapt, unsigned int ip);

	static int arpReply(pcap_t * pcapt, unsigned int srcip, unsigned char srcmac[MAC_ADDRESS_SIZE], unsigned int dstip, unsigned char * dstmac);


	//static int ArpCheat::adjustSelf(pcap_t * pcapt);

	//static int ArpCheat::sendRarp(pcap_t * pcapt, unsigned int ip, unsigned char mac[MAC_ADDRESS_SIZE]);

	//static int ArpCheat::refreshGatewayArpList(pcap_t * pcapt, unsigned int ip, unsigned char mac[MAC_ADDRESS_SIZE]);
};


#endif