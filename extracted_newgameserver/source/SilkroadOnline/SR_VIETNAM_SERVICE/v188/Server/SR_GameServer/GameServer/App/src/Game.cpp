//
// Created by kyuubi09 on 3/30/2023.
//

#include "Game.h"

#include "Helpers.h"

#include "ServerFramework/ServerFramework.h"
#include "BSObj/BSObj.h"

#include "World/GameWorldMgr.h"
#include "ReferenceData/ReferenceDataMgr.h"
#include <Objects/GObjMob.h>
#include <SqlConnection/sqlCon.h>
#include <NetHelper.h>

CGObjPC *CGame::GetCharObjSTAT(const char *pName) {
    if (g_pCGame == NULL)
        throw;

    return g_pCGame->GetCharObjByName(pName);
}

CGObjPC *CGame::GetCharObjSTAT(DWORD dwCharId) {
    if (g_pCGame == NULL)
        throw;

    return g_pCGame->GetCharObjById(dwCharId);
}

CGObjPC *CGame::GetCharObjJID_STAT(DWORD dwAccountJID) {
    if (g_pCGame == NULL)
        throw;

    return g_pCGame->GetCharObjByJID(dwAccountJID);
}

CGObj *CGame::GetGameObjSTAT(DWORD dwGameId) {
    if (g_pCGame == NULL)
        throw;

    return g_pCGame->GetGameObjById(dwGameId);
}

CGObjPC *CGame::GetCharObjByName(const char *pName) {
    std::list<std::pair<std::string, CGObjPC *>>::const_iterator it = m_listPcCharName.begin();
    for (; it != m_listPcCharName.end(); it++) {
        if ((*it).first == pName) // Doğru karşılaştırma
            return ((*it).second);
    }

    return NULL;
}

CGObjPC *CGame::GetCharObjById(DWORD dwCharId) {
    std::map<DWORD, CGObjPC *>::const_iterator it = m_mapPcCharId.find(dwCharId);
    // If we got it
    if (it != m_mapPcCharId.end())
        return ((*it).second);

    return NULL;
}

void CGame::ToTownObjByWorldId(DWORD dwWorldID) {
   
    for (std::map<DWORD, CGObjPC*>::const_iterator it = m_mapPcCharId.begin(); it != m_mapPcCharId.end(); it++)
    {
        SWorldID mineWorldID;
        (*it).second->GetWorldID(mineWorldID);

        if (mineWorldID.wLayerID == dwWorldID)
        {
            CGObjPC* pC = (*it).second;
            if (pC != NULL)
            {
                pC->TeleportToTown();

            }
         
        }

    }
}

void CGame::ToTownbyWorldAndLayerID(DWORD dwWorldID, WORD LayerID) {

    for (std::map<DWORD, CGObjPC*>::const_iterator it = m_mapPcCharId.begin(); it != m_mapPcCharId.end(); it++)
    {
        SWorldID mineWorldID;
        (*it).second->GetWorldID(mineWorldID);

        if (mineWorldID.wLayerID == dwWorldID && mineWorldID.wWorldID == LayerID)
        {
            CGObjPC* pC = (*it).second;
            if (pC != NULL)
            {
                pC->TeleportToTown();

            }

        }

    }
}

CGObjPC *CGame::GetCharObjByJID(DWORD dwAccountJID) {
    std::map<DWORD, CGObjPC *>::const_iterator it = m_mapPcJID.find(dwAccountJID);
    // If we got it
    if (it != m_mapPcJID.end())
        return ((*it).second);

    return NULL;
}

CGObj *CGame::GetGameObjById(DWORD dwGameId) {
    std::list<std::pair<DWORD, CGObj *>>::const_iterator it = m_listObjGameId.begin();
    for (; it != m_listObjGameId.end(); it++) {
        if ((*it).first == dwGameId)
            return ((*it).second);
    }

    return NULL;
}
#define ADHOC_MAX_SLOTS (13 + 96)
#define ADHOC_MAX_PLUS 250

std::map<int, int> SpawnedMobList;

void CGame::OnJustShowError(CMsg& MSG) {
    WORD wNotifyType;
    CMsg* pAckMsg;
    DWORD dwRefUniqueID;

    MSG >> wNotifyType;
    switch (wNotifyType) {
    case SR_NOTIFY_ROC_QUEST: {
        std::string strQuestMsg;
        MSG >> strQuestMsg;

        pAckMsg = g_pNetEngine->AllocNewMsg(false);
        pAckMsg->SetMsgID(SR_NOTIFY);
        *pAckMsg << WORD(SR_NOTIFY_ROC_QUEST) << strQuestMsg;

        break;
    }
    case SR_NOTIFY_UNIQUE_KILLED: {
        std::string strUniqueKiller;

        MSG >> dwRefUniqueID >> strUniqueKiller;

        pAckMsg = g_pNetEngine->AllocNewMsg(false);
        pAckMsg->SetMsgID(SR_NOTIFY);
        *pAckMsg << WORD(SR_NOTIFY_UNIQUE_KILLED) << dwRefUniqueID << strUniqueKiller;

        std::string strUniqueCodeName("UnknownUnique");

        const RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(dwRefUniqueID);
        if (pRefObj != NULL)
            strUniqueCodeName = pRefObj->m_strObjectCode;

        BS_INFO("Unique Monster Killed! UNIQUE[%s] by [%s]", strUniqueCodeName.c_str(), strUniqueKiller.c_str())
            break;
    }
    case SR_NOTIFY_UNIQUE_SPAWN: {
        WORD wRegionID;
        float fUniquePosX;
        float fUniquePosY;
        float fUniquePosZ;

        MSG >> dwRefUniqueID >> wRegionID >> fUniquePosX >> fUniquePosY >> fUniquePosZ;

        pAckMsg = g_pNetEngine->AllocNewMsg(false);
        pAckMsg->SetMsgID(SR_NOTIFY);
        *pAckMsg << WORD(SR_NOTIFY_UNIQUE_SPAWN) << dwRefUniqueID;

        std::string strUniqueCodeName("UnknownUnique");

        const RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(dwRefUniqueID);
        if (pRefObj != NULL)
            strUniqueCodeName = pRefObj->m_strObjectCode;

        BS_INFO("Unique Monster Entered! UNIQUE[%s] POS[rid:%d (%.2f,%.2f,%.2f)]", strUniqueCodeName.c_str(),
            wRegionID, fUniquePosX, fUniquePosY, fUniquePosZ)
            break;
    }
    default:
        return;
    }

    g_pGameWorldMgr->SendMsgToAllGameWorldsUser(pAckMsg);
}

enum E_CHAT_TYPE : byte
{
    LOCAL_ALL               = 1,
    PRIVATE                 = 2,
    LOCAL_GM                = 3,
    PARTY                   = 4,
    GUILD                   = 5,
    GLOBAL                  = 6,
    NOTICE                  = 7,
    STALL                   = 9,
    UNION                   = 11,
    NPC                     = 13,
    ACADEMY                 = 16
};
#define READ_LOCK_INFO_FROM_SHARD 0x5061
#define READ_UNLOCK_INFO_FROM_SHARD 0x5063
void CGame::ProcessMessage(CMsg *pMsg) {
    try {
        // Handle the server packets here
        switch (pMsg->GetMsgId()) {
        case 0x8888:
        {
            byte Type;
            *pMsg >> Type;
            if (Type == 1) {
                int CharID;
                std::string GrantName;
                *pMsg >> CharID >> GrantName;
                CGObjPC * TargetChar = g_pCGame->GetCharObjById(CharID);
                if (TargetChar != NULL && TargetChar != 0)
                {
                    TargetChar->SetGrantName(&GrantName);
                }
            }
            else if (Type == 2) {
                int RefObjId;
                int GameWorldId;
                int RegionId;
                int PosX, PosY, PosZ, GenerateRadius;
                *pMsg >> RefObjId >> GameWorldId >> RegionId >> PosX >> PosY >> PosZ >> GenerateRadius;
                CGObjMob::CreateMob(RefObjId, GameWorldId + 0x10000, (uint16_t)RegionId, (float)PosX, (float)PosY, (float)PosZ, (float)GenerateRadius);
            }
            else if (Type == 3) {
                int CharID2;
                int RefObjId2;
               
                *pMsg >> CharID2 >> RefObjId2;
                CGObjPC * TargetPC = g_pCGame->GetCharObjById(CharID2);
                if (TargetPC != NULL)
                {
                    SPosInfo minePos;
                    TargetPC->GetPosInfo(minePos);
                    SWorldID mineWorldID;
                    TargetPC->GetWorldID(mineWorldID);

                    CGObjMob::CreateMob(RefObjId2, mineWorldID.wWorldID + 0x10000, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
                }
            }
            else if (Type == 4) {
                int RefObjID;
                *pMsg >> RefObjID;
                CGObjMob::DeleteMonster(RefObjID);
            }
            else if (Type == 5) {
                int WorldID;
                *pMsg >> WorldID;
                CGObjMob::DeleteMonsterByWorldID(WorldID);
            }
            else if (Type == 6) {
                int CharID;
                int SkillID;
                *pMsg >> CharID >> SkillID;
                CGObjPC* TargetChar = g_pCGame->GetCharObjById(CharID);
                if (TargetChar != NULL && TargetChar != 0)
                {
                    TargetChar->LiveSkill(SkillID); // limitless
                }
            }
            else if (Type == 7) {
                int CharID;
                int SkillID;
                *pMsg >> CharID >> SkillID;
                CGObjPC* TargetChar = g_pCGame->GetCharObjById(CharID);
                if (TargetChar != NULL && TargetChar != 0)
                {
                    TargetChar->CancelBuff(SkillID); // limitless
                }
            }
            else if (Type == 8) {
                int CharID;
                char SkillCodeName[128];
                *pMsg >> CharID >> SkillCodeName;
                CGObjPC* TargetChar = g_pCGame->GetCharObjById(CharID);
                if (TargetChar != NULL && TargetChar != 0)
                {
                    TargetChar->EngageBuffSkill(TargetChar, SkillCodeName); // limitless
                }
            }
            else if (Type == 10) {
                int CharID;
                *pMsg >> CharID;
                CGObjPC* TargetChar = g_pCGame->GetCharObjById(CharID);
                if (TargetChar != NULL && TargetChar != 0)
                {
                    TargetChar->TeleportToTown();
                }
            }
            else if (Type == 11) {
                int WorldID;
                *pMsg >> WorldID;
                ToTownObjByWorldId(WorldID);
            }
            else if (Type == 12)
            {
                unsigned int PetUQID;
                int PetSkillID;
                *pMsg >> PetUQID >> PetSkillID;
                
                CGObjCOS_GoldPet* pc = static_cast<CGObjCOS_GoldPet*>(GetGameObjById(PetUQID));
                if (pc != NULL)
                {
                    pc->LiveSkill2(PetSkillID);
                }
            }
            else if (Type == 13) {
                int CharID;
                int CapeID;
                *pMsg >> CharID >> CapeID;
                CGObjPC* pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    pPC->UpdatePVPCapeType(CapeID);
                }
                
            }
            else if (Type == 14) {
                int CharID = 0;
                int GameWorldId = 0;
                int RegionId = 0;
                int PosX = 0;
                int PosY = 0;
                int PosZ = 0;
                *pMsg >> CharID >> GameWorldId >> RegionId >> PosX >> PosY >> PosZ;
                CGObjPC* pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    if (!pPC->MoveTo(GameWorldId + 0x10000, RegionId, PosX, PosY, PosZ, 2)) {
                        pPC->MoveTo(GameWorldId + 0x10000, RegionId, PosX, PosY, PosZ, 1);
                    }
                }
            }
            else if (Type == 15) {
                int CharID = 0;
                *pMsg >> CharID;
                CGObjPC* pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                        pPC->Ressurect(3000, 3000);
                }
            
            }
            else if (Type == 16)
            {
                int CharIDs;
                unsigned int Exp;
                *pMsg >> CharIDs >> Exp;
                CGObjPC* pPC = g_pCGame->GetCharObjById(CharIDs);
                if (pPC != NULL)
                {
                    pPC->FuncJobExpTest(Exp);
                }
            }
            else if (Type == 17)
            {
                int CharID;
                int Slot;
                std::string ItemCode;
                *pMsg >> CharID;
                *pMsg >> Slot;
                *pMsg >> ItemCode;

                const char* p = ItemCode.c_str();
                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL) {
                    if (pPC->GetItemChar(Slot) != NULL) {
                        pPC->SetLiveItem(Slot, p);
                    }
                }
            }
            else if (Type == 18)
            {
                int CharID;
                int Slot;
                int Amount;
                *pMsg >> CharID;
                *pMsg >> Slot;
                *pMsg >> Amount;
                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    if(pPC->GetItemChar(Slot) != NULL)
                    {
                        pPC->SetLiveDeleteItem(Slot, Amount);
                    }

                }
            }
            else if (Type == 19)
            {
                int CharID;
                int MutateSlot;
                std::string ItemCode;
                int ConsumeSlot;
                int ConsumeAmount;
                *pMsg >> CharID;

                *pMsg >> MutateSlot;
                *pMsg >> ItemCode;
                *pMsg >> ConsumeSlot;
                *pMsg >> ConsumeAmount;
                const char* p = ItemCode.c_str();
                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    if(pPC->GetItemChar(ConsumeSlot) != NULL && pPC->GetItemChar(MutateSlot) != NULL)
                    {
                        pPC->SetLiveDeleteItem(ConsumeSlot, ConsumeAmount);
                        pPC->SetLiveItem(MutateSlot, p);
                    }

                }
            }
            else if (Type == 20)
            {
                int CharID;
                int nSilk, nSilkGift, nSilkPoint;
                *pMsg >> CharID;
                *pMsg >> nSilk >> nSilkGift >> nSilkPoint;

                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    pPC->UpdateSilk(nSilk, nSilkGift, nSilkPoint, true);
                }
            }
            else if (Type == 21)
            {
                int CharID;
                __int64 nGold;
                int AddOrRemove;
                *pMsg >> CharID;
                *pMsg >> nGold;
                *pMsg >> AddOrRemove;
                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if (pPC != NULL)
                {
                    if(AddOrRemove == 1)
                    {
                        pPC->UpdateGold(nGold);
                    }
                    else if(AddOrRemove == 0)
                    {
                        INT64 availablegold = pPC->m_pObjDataInstance->Gold;
                        if (availablegold >= nGold) {
                            pPC->RemoveGold(nGold);
                        }
                    }
                }
            }
            else if (Type == 22)
            {
                int WorldID;
                int LayerID;
                *pMsg >> WorldID;
                *pMsg >> LayerID;
                ToTownbyWorldAndLayerID(WorldID, LayerID);
            }
            else if (Type == 131)
            {
                int CharID;
                *pMsg >> CharID;

                int ItemID;
                *pMsg >> ItemID;

                byte nPlus;
                *pMsg >> nPlus;

                byte ItemSlot;
                *pMsg >> ItemSlot;

                byte AdvLevel;
                *pMsg >> AdvLevel;


                CGObjPC * pPC = g_pCGame->GetCharObjById(CharID);
                if(pPC != NULL)
                {
                    CGItem* pItem = pPC->GetItemChar(ItemSlot);
                    if (pItem != NULL)
                    {
                        if(pItem->InstanceItem->RefItemID == ItemID)
                        {
                            CMsg* pTmpMsg = CNetHelper::AllocMsg(0x0000, false);
                            CNetHelper::BindStreamBufferWithMsg(pTmpMsg);
                            if ((pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 2
                                 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 1 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 1) ||
                                (pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 3
                                 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 3 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 7)
                                || (pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 2
                                    && pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 1 && pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 2))
                            {
                                pItem->WriteItemCosDataMsg((void*)g_CStreamBufferUnk, 1);
                            }
                            else
                            {
                                pItem->WriteItemDataToMsg((void*)g_CStreamBufferUnk, 1);
                            }

                            CNetHelper::FlushStreamBufferMsg(pTmpMsg);
                            int nLen = pTmpMsg->m_wWriteDataArrayPos - 6;
                            //===============================================================
                            //read item data from dummy msg
                            BYTE* pItemDataBuf = new BYTE[nLen];

                            pTmpMsg->SetReadPos(6);
                            pTmpMsg->Read(pItemDataBuf, nLen);

                            CMsg* pSmMsg = pPC->AllocMsg(0x5034);

                            pSmMsg->Write<int>(nLen);
                            pSmMsg->Write(pItemDataBuf, nLen);
                            *pSmMsg << pItem->InstanceItem->RefItemID;
                            *pSmMsg << nPlus;
                            *pSmMsg << AdvLevel;
                            pPC->SendMsg(pSmMsg);
                            delete[] pItemDataBuf;
                            CNetHelper::FreeMsg(pTmpMsg);

                        }

                    }
                }

            }
            else if (Type == 22222)
            {
                int CharID;
                *pMsg >> CharID;
                int nItemID64;
                *pMsg >> nItemID64;
                int NewPlus;
                *pMsg >> NewPlus;
                CGObjPC* pPC = g_pCGame->GetCharObjById(CharID);
                for (int i = 0; i < ADHOC_MAX_SLOTS; i++)
                {
                    CGItem* pItem = (CGItem*)pPC->GetItemAtStorageSlot(i);
                    if (pItem == NULL)
                        continue;

                    CInstanceItem* pInstance = pItem->InstanceItem;
                    if (pInstance == NULL)
                        continue;

                    //int so we can check >= <= properly
                    

                    //wtf u doing?
                    if (NewPlus >= ADHOC_MAX_PLUS || NewPlus < 0)
                        NewPlus = ADHOC_MAX_PLUS;

                    if (pInstance->ID64 == nItemID64)
                    {
                        pInstance->SetPlus((BYTE)NewPlus);
                        pItem->RefreshItemStats();

                        CMsg* pMsg = pPC->AllocMsgForPeer(0x3040);
                        *pMsg << BYTE(i);
                        *pMsg << BYTE(2); //flag opt lvl
                        *pMsg << ((BYTE)NewPlus);
                        pPC->SendMsgToPeer(pMsg);

                      /*  CShardDBSet* pShardDbSet = CDbMgr::GetShardDbSet();

                        char szQuery[256] = { 0 };
                        sprintf(szQuery, "UPDATE _Items SET OptLevel = %d WHERE ID64 = %lld",
                            nNewPlus, stRecord.nItemID64);

                        if (!pShardDbSet->TryExecNonQuery(szQuery))
                            LOG_WRITE(LOG_ERROR, "%s - What the fuck ?! Query failed, query = %s",
                                __FUNCTION__, szQuery);*/



                        break;
                    }
                }
            }

        }
        break;
            case READ_LOCK_INFO_FROM_SHARD: // locked item added all gameservers
            {
                INT64 ItemID64;
                *pMsg >> ItemID64;
                if (CSqlCon::LockedItemList.find(ItemID64) == CSqlCon::LockedItemList.end())
                {
                    CSqlCon::LockedItemList.insert((std::make_pair(ItemID64, 0)));
                }
            }
                break;
            case READ_UNLOCK_INFO_FROM_SHARD: // unlocked items removed all
            {
                INT64 ItemID64;
                *pMsg >> ItemID64;
                if (CSqlCon::LockedItemList.find(ItemID64) != CSqlCon::LockedItemList.end())
                {
                    CSqlCon::LockedItemList.erase(ItemID64);
                }
            }
                break;
            case 0x5066:
            {
                int CharID;
                INT64 ID64;
                int OldOptLevel;
                long enddtime;

                *pMsg >> CharID;
                *pMsg >> ID64;
                *pMsg >> OldOptLevel;
                *pMsg >> enddtime;

                if (CSqlCon::TimedItemList.find(ID64) == CSqlCon::TimedItemList.end())
                {
                    STimedItemPlusDbRecord pRecord = STimedItemPlusDbRecord();
                    pRecord.CharID = CharID;
                    pRecord.dwItemID64 = ID64;
                    pRecord.CurrentPlus = OldOptLevel;
                    pRecord.endTime = enddtime;
                    CSqlCon::TimedItemList.insert(std::make_pair(ID64, pRecord));
                }
            }
                break;
            case 0x5068:
            {
                INT64 ID64;

                *pMsg >> ID64;


                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CSqlCon::TimedItemList.erase(ID64);
                }
            }
                break;
            case 0x5069:
            {
                int SenderCharID;
                pMsg->Read<int>(SenderCharID);


                byte eChatType;
                pMsg->Read<BYTE>(eChatType);

                byte btChatIndex;
                pMsg->Read<BYTE>(btChatIndex);


                std::string strReceiverName;
                if(eChatType == PRIVATE) {
                    pMsg->ReadString(strReceiverName);

                }

                std::string strAsciiText;
                pMsg->ReadString(strAsciiText);


                byte LinkedItemSlot;
                pMsg->Read<BYTE>(LinkedItemSlot);
/*
                if (eChatType == E_CHAT_TYPE::PRIVATE)
                {
                    wchar_t szBuffer[512] = { 0 };
                    wsprintf(szBuffer, L"%s(TO):%s", std::wstring(strReceiverName.begin(), strReceiverName.end()).c_str(), strMsgText.c_str());
                    strMsgText = szBuffer;
                }

                CMsgStreamBuffer* pClientMsg = BuildChatItemLinkClientMsg(eChatType, strSenderName, strMsgText, nGameID, vItemInfo);

                if (eChatType == E_CHAT_TYPE::LOCAL_ALL || eChatType == E_CHAT_TYPE::LOCAL_GM)
                {
                    CGObjPC* pPC = GetCharObjById(nDBID);

                    if(pPC != NULL)
                        pPC->BroadcastMsgToNearbyPlayers(pClientMsg);

                }

                if (eChatType == E_CHAT_TYPE::PARTY)
                {
                    __OBJ_MGR_PLAYER_LST lstParty = CObjMgr::GetPartyMembers(nIdentifier);
                    __OBJ_MGR_PLAYER_LST_IT itMember = lstParty.begin();

                    for (; itMember != lstParty.end(); itMember++)
                    {
                        CGObjPC* pMember = *itMember;
                        CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pMember);
                        pMember->SendMsgToPeer(pMemberMsg);


                        CMsgStreamBuffer* pBotMsg = pMember->AllocMsgForPeer(0x3026);
                        pBotMsg->Write(eChatType);
                        pBotMsg->WriteStringA(strSenderName);
                        pBotMsg->WriteStringA(std::string(strMsgTextOriginal.begin(), strMsgTextOriginal.end()));
                        pMember->SendMsgToPeer(pBotMsg);
                    }
                }

                if (eChatType == E_CHAT_TYPE::GUILD)
                {
                    __OBJ_MGR_PLAYER_LST lstGuild = CObjMgr::GetGuildMembers(nIdentifier);
                    __OBJ_MGR_PLAYER_LST_IT itMember = lstGuild.begin();

                    for (; itMember != lstGuild.end(); itMember++)
                    {
                        CGObjPC* pMember = *itMember;
                        CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pMember);
                        pMember->SendMsgToPeer(pMemberMsg);
                    }
                }

                if (eChatType == E_CHAT_TYPE::UNION)
                {
                    __OBJ_MGR_PLAYER_LST lstAlliance = CObjMgr::GetAllianceMembers(nIdentifier);
                    __OBJ_MGR_PLAYER_LST_IT itMember = lstAlliance.begin();

                    for (; itMember != lstAlliance.end(); itMember++)
                    {
                        CGObjPC* pMember = *itMember;
                        CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pMember);
                        pMember->SendMsgToPeer(pMemberMsg);
                    }
                }

                if (eChatType == E_CHAT_TYPE::ACADEMY)
                {
                    __OBJ_MGR_PLAYER_LST lstAcademy = CObjMgr::GetTrainingCampMembers(nIdentifier);
                    __OBJ_MGR_PLAYER_LST_IT itMember = lstAcademy.begin();

                    for (; itMember != lstAcademy.end(); itMember++)
                    {
                        CGObjPC* pMember = *itMember;
                        CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pMember);
                        pMember->SendMsgToPeer(pMemberMsg);
                    }
                }

                if (eChatType == E_CHAT_TYPE::GLOBAL)
                {
                    //*must* use CObjMgr to get by gid only on this GS
                    IGObj* pPC = CObjMgr::GetPlayerByGameID(nGameID);
                    int nLastGlobalRefItemID = nIdentifier;

                    if (pPC != NULL)
                    {
                        if (CRefDataManager::IsGlobalChatMultiShard(nLastGlobalRefItemID))
                        {
                            CMiscDBSet* pDbSet = CDbMgr::GetMiscDbSet();

                            WORD wOldReadPos = pClientMsg->GetReadPos();
                            pClientMsg->SetReadPos(6);
                            int nDataLen = pClientMsg->GetWritePos() - pClientMsg->GetReadPos();
                            BYTE* pBuffer = new BYTE[nDataLen];
                            pClientMsg->Read(pBuffer, nDataLen);
                            pClientMsg->SetReadPos(wOldReadPos);

                            pDbSet->AddAutoNetMsgBroadcastRecord(pClientMsg->GetID(), false, false, pBuffer, nDataLen);

                            delete[] pBuffer;
                        }
                    }

                    //Not a multi-shard global, so simply send msg to all players
                    if (!CRefDataManager::IsGlobalChatMultiShard(nLastGlobalRefItemID))
                    {
                        CObjMgr::GetPlayerListACS()->Enter();

                        __OBJ_MGR_PLAYER_LST& lstPlayers = CObjMgr::GetPlayers();
                        __OBJ_MGR_PLAYER_LST_IT itPlayer = lstPlayers.begin();

                        for (; itPlayer != lstPlayers.end(); itPlayer++)
                        {
                            CGObjPC* pPlayer = *itPlayer;
                            CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pPlayer);
                            pPlayer->SendMsgToPeer(pMemberMsg);
                        }

                        CObjMgr::GetPlayerListACS()->Leave();
                    }
                }

                if (eChatType == E_CHAT_TYPE::PRIVATE)
                {
                    CGObjPC* pSender = CObjMgr::GetPlayerByDBID(nDBID);
                    CGObjPC* pTarget = CObjMgr::GetPlayerByCharName(strReceiverName);

                    if (pSender != NULL)
                        pSender->SendMsgToPeer(CNetHelper::CopyMsg(pClientMsg, pSender));

                    if (pTarget != NULL)
                    {
                        wchar_t szBuffer[512] = { 0 };
                        wsprintf(szBuffer, L"%s(FROM):%s", std::wstring(strSenderName.begin(), strSenderName.end()).c_str(), strMsgTextOriginal.c_str());

                        CMsgStreamBuffer* pClientMsgForTarget = BuildChatItemLinkClientMsg(eChatType, strSenderName, szBuffer, nGameID, vItemInfo);
                        pTarget->SendMsgToPeer(CNetHelper::CopyMsg(pClientMsgForTarget, pTarget));
                    }
                }

                if (eChatType == E_CHAT_TYPE::STALL)
                {
                    __OBJ_MGR_PLAYER_LST lstStallMembers = CObjMgr::GetStallMembers(nIdentifier);
                    __OBJ_MGR_PLAYER_LST_IT itMember = lstStallMembers.begin();

                    for (; itMember != lstStallMembers.end(); itMember++)
                    {
                        CGObjPC* pMember = *itMember;
                        CMsgStreamBuffer* pMemberMsg = CNetHelper::CopyMsg(pClientMsg, pMember);
                        pMember->SendMsgToPeer(pMemberMsg);
                    }
                }
*/
         /*       CGObjPC *Sender = GetCharObjById(SenderCharID);
                if (Sender != NULL) {


                    if(eChatType == PRIVATE) {
                        CGObjPC *TargetChar = GetCharObjByName(strReceiverName.c_str());
                        if(TargetChar != NULL) {
                            printf("TargetCharIs Found %p\n", TargetChar);

                            CGItem* pItem = Sender->GetItemChar(LinkedItemSlot);
                            if (pItem != NULL) {
                                printf("Item found in slot %d\n", LinkedItemSlot);

                                CMsg* pTmpMsg = CNetHelper::AllocMsg(0x0000, false);
                                CNetHelper::BindStreamBufferWithMsg(pTmpMsg);

                                if ((pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 2 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 1 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 1) ||
                                    (pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 3 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 3 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 7) ||
                                    (pItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 2 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID3() == 1 &&
                                     pItem->InstanceItem->pCRefObjItem->TID.getTypeID4() == 2))
                                {
                                    pItem->WriteItemCosDataMsg((void*)g_CStreamBufferUnk, 1);
                                } else {
                                    pItem->WriteItemDataToMsg((void*)g_CStreamBufferUnk, 1);
                                }

                                CNetHelper::FlushStreamBufferMsg(pTmpMsg);
                                int nLen = pTmpMsg->m_wWriteDataArrayPos - 6;
                                printf("nLen: %d\n", nLen);

                                // Read item data from dummy msg
                                BYTE* pItemDataBuf = new BYTE[nLen];
                                pTmpMsg->SetReadPos(6);
                                pTmpMsg->Read(pItemDataBuf, nLen);

                                std::string sendercname = Sender->GetCharName();
                                CMsg* PacketForTarget = TargetChar->AllocMsg(0x705E);
                                *PacketForTarget << eChatType;
                                PacketForTarget->WriteString(sendercname);
                                PacketForTarget->WriteString(strAsciiText);
                                PacketForTarget->Write<int>(nLen);
                                PacketForTarget->Write(pItemDataBuf, nLen);
                                TargetChar->SendMsg(PacketForTarget);
                                printf("Sent packet to target.\n");


                                std::string sendercnametarget = TargetChar->GetCharName();
                                CMsg* PacketForTargetX = Sender->AllocMsg(0x705E);
                                *PacketForTargetX << eChatType;
                                PacketForTargetX->WriteString(sendercnametarget);
                                PacketForTargetX->WriteString(strAsciiText);
                                PacketForTargetX->Write<int>(nLen);
                                PacketForTargetX->Write(pItemDataBuf, nLen);
                                Sender->SendMsg(PacketForTargetX);
                                printf("Sent packet to target.\n");

                                delete[] pItemDataBuf;
                                CNetHelper::FreeMsg(pTmpMsg);
                            }
                        }
                    }
                } */
            }
            break;
        }
    }
    catch (...) {
        pMsg->FlushRemainingBytes();
        WORD wMsgId = pMsg->GetMsgId();

        ServerFramework::ReportLog(LOG_TYPE_NOTIFY, " An Exception occurred in CGame::ProcessMessage() MsgID : %x",
                                   wMsgId);
        return;
    }
    reinterpret_cast<void (__stdcall *)(CGame *, CMsg *)>(0x00414260)(this, pMsg);
}

