#pragma once

#ifndef CLIENTADDRESS_H
#define CLIENTADDRESS_H

//#include <list>

#include <vector>

#include "PublicUtils.h"

using namespace std;








class  ClientAddress{
public:
	static int getMACFromIP(unsigned int ip,unsigned char mac[]);
	static unsigned char* ClientAddress::isTarget(unsigned int ip);
	static unsigned int ClientAddress::isTarget(unsigned char mac[MAC_ADDRESS_SIZE]);

	//static int ClientAddress::isTarget(unsigned int ip);
};


#endif