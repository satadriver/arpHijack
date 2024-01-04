
#include "nat.h"
#include <time.h>
#include <windows.h>
#include "PublicUtils.h"
#include <stdio.h>

static LPNATDATA gNatUdpBase;
static LPNATDATA gNatUdpPointer;

static LPNATDATA gNatTcpBase;
static LPNATDATA gNatTcpPointer;


#define SOCKET_MAX_ALIVE_TIME 300

void NAT::init() {
	gNatUdpBase = new NATDATA[NATSIZE];
	gNatTcpBase = new NATDATA[NATSIZE];
	gNatUdpPointer = gNatUdpBase;
	gNatTcpPointer = gNatTcpBase;

	for (unsigned int i = 0;i < NATSIZE; i ++)
	{
		gNatUdpBase[i].port = NATMIN + i;
		gNatUdpBase[i].isfull = 0;
	}

	for (unsigned int i = 0; i < NATSIZE; i++)
	{
		gNatTcpBase[i].port = NATMIN + i;
		gNatTcpBase[i].isfull = 0;
	}
}


NATDATA* NAT::ifexist(unsigned long ip, unsigned short port,int protocol) {
	LPNATDATA lpptr = 0;
	LPNATDATA lpbase = 0;

	if (protocol == IPPROTO_TCP)
	{
		lpptr = gNatTcpPointer;
		lpbase = gNatTcpBase;
	}
	else if (protocol == IPPROTO_UDP)
	{
		lpptr = gNatUdpPointer;
		lpbase = gNatUdpBase;
	}
	else {
		printf("not support\r\n");
		return 0;
	}

	for (int i = 0;i < NATSIZE; i ++)
	{
		if (lpbase[i].ip == ip && lpbase[i].port == port)
		{
			return &lpbase[i];
		}
	}

	return 0;
}



void NAT::transfer(unsigned char *mac,unsigned long &ip,unsigned short &port, int protocol) {
	time_t now = time(0);
	LPNATDATA lpnatold = 0;
	LPNATDATA lpptr = 0;
	LPNATDATA lpbase = 0;

	if (protocol == IPPROTO_TCP)
	{
		lpnatold = gNatTcpPointer;
		lpptr = gNatTcpPointer;
		lpbase = gNatTcpBase;
	}else if (protocol == IPPROTO_UDP)
	{
		lpnatold = gNatUdpPointer;
		lpptr = gNatUdpPointer;
		lpbase = gNatUdpBase;
	}
	else {
		printf("not support\r\n");
		return;
	}

	do
	{
		if ( (lpptr->isfull == 0) || (now - lpptr->time > SOCKET_MAX_ALIVE_TIME) )
		{
			memcpy(lpptr->mac, mac,MAC_ADDRESS_SIZE);
			lpptr->port = port;
			lpptr->ip = ip;
			lpptr->isfull = TRUE;
			lpptr->time = now;

			unsigned short tmpport = (lpptr - lpbase) + NATMIN;
			port = ntohs(tmpport);
			//ip = gFakeProxyIP;
			
			if (lpptr >= lpbase + NATSIZE)
			{
				lpptr = lpbase;
			}
			else {
				lpptr++;
			}

			return;
		}
		else {
			if (lpptr >= lpbase + NATSIZE)
			{
				lpptr = lpbase;
			}
			else {
				lpptr++;
			}
		}	
	} while (lpptr != lpnatold);


	printf("nat table is full!\r\n");
	return;
}

void NAT::get(unsigned char *mac, unsigned long &ip, unsigned short &port, int protocol) {
	LPNATDATA lpptr = 0;
	LPNATDATA lpbase = 0;

	if (protocol == IPPROTO_TCP)
	{
		lpptr = gNatTcpPointer;
		lpbase = gNatTcpBase;
	}
	else if (protocol == IPPROTO_UDP)
	{
		lpptr = gNatUdpPointer;
		lpbase = gNatUdpBase;
	}
	else {
		printf("not support\r\n");
		return;
	}

	unsigned short tmpport = ntohs(port);
	LPNATDATA lpnat = lpbase + (tmpport - NATMIN);
	port = lpnat->port;
	ip = lpnat->ip;
	memcpy(mac, lpnat->mac, MAC_ADDRESS_SIZE);
}

void NAT::reset(unsigned short port, int protocol) {

	LPNATDATA lpptr = 0;
	LPNATDATA lpbase = 0;

	if (protocol == IPPROTO_TCP)
	{
		lpptr = gNatTcpPointer;
		lpbase = gNatTcpBase;
	}
	else if (protocol == IPPROTO_UDP)
	{
		lpptr = gNatUdpPointer;
		lpbase = gNatUdpBase;
	}
	else {
		printf("not support\r\n");
		return;
	}

	LPNATDATA lpnat = lpbase + port - NATMIN;
	lpnat->isfull = 0;
}









//#define RAND_MAX 0xffffffff
void test() {
	NAT::init();
	srand(time(0));
	int tmpip = 0x72727272;
	unsigned short tmpport = 65535;
	int protocol = 6;
	unsigned __int64 intmac = 0x1234567890123456;

	unsigned char *mac = (unsigned char *)&intmac;

	int counter = 0;
	while (true)
	{
		unsigned char oldmac[6];
		memcpy(oldmac, mac, 6);

		unsigned long ip = tmpip;
		unsigned short port = tmpport;
		unsigned long oldip = ip;
		unsigned short oldport = port;


		NAT::transfer(mac, ip, port, protocol);


		NAT::get(mac, ip, port, protocol);

		if (oldip != ip || oldport != port || memcmp(mac, oldmac, 6))
		{
			printf("error\r\n");
		}
		Sleep(100);

		int randvalue = rand();
		tmpip += randvalue;
		tmpport += randvalue;
		tmpport = tmpport % 65535;
		intmac += randvalue;

		if (protocol == 6)
		{
			protocol = 17;
		}

		counter++;
		if (counter % 100 == 0)
		{
			printf("counter:%d\r\n", counter);
		}
	}
}