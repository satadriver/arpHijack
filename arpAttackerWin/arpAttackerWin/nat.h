#pragma once


#include <iostream>
#include <string>
#include "PublicUtils.h"

using namespace std;

#pragma pack(1)

typedef struct {
	unsigned char mac[MAC_ADDRESS_SIZE];
	unsigned long ip;
	unsigned short port;
	time_t time;
	unsigned char isfull;
}NATDATA,*LPNATDATA;


#define NATMIN 10000
#define NATMAX 14096
#define NATSIZE (NATMAX - NATMIN)

class NAT {
public:



	static void init();

	static void transfer(unsigned char *mac, unsigned long &ip, unsigned short &port,int protocol);

	static void get(unsigned char *mac, unsigned long &ip, unsigned short &port, int protocol);

	static void reset(unsigned short port, int protocol);

	static NATDATA* ifexist(unsigned long ip, unsigned short port, int protocol);
};