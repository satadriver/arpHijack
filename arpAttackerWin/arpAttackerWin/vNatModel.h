/*
 * VNatModel.h
 *
 *  Created on: 2018�?0�?1�? *      Author: Shang
 */

/*
#ifndef MAIN_MODULE_VNATMODEL_H_
#define MAIN_MODULE_VNATMODEL_H_

#include "public/public.h"
#include "public/UnorderedMap.h"
#include <queue>
#include "main/packet/InnerDataPacket.h"

#define NAT_TABLE_SIZE 64*1024

typedef struct {
	uint8_t UserMac[ETH_ALEN];
	uint8_t L4Type;
	uint32_t SrcIP;
	uint16_t SrcPort;
	uint32_t DestIP;	// �?，则进行目的ip转换
	uint16_t NatPort;
	uint32_t UpdateTime;
}NatMeta;

class VNatModel
{
public:
	VNatModel(uint16_t port_min = 1000, uint16_t port_max = NAT_TABLE_SIZE - 1);
	virtual ~VNatModel();

	void onAging();

	uint16_t learn(InnerDataPacket* packet, bool need_dest=false);

	NatMeta* query(uint16_t port);

private:
	NatMeta* mNatMetas[NAT_TABLE_SIZE];
	UnorderedMap<long, NatMeta*> mNatTable;
	queue<uint16_t> mPortsPool;
	uint16_t mPortMin;
	uint16_t mPortMax;
	uint32_t mNextSyncTs;

	static long genSessionID(uint8_t type, uint32_t ip, uint16_t port);
};

#endif /* MAIN_MODULE_VNATMODEL_H_ */

*/
