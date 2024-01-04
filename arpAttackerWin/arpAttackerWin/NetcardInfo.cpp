

#include <stdio.h>
#include <windows.h>
#include <Iptypes.h >
#include <iphlpapi.h>
#include "Public.h"
#include "NetcardInfo.h"


#pragma comment(lib,"Iphlpapi.lib")


PIP_ADAPTER_INFO NetcardInfo::ShowNetCardInfo(int *count) {
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)GlobalAlloc(GPTR, sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		printf("ShowNetCardInfo GlobalAlloc error\r\n");
		return FALSE;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		GlobalFree((char*)pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)GlobalAlloc(GPTR, ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			printf("ShowNetCardInfo GetAdaptersInfo first error\r\n");
			return FALSE;
		}
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		int number = 0;
		PIP_ADAPTER_INFO pAdapter = 0;
		printf("本机安装的网卡列表如下:\r\n");
		for (pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			/*
			if(pAdapter->Type != MIB_IF_TYPE_ETHERNET && pAdapter->Type !=  IF_TYPE_IEEE80211)
			{
			continue;
			}

			if(pAdapter->AddressLength != MAC_ADDRESS_SIZE)
			{
			continue;
			}
			if (lstrlenA(pAdapter->IpAddressList.IpAddress.String) < 8 || lstrlenA(pAdapter->GatewayList.IpAddress.String) < 8)
			{
			continue;
			}

			if (RtlCompareMemory(pAdapter->IpAddressList.IpAddress.String,"0.0.0.0",7) != 7 && RtlCompareMemory(pAdapter->GatewayList.IpAddress.String,"0.0.0.0",7) != 7)
			{
			break;
			}
			*/
			number++;
			printf("网卡号码:\t%d\r\n网卡名称:\t%s\r\n网卡描述:\t%s\r\n网卡类型:\t%d\r\n网卡IP地址:\t%s\r\n网关IP地址:\t%s\r\n\r\n",
				number, pAdapter->AdapterName, pAdapter->Description, pAdapter->Type, pAdapter->IpAddressList.IpAddress.String,
				pAdapter->GatewayList.IpAddress.String);
		}

		*count = number;
		//GlobalFree((char*)pAdapterInfo); 
		return pAdapterInfo;
	}
	else
	{
		printf("GetNetCardInfo GetAdaptersInfo second error\r\n");
		GlobalFree((char*)pAdapterInfo);
		return FALSE;
	}
}




PIP_ADAPTER_INFO NetcardInfo::GetNetCardAdapter(PIP_ADAPTER_INFO pAdapterInfo, int seq) {

	PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
	for (int number = 0; number < seq; pAdapter = pAdapter->Next, number++)
	{
		if (pAdapter == NULL)
		{
			return FALSE;
		}
	}
	return pAdapter;
}