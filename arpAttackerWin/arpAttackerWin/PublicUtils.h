#pragma once


#ifndef PUBLICUTILS_H_H_H
#define PUBLICUTILS_H_H_H

#include <iostream>
#include <vector>
#include <winsock2.h>
#include <map>


using namespace std;


#define BROADCAST_MAC_ADDRESS				"\x0ff\x0ff\x0ff\x0ff\x0ff\x0ff"
#define ZERO_MAC_ADDRESS					"\x00\x00\x00\x00\x00\x00"

#define WINPCAP_NETCARD_NAME_PREFIX			"\\Device\\NPF_"
#define CONFIG_INIT_FILENAME				"config.ini"
#define PCAP_IP_FILTER						"ip or arp or icmp"		//"udp dst port 53 or udp src port 53"

#define PCAP_OPENFLAG_PROMISCUOUS			1
#define PCAP_OPEN_LIVE_TO_MS_VALUE_LEAST	-1
#define PCAP_OPEN_LIVE_TO_MS_VALUE_0		0
#define MAX_PACKET_SIZE						0x10000
#define MAX_PCAP_BUFFER						0x4000000			//pcap buf size = 64M
#define MTU									1500
#define MAC_ADDRESS_SIZE					6	

#define MAP_CLEAR_WAITSECONDS				120000
#define REFRESHTARGETS_DELAY				300000

#pragma pack(1)


typedef struct
{
	unsigned char clientMAC[MAC_ADDRESS_SIZE];
	unsigned int clientIP;
	unsigned short clientPort;
	//unsigned short proxyPort;
	time_t time;
}CLIENTADDRESSES, *LPCLIENTADDRESSES;



typedef struct {
	unsigned int serverIP;
	unsigned short serverPort;
	unsigned int clientIP;
	unsigned short clientPort;
}MAPSOCKETKEY, *LPMAPSOCKETKEY;

//使用#pragma pack(n)，指定c编译器按照n个字节对齐；
//使用#pragma pack()，取消自定义字节对齐方式。
#pragma pack()



extern unsigned int gNetMask;
extern unsigned int gNetMaskIP;

extern unsigned int gGatewayIP;
extern unsigned int gLocalIP;

extern unsigned char gGatewayMAC[MAC_ADDRESS_SIZE];
extern unsigned char gLocalMAC[MAC_ADDRESS_SIZE];

extern vector <CLIENTADDRESSES> gAttackTargetIP;

extern vector <CLIENTADDRESSES> gOnlineObjects;

//extern unsigned short PORT_PROXY_VALUE;


extern unsigned int gFakeProxyIP;

extern char gDevName[MAX_PATH];

extern int gNetcardIndex;

extern string gCardName;

extern int gCapSpeed;
extern int gArpDelay;



#endif