

#include "virtualIP.h"
#include "snifferTargets.h"
#include "connectionManager.h"
#include <iostream>
#include <string>
#include <vector>
#include "Public.h"
#include <algorithm>



using namespace std;

unsigned int VirtualIP::makeVirtualIP(void * param, vector <CLIENTADDRESSES> &targets) {

	int ret = SnifferTargets::snifferHostsMain(param, targets);
	if (targets.size() <=0 )
	{
		printf("not found targets on the line,please rescan\r\n");
		return 0;
	}


	//sort(gOnlineObjects.begin(), gOnlineObjects.end());
	//gOnlineObjects.erase(unique(gOnlineObjects.begin(), gOnlineObjects.end()), gOnlineObjects.end());

	int cnt = ConnectionManager::getSubnetSize();

	for (int i = 0; i < cnt; i++) {
		unsigned int ip = ntohl(ntohl(gNetMaskIP) + i);

		if (i == 0xff || ip == gLocalIP || ip == gNetMaskIP || ip == gGatewayIP) {
			continue;
		}

		
		unsigned int j = 0;
		for (j = 0;j < targets.size(); j ++)
		{
			if (ip == targets[j].clientIP)
			{
				break;
			}
		}

		if (j >= targets.size())
		{
			gFakeProxyIP = ip;
			break;
		}
	}

	if (gFakeProxyIP)
	{
		string strip = Public::formatIP(gFakeProxyIP);
		printf("get fake client ip:%s\r\n", strip.c_str());
	}
	else {
		printf("find fake client ip error\r\n");
		
	}

	return gFakeProxyIP;
	
}