
#include <windows.h>
#include <IPHlpApi.h>
#pragma comment(lib,"iphlpapi.lib")
#include <stdio.h>
#include "PublicUtils.h"
#include "Public.h"



string Public::formatIP(unsigned int ip) {
	unsigned char cip[sizeof(unsigned int)];
	memmove(cip, &ip, sizeof(unsigned int));
	char szip[256];
	wsprintfA(szip, "%u.%u.%u.%u", cip[0], cip[1], cip[2], cip[3]);
	return szip;
}


string Public::formatMAC(unsigned char mac[MAC_ADDRESS_SIZE]) {

	char szmac[256];
	wsprintfA(szmac, "%02x-%02x-%02x-%02x-%02x-%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return szmac;
}





DWORD Public::GetLocalIpAddress()
{
	char local[MAX_PATH] = {0};
	int iRet = gethostname(local, sizeof(local));
	if (iRet )
	{
		return FALSE;
	}
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
	{
		return FALSE;
	}

	in_addr addr = {0};
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); 
	if (addr.S_un.S_addr == 0)
	{
		return FALSE;
	}
	return addr.S_un.S_addr;
}




DWORD Public::GetSubNet(char * szIP,char * szSubNet){

	char * pHdr = szIP;
	char * pEnd = szIP;

	pHdr = strstr(pHdr,".");
	if (pHdr == FALSE)
	{
		return FALSE;
	}
	pHdr += 1;

	pHdr = strstr(pHdr,".");
	if (pHdr == FALSE)
	{
		return FALSE;
	}
	pHdr += 1;

	pEnd = strstr(pHdr,".");
	if (pEnd == FALSE)
	{
		return FALSE;
	}

	memmove(szSubNet,pHdr,pEnd - pHdr);
	return TRUE;
}






DWORD Public::WriteLogFile(char * pFileName,char * pData)
{
	HANDLE hFile = CreateFileA(pFileName,GENERIC_READ | GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwCnt = SetFilePointer(hFile,0,0,FILE_END);
	if (dwCnt == INVALID_SET_FILE_POINTER)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	int datalen = lstrlenA(pData);
	int iRet = WriteFile(hFile,pData, datalen,&dwCnt,0);
	CloseHandle(hFile);
	if (iRet == 0 || dwCnt != datalen)
	{
		return FALSE;
	}

	return TRUE;
}

int Public::removespace(char * src, char * dst)
{
	int len = strlen(src);
	int i = 0, j = 0;
	for (; i < len; i++) {
		if (src[i] == ' ' || src[i] == 0x9) {
			continue;
		}
		else {
			dst[j] = src[i];
			j++;
		}
	}
	*(dst + j) = 0;
	return j;
}






int Public::RecordInFile(char * szFileName,unsigned char * strBuffer,int iCounter)
{
	int iRet = 0;
	FILE * fpFile = 0;
	iRet = fopen_s(&fpFile,szFileName,"ab+");
	if (fpFile )
	{
		unsigned long ulFileSize = fseek(fpFile,0,SEEK_END);
		iRet = fwrite(strBuffer,1,iCounter,fpFile);
		fclose(fpFile);
		if (iRet != iCounter)
		{
			printf("写文件错误\n");
			return FALSE;
		}
		
		return TRUE;
	}
	else if (fpFile == 0)
	{
		iRet = fopen_s(&fpFile,szFileName,"wb+");
		if (fpFile)
		{
			unsigned long ulFileSize = fseek(fpFile,0,SEEK_END);
			fwrite(strBuffer,1,iCounter,fpFile);	
			fclose(fpFile);
			if (iRet != iCounter)
			{
				printf("写文件错误\n");
				return FALSE;
			}
			return TRUE;
		}
		else
		{
			printf("打开文件错误\n");
			return FALSE;
		}
	}
	return FALSE;
}