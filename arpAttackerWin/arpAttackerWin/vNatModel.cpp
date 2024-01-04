/*
 * VNatModel.cpp
 *
 *  Created on: 2018�?0�?1�? *      Author: Shang
 */

/*
#include "VNatModel.h"
#include "public/Memory/MemoryAllocator.h"

#define NAT_AGE_TIME 2 * 60
#define NAT_SYNC_TIME 10 * 3600

VNatModel::VNatModel(uint16_t port_min, uint16_t port_max): mNatTable(1000)
{
	mPortMin = port_min;
	mPortMax = port_max;

	for (int i = 0; i < NAT_TABLE_SIZE; ++i) {
		mNatMetas[i] = NULL;
	}
	for (uint32_t i = port_min; i <= port_max; ++i) {
		mPortsPool.push(i);
	}
	mNatTable.clear();
	mNextSyncTs = MyTimer::getTime() + NAT_SYNC_TIME;
}

VNatModel::~VNatModel()
{
	mNatTable.deleteAll();
}

void VNatModel::onAging()
{
	uint32_t now = MyTimer::getTime();
	NatMeta* meta;
	UnorderedMap<long, NatMeta*>::_table* map = mNatTable.getMap();
	for(UnorderedMap<long, NatMeta*>::iterator iter = map->begin(); iter != map->end(); ){
		meta = iter->second;
		if((now - meta->UpdateTime) > NAT_AGE_TIME){	//老化
			mNatMetas[meta->NatPort] = NULL;
			map->erase(iter++);

			mPortsPool.push(meta->NatPort);
//			delete meta;
			MyLog::info("delete nat for aging: port = %u", meta->NatPort);
			MemoryAllocator::release(meta);
		}else {
			iter++;
	    }
	}

	if(now > mNextSyncTs ){	// 10小时同步一次nat�?		mNextSyncTs = now + NAT_SYNC_TIME;

		int total = mNatTable.size() + mPortsPool.size();
		if(total < (mPortMax - mPortMin - 10)){	// 发生端口泄漏, 泄漏阈�?10
			MyLog::warning("VNatModel occurs port leak error");

			while(!mPortsPool.empty())	// 清空
				mPortsPool.pop();
			mNatTable.clear();

			for (uint32_t i = mPortMin; i <= mPortMax; ++i) {
				if(mNatMetas[i] == NULL){
					mPortsPool.push(i);
				}else{
					meta = mNatMetas[i];
					mNatTable.setItem(genSessionID(meta->L4Type, meta->SrcIP, meta->SrcPort), meta);
				}
			}
		}
	}
}

uint16_t VNatModel::learn(InnerDataPacket* packet, bool need_dest)
{
	uint16_t new_port;
	long sid = genSessionID(packet->getL4Type(), packet->getSrcIP(), packet->getSrcPort());
	NatMeta* meta = mNatTable.getItem(sid, NULL);
	if(NULL == meta){
		if(!mPortsPool.empty()){
			new_port = mPortsPool.front();
			mPortsPool.pop();

//			meta = new NatMeta;
			meta = (NatMeta*) MemoryAllocator::alloc(sizeof(NatMeta));
			memcpy(meta->UserMac, packet->getMacData(), ETH_ALEN);
			meta->L4Type = packet->getL4Type();
			meta->SrcIP = packet->getSrcIP();
			meta->SrcPort = packet->getSrcPort();
			meta->NatPort = new_port;
			meta->UpdateTime = MyTimer::getTime();

			if(need_dest){
				meta->DestIP = packet->getDestIP();
			}else{
				meta->DestIP = 0;
			}

			mNatMetas[new_port] = meta;
			mNatTable.setItem(sid, meta);
		}else{
			// 没有空余的port可用
			MyLog::warning("VNatModel: mPortsPool has no port");
			new_port = 0;
		}
	}else{
		meta->UpdateTime = MyTimer::getTime();
		new_port = meta->NatPort;
	}
	return new_port;
}

NatMeta* VNatModel::query(uint16_t port)
{
	return mNatMetas[port];
}

long VNatModel::genSessionID(uint8_t type, uint32_t ip, uint16_t port)
{
	return (((long)ip) << 20) + (((long)port) << 4) + type ;
}
*/