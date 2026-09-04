#include "NetHelper.h"
#include <memory/detours.h>


CNetHelper::FN_EXEC_HANDLER_FOR_CGOBJPC CNetHelper::s_pfnExecHandlerForCGObjPC;
CNetHelper::FN_ALLOC_MSG CNetHelper::s_pfnAllocMsg;
CNetHelper::FN_ALLOC_MSG_FOR_BROADCAST CNetHelper::s_pfnAllocMsgForBroadcast;
CNetHelper::FN_FREE_MSG CNetHelper::s_pfnFreeMsg;
CNetHelper::FN_CGOBJPC_ALLOCMSGFORPEER CNetHelper::s_pfnCGObjPC_AllocMsgForPeer;

#define NET_ENGINE_ALLOC_MSG_FN_OFFSET					0x0096BF10
#define NET_ENGINE_ALLOC_MSG_FOR_BROADAST_FN_OFFSET		0x00430A00
#define NET_ENGINE_FREE_MSG_FN_OFFSET					0x0096C030

#define FN_EXEC_HANDLER_FOR_CGOBJPC_MSG_OFFSET			0x0050EEE0
#define FN_CGOBJPC_ALLOC_MSG_FOR_PEER_OFFSET			0x004E0830

const DWORD FN_SEND_MSG_TO_SM_OFFSET = 0x00430990;

void CNetHelper::Initialize()
{
	s_pfnAllocMsg = reinterpret_cast<FN_ALLOC_MSG>(
		NET_ENGINE_ALLOC_MSG_FN_OFFSET
		);

	s_pfnAllocMsgForBroadcast = reinterpret_cast<FN_ALLOC_MSG_FOR_BROADCAST>(
		NET_ENGINE_ALLOC_MSG_FOR_BROADAST_FN_OFFSET
		);

	s_pfnFreeMsg = reinterpret_cast<FN_FREE_MSG>(
		NET_ENGINE_FREE_MSG_FN_OFFSET
		);

	s_pfnExecHandlerForCGObjPC = reinterpret_cast<FN_EXEC_HANDLER_FOR_CGOBJPC>(
		FN_EXEC_HANDLER_FOR_CGOBJPC_MSG_OFFSET
		);

	s_pfnCGObjPC_AllocMsgForPeer = reinterpret_cast<FN_CGOBJPC_ALLOCMSGFORPEER>(
		FN_CGOBJPC_ALLOC_MSG_FOR_PEER_OFFSET
		);

	//DetourTransactionBegin();
	//DetourAttach(&(PVOID&)s_pfnCGObjPC_AllocMsgForPeer, CNetHelper::MyCGObjPC_AllocMsgForPeer);
	//DetourTransactionCommit();
}


CMsg* CNetHelper::AllocMsg(WORD wMsgID, bool bEncrypted)
{
	CMsg* pMsg = s_pfnAllocMsg(g_pNetEngine, bEncrypted);
	pMsg->SetMsgID(wMsgID);
	return pMsg;
}

CMsg* CNetHelper::AllocMsgForBroadcast(WORD wMsgID)
{
	return s_pfnAllocMsgForBroadcast(wMsgID);
}

void CNetHelper::FreeMsg(CMsg* pMsg)
{
	s_pfnFreeMsg(g_pNetEngine, pMsg);
}

void CNetHelper::SendMsgToSM(CMsg* pMsg)
{
	__asm pushad;
	__asm pushfd;

	//idk cx
	__asm mov esi, pMsg;
	__asm call FN_SEND_MSG_TO_SM_OFFSET;

	__asm popfd;
	__asm popad;
}

void CNetHelper::SendMsgToPartyMembers(CParty* pParty, CMsg* pMsg, int nSenderGID)
{
	reinterpret_cast<void(__thiscall*)(CParty*, CMsg*, int)>(0x005BB770)(pParty, pMsg, nSenderGID);
}

void CNetHelper::SendMsgToGuildMembers(CGuild* pGuild, CMsg* pMsg, int nSenderID)
{
	reinterpret_cast<void(__thiscall*)(CGuild*, CMsg*, int)>(0x005C4260)(pGuild, pMsg, nSenderID);
}

void CNetHelper::SendMsgToTrainingCampMembers(CTrainingCamp* pCamp, CMsg* pMsg, int nSenderID)
{
	reinterpret_cast<void(__thiscall*)(CTrainingCamp*, CMsg*, int)>(0x005DE1E0)(pCamp, pMsg, nSenderID);
}

void CNetHelper::SendMsgToPC(CGObjPC* pPC, CMsg* pMsg)
{
	s_pfnExecHandlerForCGObjPC(pPC, pMsg);
	CNetHelper::FreeMsg(pMsg);

#if 1 == 0
	CMsgStreamBuffer* pNoticeMsg = pPC->AllocMsgForPeer(MSGID_FILTER_OR_GS_TO_CLIENT);
	pNoticeMsg->Write<E_FILTER_OR_GS_TO_CLIENT_MSG_TYPE::Enum>(E_FILTER_OR_GS_TO_CLIENT_MSG_TYPE::SHOW_TOP_MESSAGE);
	pNoticeMsg->WriteStringA("Top warning msg here");
	pNoticeMsg->Write<BYTE>(1);
	pPC->SendMsgToPeer(pNoticeMsg);
#endif
}

void CNetHelper::BindStreamBufferWithMsg(CMsg* pMsg)
{
	__asm pushad;
	__asm pushfd;

	__asm mov esi, pMsg;
	__asm mov edi, g_CStreamBufferUnk; //see IDA (CStreamBuffer related)
	__asm mov edx, 0x42C8F0; //call addr
	__asm call edx;

	__asm popfd;
	__asm popad;
}

void CNetHelper::FlushStreamBufferMsg(CMsg* pMsg)
{
	__asm pushad;
	__asm pushfd;

	__asm mov eax, pMsg;
	__asm mov esi, g_CStreamBufferUnk;
	__asm mov edx, 0x42C820;
	__asm call edx;

	__asm popfd;
	__asm popad;
}

CMsg* CNetHelper::CopyMsg(CMsg* pSrcMsg)
{
	CMsg* pNewMsg = CNetHelper::AllocMsg(pSrcMsg->GetMsgId(), false);

	//WORD wOldReadPos = pSrcMsg->GetReadPos();
	//pSrcMsg->SetReadPos(6);
	//int nLen = pSrcMsg->GetWritePos() - pSrcMsg->GetReadPos();
	//BYTE* pBuffer = new BYTE[nLen];
	//pSrcMsg->Read(pBuffer, nLen);
	//pNewMsg->Write(pBuffer, nLen);
	//delete[] pBuffer;

	return pNewMsg;
}

CMsg* CNetHelper::CopyMsg(CMsg* pSrcMsg, CGObjPC* pPC)
{
	CMsg* pNewMsg;

	//WORD wOldReadPos = pSrcMsg->GetReadPos();
	//pSrcMsg->SetReadPos(6);
	//int nLen = pSrcMsg->GetWritePos() - pSrcMsg->GetReadPos();
	//BYTE* pBuffer = new BYTE[nLen];
	//pSrcMsg->Read(pBuffer, nLen);
	//pNewMsg->Write(pBuffer, nLen);
	//delete[] pBuffer;

	return pNewMsg;
}