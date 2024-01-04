
#include "snifferTargets.h"
#include "PublicUtils.h"
#include <windows.h>
#include "ClientAddress.h"
#include <time.h>
#include "config.h"
#include <string>
#include <iostream>
#include "connectionManager.h"
#include <vector>
#include "Packet.h"
#include "Public.h"


using namespace std;


#define ARP_SNIFFER_WAITTIME 6

int SnifferTargets::snifferHostsMain(void * param, vector <CLIENTADDRESSES> &attackList)
{
	int iRet = 0;
	char szShowInfo[1024];

	pcap_t * pcapt = (pcap_t *)param;

	pcap_pkthdr *				lpPcapHdr = 0;
	const unsigned char *		lpPacket = 0;
	iRet = pcap_next_ex(pcapt, &lpPcapHdr, &lpPacket);

	int cnt = ConnectionManager::getSubnetSize();
	for (int i = 0; i < cnt; i++)
	{
		unsigned int startip = ntohl(ntohl(gNetMaskIP) + i);

		if (i == 0xff || startip == gLocalIP || startip == gNetMaskIP || startip == gGatewayIP) {
			continue;
		}

		iRet = ArpCheat::broadcastArp(pcapt, startip);

		iRet = ArpCheat::broadcastArp(pcapt, startip);

		iRet = ArpCheat::broadcastArp(pcapt, startip);

		Sleep(1);
	}

	time_t oldtime = time(0);

	while (time(0) - oldtime < ARP_SNIFFER_WAITTIME)
	{

		iRet = pcap_next_ex(pcapt, &lpPcapHdr, &lpPacket);
		if (iRet == 0)
		{
			continue;
		}
		else if (iRet < 0)
		{
			char * lpError = pcap_geterr(pcapt);
			wsprintfA(szShowInfo, "pcap_next_ex return value is 0 or negtive,error description:%s\r\n", lpError);
			printf(szShowInfo);
			continue;
		}
		else if (lpPcapHdr->caplen != lpPcapHdr->len || lpPcapHdr->caplen >= MAX_PACKET_SIZE)
		{
			printf("pcap_next_ex caplen error\r\n");
			continue;
		}

		int iCapLen = lpPcapHdr->caplen;
		LPMACHEADER lpMac = (LPMACHEADER)lpPacket;

		if (lpMac->Protocol == 0x0608) {
			LPARPHEADER		ARPheader = (LPARPHEADER)((char*)lpMac + sizeof(MACHEADER));
			if (ARPheader->HardWareType == 0x0100 && ARPheader->ProtocolType == 0x0008 && ARPheader->HardWareSize == MAC_ADDRESS_SIZE &&
				ARPheader->ProtocolSize == sizeof(unsigned int)) {
				unsigned int senderip = *(unsigned int *)ARPheader->SenderIP;
				unsigned int recverip = *(unsigned int *)ARPheader->RecverIP;

				if (ARPheader->Opcode == 0x0200)
				{

					iRet = Config::addTarget(attackList, senderip, ARPheader->SenderMac);
				}
				//here error,why?
				/*
				else if (ARPheader->Opcode == 0x0200)
				{
					CLIENTADDRESSES ca = { 0 };
					ca.clientIP = recverip;
					memmove(ca.clientMAC, ARPheader->RecverMac, MAC_ADDRESS_SIZE);
					ca.proxyPort = PORT_PROXY_VALUE;
					PORT_PROXY_VALUE -= 1;
					attackList.push_back(ca);

					string strip = Public::formatIP(ca.clientIP);
					string strmac = Public::formatMAC(ca.clientMAC);
					printf("add attack ip:%s,mac:%s\r\n", strip.c_str(), strmac.c_str());
				}*/
			}
		}
	}
	
	return 0;
}




int SnifferTargets::snifferHosts(vector <CLIENTADDRESSES> &target) {
	int ret = 0;

	int counter = 0;

	int cnt = ConnectionManager::getSubnetSize();

	for (int i = 0; i < cnt; i++) {
		unsigned int startip = ntohl(ntohl(gNetMaskIP) + i);

		if (i == 0xff || startip == gLocalIP || startip == gNetMaskIP || startip == gGatewayIP) {
			continue;
		}

		ret = Config::addTarget(startip, target);
		if (ret == 0)
		{
			counter++;
		}
	}

	return counter;
}