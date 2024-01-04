
#include "refreshTargets.h"

#include <windows.h>
#include "..\\include\\pcap.h"
#include "..\\include\\pcap\\pcap.h"
#include "PublicUtils.h"
#include "snifferTargets.h"


int RefreshTargets::refreshTargets(void * param) {
	pcap_t * pcapt = (pcap_t*)param;


	while (TRUE)
	{
		gOnlineObjects.clear();
		int ret = SnifferTargets::snifferHostsMain(pcapt, gOnlineObjects);
		Sleep(REFRESHTARGETS_DELAY);
	}

}