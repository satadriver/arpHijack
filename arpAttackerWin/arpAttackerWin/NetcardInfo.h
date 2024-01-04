#pragma once

#ifndef NETCARDINFO_H
#define NETCARDINFO_H
#include <windows.h>
#include <Iptypes.h >
#include <iphlpapi.h>




class NetcardInfo {
public:
	static PIP_ADAPTER_INFO ShowNetCardInfo(int * counter);
	static PIP_ADAPTER_INFO GetNetCardAdapter(PIP_ADAPTER_INFO pAdapterInfo, int seq);
};

#endif