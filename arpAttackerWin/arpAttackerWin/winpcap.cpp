

#include "winpcap.h"
#include "PublicUtils.h"


pcap_t * Winpcap::init(string devn,int speed) {
	char		strPcapErrBuf[PCAP_ERRBUF_SIZE];
	string devname = string(WINPCAP_NETCARD_NAME_PREFIX) + devn;
	pcap_t *	pcapt = pcap_open_live(devname.c_str(), MAX_PACKET_SIZE, 1, speed, strPcapErrBuf);
	if (pcapt == NULL)
	{
		fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap\n", devname.c_str());
		getchar();
		return 0;
	}

	int ret = pcap_setbuff(pcapt, MAX_PCAP_BUFFER);	//the limit buffer size of capraw is 100M
	if (ret == -1)
	{
		printf("pcap_setbuff error!the limit of the buffer size is 100M,maybe it is too big!\n");
		getchar();
		return 0;
	}

// 	bpf_program		stBpfp = {0};
// 	u_int			uiMypcapNetMask = gNetMask;
// 	nRetCode = pcap_compile(pcapt, &stBpfp, PCAP_IP_FILTER, 1, uiMypcapNetMask);	
// 	if(nRetCode <0 )
// 	{		
// 		fprintf(stderr,"数据包过滤条件语法设置失败,请检查过滤条件的语法设置\n");
// 		getchar();
// 		return FALSE;
// 	}
// 
// 	nRetCode = pcap_setfilter(pcapt, &stBpfp);
// 	if( nRetCode < 0 )
// 	{
// 		fprintf(stderr,"数据包过滤条件设置失败\n");
// 		getchar();
// 		return FALSE;
// 	}

	return pcapt;
}