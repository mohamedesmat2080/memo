#include <Rpc.h>
#include "ShardNetManager.h"
//#include "ClientManager.h"

CShardNetManager::FN_BROADCAST_MSG_TO_GAMESERVERS CShardNetManager::s_pfnBroadcastMsgToGameServers;
CShardNetManager::FN_ALLOC_MSG_FOR_GS CShardNetManager::s_pfnAllocMsgForGS;

class CNetEngine;

#define NET_ENGINE_FOR_GS_REF_OFFSET				0x00858868
#define FN_BROADCAST_MSG_TO_GAMESERVERS_OFFSET		0x004121E0
//CNetEngine VFT + 0x48
#define FN_ALLOC_MSG_FOR_GS_OFFSET					0x006B8260

#define g_pNetEngineForGS (*(CNetEngine**)NET_ENGINE_FOR_GS_REF_OFFSET)


void CShardNetManager::Setup()
{
    s_pfnBroadcastMsgToGameServers = reinterpret_cast<FN_BROADCAST_MSG_TO_GAMESERVERS>(
        FN_BROADCAST_MSG_TO_GAMESERVERS_OFFSET
        );

    s_pfnAllocMsgForGS = reinterpret_cast<FN_ALLOC_MSG_FOR_GS>(
        FN_ALLOC_MSG_FOR_GS_OFFSET
        );



    //CreateThread(NULL, 0, MyTestWorker2, NULL, 0, NULL);

#ifdef __SHARDNET_DEMO__
    CreateThread(NULL, 0, MyTestWorkerThread, NULL, 0, NULL);
#endif


    //printf("Shard net mgr thread spawned\n");

}

void CShardNetManager::BroadcastMsgToGameServers(CMsgStreamBuffer* pMsg)
{
    s_pfnBroadcastMsgToGameServers(pMsg);
}

CMsgStreamBuffer* CShardNetManager::AllocMsgForGS()
{
    return s_pfnAllocMsgForGS(g_pNetEngineForGS, 0);
}