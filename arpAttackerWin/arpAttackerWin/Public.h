
#ifndef PUBLIC_H_H_H
#define PUBLIC_H_H_H

#include <windows.h>

#include "PublicUtils.h"	
#include <iostream>
#include <string>

using namespace std;


class Public {
public:
	static int removespace(char * src, char * dst);
	static DWORD GetSubNet(char * szIP, char * szSubNet);

	static DWORD GetLocalIpAddress();
	static DWORD WriteLogFile(char * pFileName, char * pData);
	static int RecordInFile(char * szFileName, unsigned char * strBuffer, int iCounter);

	static string formatIP(unsigned int);
	static string formatMAC(unsigned char mac[MAC_ADDRESS_SIZE]);
};

#endif