//
// Created by YUMBUL on 19.03.2023.
//

#include <Game.h>
#include "GObjPC.h"
#include "GObjMob.h"
#include <sstream>
#include <memory/MemoryUtility.h>
#include <BSObj/BSObj.h>
#include <Objects/GItem.cpp>
#include <SqlConnection/sqlCon.h>
#include <time.h>
#include <ReferenceData/ReferenceDataMgr.h>
#include <NetHelper.h>
#include <ctime>


CGObjPC::FN_GETITEMATSTORAGESLOT CGObjPC::s_pfnGetItemAtStorageSlot;
CGObjPC::FN_OFFSET_EXP_POINT CGObjPC::s_pfnOffsetExpPoint;
#define OFFSET_EXP_POINT_FN_OFFSET					0x004E5250
#define GET_ITEM_AT_MAIN_STORAGE_SLOT_FN_OFFSET		0x004A68E0

IGObj* CGObjPC::GetItemAtStorageSlot(int nSlotIndex)
{
    return s_pfnGetItemAtStorageSlot(this, nSlotIndex);
}
void CGObjPC::Setup()
{
    s_pfnGetItemAtStorageSlot = reinterpret_cast<FN_GETITEMATSTORAGESLOT>(GET_ITEM_AT_MAIN_STORAGE_SLOT_FN_OFFSET);
    s_pfnOffsetExpPoint = reinterpret_cast<FN_OFFSET_EXP_POINT>(OFFSET_EXP_POINT_FN_OFFSET);
}
#define TELEPORT_TO_TARGET_POINT_FN_OFFSET			0x004DF290

void CGObjPC::GiveExp(DWORD dwExpSourceObjUniqueID, long long qwLevelExp, long long qwSkillExp, DWORD& dwUnk)
{
    if (qwLevelExp < 0 && this->GetLevel() == 101)
    {
        qwLevelExp = 0;
    }
    reinterpret_cast<void(__thiscall*)(CGObjPC*, DWORD, long long, long long, DWORD&)>(0x004E5250)(this, dwExpSourceObjUniqueID, qwLevelExp, qwSkillExp, dwUnk);
}
void CGObjPC::Ressurect(float fRecoverHpMult, float fRecoverMpMult)
{
    ////Ressurect first..
    const DWORD dwCallAddr = TELEPORT_TO_TARGET_POINT_FN_OFFSET;

    __asm pushad;
    __asm pushfd;

    __asm push 1;
    __asm mov eax, dword ptr[this];
    __asm call dwCallAddr;

    __asm popfd;
    __asm popad;

    //Update HP MP
    this->UpdateHpMp(this->GetMaxHealth() * fRecoverHpMult, this->GetMaxMana() * fRecoverMpMult);
}
void CGObjPC::UpdateHpMp(int nHp, int nMp)
{
    reinterpret_cast<void(__stdcall*)(CGObjPC*, int, int, int)>(0x004A86A0)(this, nHp, nMp, 8);
}

void CGObjPC::FUN_0048b810(void* p1, char p3)
{
    this->speed1 = 150;

    reinterpret_cast<void*(__thiscall*)(CGObjPC*, void*, char)>(0x0048b810)(this, p1, p3);
}

void CGObjPC::UseItemFunction(unsigned int *pMsg) {
    //printf("%d \n", this);
    return reinterpret_cast<void(__thiscall *)(CGObjPC *, unsigned int *)>(0x00510980)(this, pMsg);
}

unsigned int CGObjPC::Func_3911()
{

    unsigned int a = reinterpret_cast<unsigned int(__thiscall*)(CGObjPC*)>(0x004df9e0)(this);
    int is = m_PCInventory.m_nSlotsCount;
    for (int i = 0; i < is; i++)
    {
        if (this->GetItemChar(i) != NULL)
        {
            //printf("%s %d\n", this->GetItemChar(i)->GetCodeName(), i);
            INT64 ID64 = this->GetItemChar(i)->ID64;
            if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
            {
                CMsg* newpMsg = this->AllocMsg(0xF200);
                *newpMsg << byte(0x0);
                *newpMsg << byte(i);
                this->SendMsg(newpMsg);
            }
            if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
            {
                CMsg* pMsg2 = this->AllocMsg(0x5025);
                *pMsg2 << (byte)0;
                *pMsg2 << BYTE(i);
                *pMsg2 << (long)CSqlCon::TimedItemList[ID64].endTime;
                this->SendMsg(pMsg2);
            }
            if (CSqlCon::STimedDevillList.find(ID64) != CSqlCon::STimedDevillList.end())
            {
                CMsg* pMsg2 = this->AllocMsg(0x5025);
                *pMsg2 << (byte)0;
                *pMsg2 << BYTE(i);
                *pMsg2 << (long)CSqlCon::STimedDevillList[ID64].endTime;
                this->SendMsg(pMsg2);
            }
        }
    }
    int irr = 0;
    for (std::vector<CGItem*>::iterator it = m_PCAvatarInventory.m_vecItems.begin(); it != m_PCAvatarInventory.m_vecItems.end(); it++)
    {
        irr++;
        if ((*it) != NULL)
        {


            if (CSqlCon::LockedItemList.find((*it)->InstanceItem->ID64) != CSqlCon::LockedItemList.end())
            {
                CMsg* newpMsg = this->AllocMsg(0xF200);
                byte type = 1;
                *newpMsg << (byte)type;
                *newpMsg << irr;
                this->SendMsg(newpMsg);
            }
            if (CSqlCon::STimedDevillList.find((*it)->InstanceItem->ID64) != CSqlCon::STimedDevillList.end())
            {
                CMsg* pMsg2 = this->AllocMsg(0x5025);
                *pMsg2 << (byte)2;
                *pMsg2 << BYTE(irr);
                *pMsg2 << (long)CSqlCon::STimedDevillList[(*it)->InstanceItem->ID64].endTime;
                this->SendMsg(pMsg2);

            }
        }

    }

    SWorldID mineWorldID;
    this->GetWorldID(mineWorldID);
    CMsg* info = this->AllocMsg(0x5035);
    *info << mineWorldID.wLayerID;
    *info << mineWorldID.wWorldID;
    this->SendMsg(info);

    return a;
}
void CGObjPC::Func_373(CMsg* p1, unsigned int p2)
{
    printf("%d == %d\n", p1, p2);
    reinterpret_cast<void(__thiscall *)(CGObjPC *, CMsg*, unsigned int)>(0x004e9440)(this, p1, p2);
}
void CGObjPC::Func_386(CMsg* pMsg)
{
    printf("%p = msgID = %p \n", pMsg, *pMsg->m_wpMsgId);
    //void * test = (void*)(this + 0x278)(0xb034);

    reinterpret_cast<void(__thiscall*)(CGObjPC*, CMsg*)>(0x0050f5c0)(this, pMsg);
}
void CGObjPC::CharacterStats()
{
    reinterpret_cast<void(__thiscall*)(CGObjPC*)>(0x004ebe30)(this);
}
int CGObjPC::Testnew2(int p2, int p3, float p4)
{
    //printf("%p\n", this);
    return reinterpret_cast<int(__thiscall*)(CGObjPC*, int, int, float)>(0x0040e830)(this, p2, p3, p4);

}

void CGObjPC::LivedpsMobAttackRecorder(int param_1, int p2, int p3) {

    //printf("%p %p %p %p %p \n", param_1, p2, p3);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, int, int, int)>(0x004aa640)(this, param_1, p2, p3);
}

void CGObjPC::NewX(int param_1)
{
    //printf("%p\n", this);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, int)>(0x0048e150)(this, param_1);
}

void CGObjPC::FUNCMovement(unsigned int param)
{

    //printf("%p 22222222222222222\n", param);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int)>(0x004b0ea0)(this, param);
}


void CGObjPC::FUN_00485bb0(unsigned int param1, unsigned int param2, unsigned int param3)
{
    //printf("%p !!! %p !!! %p\n", param1, param2, param3);
    this->speed1 = 100;
    this->speed2 = 150;
    if (this->GoldPetPtr != NULL)
    {
        this->GoldPetPtr->speed1 = 100;
        this->GoldPetPtr->speed2 = 150;
    }
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int, unsigned int)>(0x00485bb0)(this, param1, param2, param3);
}


void CGObjPC::Fun_004a9b70(unsigned int p1, unsigned int p2)
{
    //printf("%p %p\n", p1, p2);
    //return;
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int)>(0x004a73f0)(this, p1, 0);
}
void CGObjPC::FUN348(unsigned int param)
{
    //printf("sieeeeeeee 348 \n");
    this->speed1 = 100;
    this->speed2 = 150;
    if (this->GoldPetPtr != NULL)
    {
        this->GoldPetPtr->speed1 = 100;
        this->GoldPetPtr->speed2 = 150;
    }




    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int)>(0x004aa340)(this, param);
}

void CGObjPC::FUN343(unsigned int param1, unsigned int param2, unsigned int param3)
{
    //printf("%p !!! %p !!! %p\n", param1,param2,param3);
    this->speed1 = 100;
    this->speed2 = 150;
    if (this->GoldPetPtr != NULL)
    {
        this->GoldPetPtr->speed1 = 100;
        this->GoldPetPtr->speed2 = 150;
    }




    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int, unsigned int)>(0x004a9d00)(this, param1, param2, param3);
}

void CGObjPC::FUN_004aa410()
{

    reinterpret_cast<void* (__thiscall*)(CGObjPC*)>(0x004aa410)(this);
}

void CGObjPC::setspeed(float p1, float p2)
{
    this->speed1 = p1;
    this->speed2 = p2;
}

void CGObjPC::FUN_004b3000(unsigned int p1, unsigned int p2, float p3)
{
    //printf("%p ! %p ! %.2lf \n", p1, p2, p3);
    if (this->OnPet == 1)
    {
        p3 = 1000.00;
    }
    reinterpret_cast<void* (__thiscall*)(CGObjPC*, unsigned int p1, unsigned int p2, float p3)>(0x004b3000)(this, p1, p2, p3);
}

void ReplaceAll(std::string& Value, const std::string& From, const std::string& To) {
    if (From.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = Value.find(From.c_str(), start_pos)) != std::string::npos) {
        Value.replace(start_pos, From.length(), To.c_str());
        start_pos += To.length();// Increase the same length in case 'To' contains 'From', like replacing 'x' with 'yx'
    }
}
int find_whole(std::string sentence, std::string word) {
    size_t pos = sentence.find(word);
    size_t offset = pos + sentence.size() + 1;

    if ((pos != std::string::npos) && (sentence.substr(pos, offset) == word))
        return pos;
    return std::string::npos;
}
void CGObjPC::fun(unsigned int a1, unsigned int a2, unsigned int a3) {
    //printf("a1 %d,a2 %d, a3 %d\n", a1, a2, a3);
    reinterpret_cast<void(__thiscall *)(CGObjPC *, int, int, int)>(0x004A9C80)(this, a1, a2, a3);
}
int GetDegreeLevel(int itemClass) {
    return ((itemClass - 1) / 3 + 1);
}
tid_t TypeId::getTypeID1() const
{
    return (m_type_id_value & TID1_MASK)/4;
}
tid_t TypeId::getTypeID2() const
{
    return (m_type_id_value & TID2_MASK)/32;
}
tid_t TypeId::getTypeID3() const
{
    return (m_type_id_value & TID3_MASK)/128;
}
tid_t TypeId::getTypeID4() const
{
    return (m_type_id_value & TID4_MASK)/2048;
}

std::string wchar2string(wchar_t* str)
{
    std::string mystring;
    while (*str)
        mystring += (char)*str++;
    return  mystring;
}
#define SEND_SHARD_TO_LOCK_INFO 0x5060
#define SEND_SHARD_TO_UNLOCK_INFO 0x5062
#define TYPE_OF_ITEM_LOCKED 0
#define TYPE_OF_WRONG_PASSWORD 1

#define KEY_OF_SECRET "B4517142409MG!NEWFILTER"

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
int SuccesRateOptLevel_1 = 100;
int SuccesRateOptLevel_2 = 100;
int SuccesRateOptLevel_3 = 99;
int SuccesRateOptLevel_4 = 75;
int SuccesRateOptLevel_5 = 50;
int SuccesRateOptLevel_6 = 20;
int SuccesRateOptLevel_7 = 10;
int SuccesRateOptLevel_8 = 8;
int SuccesRateOptLevel_9 = 5;
int SuccesRateOptLevel_10 = 2;
int SuccesRateOptLevel_11 = 1;
int SuccesRateOptLevel_12 = 1;
int SuccesRateOptLevel_13 = 1;
int SuccesRateOptLevel_14 = 1;
int SuccesRateOptLevel_15 = 1;
enum eAlchemyResultType
{
    SUCCESS = 0,
    FAILED = 1,
    NOTHING = 2,
};
enum eAlchemyType
{
    WITH_PROOF = 0,
    WITHOUT_ROOF = 1
};
#define GRANT_NAME_PACKET 0x3500

void CGObjPC::ReaderPacket(CMsg* pMsg) {

    if (*pMsg->m_wpMsgId == GRANT_NAME_PACKET) {

        std::string NewGrantName;
        *pMsg >> NewGrantName;


        if (this->MyGuild != NULL)
        {
            if (this->MyGuild->IntanceGuild->GuildLevel >= 4)
            {
                this->SetGrantName(&NewGrantName);
            }
        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3501) {

        BYTE TitleID;
        *pMsg >> TitleID;


        if (this->m_pLifeState->m_btBodyState == BODYMODE_HWAN) {
            //CMsg* pMsg = this->AllocMsg(0x5015);
            //byte i = 1;
            //*pMsg << i;
            //this->SendMsg(pMsg);
        }
        else {
            this->UpdateHwan(TitleID);
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3502) {

        BYTE Cape;
        *pMsg >> Cape;

        if (this->CanSendMessage()) {

            this->UpdatePVPCapeType(Cape);
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3503) {
        BYTE WorldID;
        *pMsg >> WorldID;

        unsigned short wRegionID;
        *pMsg >> wRegionID;

        float X;
        float Y;
        float Z;
        *pMsg >> X;
        *pMsg >> Y;
        *pMsg >> Z;


        if (this->CanSendMessage()) {


            if (!this->MoveTo(WorldID + 0x10000, wRegionID, X, Y, Z, 2)) {
                this->MoveTo(WorldID + 0x10000, wRegionID, X, Y, Z, 1);
            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3504) {
        int SlotID;
        *pMsg >> SlotID;

        BYTE WorldID;
        *pMsg >> WorldID;
        unsigned short wRegionID;
        *pMsg >> wRegionID;

        float X;
        float Y;
        float Z;
        *pMsg >> X;
        *pMsg >> Y;
        *pMsg >> Z;

        if (this->CanSendMessage()) {

            if (this->GetItemChar(SlotID)->InstanceItem != NULL)
            {
                printf("%d \n", this->GetItemChar(SlotID)->InstanceItem->pCRefObjItem->TID.m_type_id_value);
                if (this->GetItemChar(SlotID)->InstanceItem->pCRefObjItem->TID.m_type_id_value == 6636 ||this->GetItemChar(SlotID)->InstanceItem->pCRefObjItem->TID.m_type_id_value == 6637) {
                    this->SetLiveDeleteItem(SlotID, 1);
                    CMsg* pMsg32 = this->AllocMsg(0x305C);
                    unsigned int pGameID = this->GetGameID();
                    unsigned int ScID = 3769;
                    *pMsg32 << pGameID; //flag opt lvl
                    *pMsg32 << ScID;
                    this->SendMsg(pMsg32);
                    if (!this->MoveTo(1 + 0x10000, wRegionID, X, Y, Z, 2)) {
                        this->MoveTo(1 + 0x10000, wRegionID, X, Y, Z, 1);
                    }
                }

            }
            return;
        }


    }
    else if (*pMsg->m_wpMsgId == 0x3505) {

        byte PaymentMethod;
        *pMsg >> PaymentMethod;

        byte ItemSlot;
        *pMsg >> ItemSlot;

        std::string ItemCodeName;
        std::string TargetItemCodeName;

        *pMsg >> ItemCodeName;
        *pMsg >> TargetItemCodeName;

        int TargetGold;
        if(PaymentMethod == 1)
        {
            *pMsg >> TargetGold;
        }
        if (this->CanSendMessage()) {

            if(this->GetItemChar(ItemSlot) != NULL)
            {
                if (CSqlCon::LockedItemList.find(this->GetItemChar(ItemSlot)->InstanceItem->ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if(this->GetItemChar(ItemSlot)->InstanceItem->pCRefObjItem->m_strObjectCode == ItemCodeName)
                {
                    if(PaymentMethod == 1)
                    {
                        INT64 availablegold = this->m_pObjDataInstance->Gold;
                        if (availablegold >= TargetGold) {
                            this->RemoveGold(TargetGold);
                            this->SetLiveItem(ItemSlot, TargetItemCodeName.c_str());

                            /*    this->GetItemChar(ItemSlot)->RefreshItemStats();
                                byte AdvOptLevel = CSqlCon::GetItemBindingOpt(this->GetItemChar(ItemSlot)->ID64);

                                CMsg* pMsgrefresh = AllocMsg(0x3040);
                                *pMsgrefresh << BYTE(ItemSlot);
                                *pMsgrefresh << BYTE(2); //flag opt lvl
                                *pMsgrefresh << (BYTE)this->GetItemChar(ItemSlot)->InstanceItem->OptLevel + AdvOptLevel;
                                this->SendMsg(pMsgrefresh);


                                this->m_PCInventory.RefreshEquipParamEntryForPC(false);
        */

                        }
                        else
                        {
                            CMsg* pck = this->AllocMsg(0x5015);
                            *pck << byte(3);
                            this->SendMsg(pck);
                        }
                    }
                    else if(PaymentMethod == 0)
                    {
                        this->SetLiveItem(ItemSlot, TargetItemCodeName.c_str());
                    }

                    /// SEND RESULT ?
                }
            }
        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3506) {

        std::string ItemCodeName;
        *pMsg >> ItemCodeName;
        __int32 Amount;
        *pMsg >> Amount;

        bool RandomizeStats;
        *pMsg >> RandomizeStats;
        __int8 OptLevel;
        *pMsg >> OptLevel;
        char* chr = const_cast<char*>(ItemCodeName.c_str());


        if (this->CanSendMessage()) {

            this->AddItem(chr, Amount, RandomizeStats, OptLevel);
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3507) {

        INT64 Amount;
        *pMsg >> Amount;
        if (this->CanSendMessage()) {

            INT64 availablegold = this->m_pObjDataInstance->Gold;
            if (availablegold >= Amount) {
                this->RemoveGold(Amount);
            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3508) {
        int Slot;

        *pMsg >> Slot;
        std::string ItemCodeName;
        *pMsg >> ItemCodeName;
        int Slotdelete;
        *pMsg >> Slotdelete;
        if (this->GetItemChar(Slot)->InstanceItem != NULL) {
            if (this->CanSendMessage()) {

                this->SetLiveDeleteItem(Slotdelete, 1);
                this->SetLiveItem(Slot, ItemCodeName.c_str());
            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3509) {
        int Slot;
        *pMsg >> Slot;
        int Ammont;
        *pMsg >> Ammont;
        int Slot2;
        *pMsg >> Slot2;
        int Ammont2;
        *pMsg >> Ammont2;
        std::string ItemCodeName;
        *pMsg >> ItemCodeName;
        int Ammont3;
        *pMsg >> Ammont3;
        int puls;
        *pMsg >> puls;
        char* chr = const_cast<char*>(ItemCodeName.c_str());

        if (this->GetItemChar(Slot)->InstanceItem != NULL && this->GetItemChar(Slot2)->InstanceItem != NULL) {
            if (this->CanSendMessage()) {

                this->AddItem(chr, Ammont3, 0, puls);
                this->SetLiveDeleteItem(Slot, Ammont);
                this->SetLiveDeleteItem(Slot2, Ammont2);
            }
        }
        return;

    }
    else if (*pMsg->m_wpMsgId == 0x3511) {

        int SkillID;
        *pMsg >> SkillID;
        unsigned int petuqID;
        *pMsg >> petuqID;
        byte AnimationID;
        *pMsg >> AnimationID;
        CGObj* pc = g_pCGame->GetGameObjById(petuqID);
        if (pc != NULL) {
            if (this->CanSendMessage()) {


                std::list<CSkill*>::iterator it = pData.UsedSkillList.begin();
                for (; it != pData.UsedSkillList.end(); ++it) {
                    SSkillPreEngagementData* preEngagementData = (*it)->m_pSkillPreEngagementData;
                    if(SkillID == preEngagementData->m_nSkillId)
                        return;
                }

                CMsg* newmsg = CNetHelper::AllocMsg(0x182C, true);
                *newmsg << petuqID;
                *newmsg << AnimationID;
                this->BroadcastMsgToNearbyPlayers(newmsg);

                this->LiveSkill(SkillID);
            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3521) {
        unsigned int exp;
        *pMsg >> exp;

        if (this->GetJobState() != 4)
        {
            this->FuncJobExpTest(exp);
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3527) {
        int SilkCount;
        *pMsg >> SilkCount;

        if (this->CanSendMessage()) {
            this->UpdateSilk(SilkCount, 0, 0, true);
        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3528)
    {
        unsigned int PetUQID;
        int PetSkillID;
        *pMsg >> PetUQID >> PetSkillID;
        CGObjCOS_GoldPet* pc = static_cast<CGObjCOS_GoldPet*>(g_pCGame->GetGameObjById(PetUQID));
        if (pc != NULL)
        {
            pc->LiveSkill2(PetSkillID);
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3530)
    {
        int itemId;
        *pMsg >> itemId;

        int itemSlotId;
        *pMsg >> itemSlotId;

        unsigned short itemTypeId;
        *pMsg >> itemTypeId;

        if (this->GetItemChar(itemSlotId)->InstanceItem != NULL)
        {
            if (this->GetItemChar(itemSlotId)->InstanceItem->RefItemID == itemId && this->GetItemChar(itemSlotId)->InstanceItem->pCRefObjItem->TID.m_type_id_value == 0xC6ED)
            {
                this->SetLiveDeleteItem(itemSlotId, 1);

                CMsg* pMsgg = this->AllocMsg(0x210B);
                *pMsgg << itemId;
                this->SendMsg(pMsgg);

            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3531)
    {
        int itemId;
        *pMsg >> itemId;

        int itemSlotId;
        *pMsg >> itemSlotId;

        unsigned short itemTypeId;
        *pMsg >> itemTypeId;

        int LockedItemSlot;
        *pMsg >> LockedItemSlot;


        if (this->GetItemChar(LockedItemSlot) != NULL)
        {
            INT64 ID64 = this->GetItemChar(LockedItemSlot)->ID64;
            if (CSqlCon::LockedItemList.find(ID64) == CSqlCon::LockedItemList.end())
            {
                if (this->GetItemChar(itemSlotId)->InstanceItem != NULL)
                {
                    if (this->GetItemChar(itemSlotId)->InstanceItem->RefItemID == itemId && this->GetItemChar(itemSlotId)->InstanceItem->pCRefObjItem->TID.m_type_id_value == 0xCEED)
                    {
                        this->SetLiveDeleteItem(itemSlotId, 1);

                        CSqlCon::LockedItemList.insert((std::make_pair(ID64, 0)));

                        CMsg* newpMsg = this->AllocMsg(0x5030);
                        *newpMsg << LockedItemSlot << ID64;

                        this->SendMsg(newpMsg);

                        CMsg* pMsg2 = this->AllocMsg(0x305C);
                        *pMsg2 << unsigned int(this->GetGameID()); //flag opt lvl
                        *pMsg2 << unsigned int(3769);

                        this->SendMsg(pMsg2);


                        CMsg* pShardMsgFirst = NEWMSG(SEND_SHARD_TO_LOCK_INFO, false);
                        *pShardMsgFirst << ID64;
                        CNetHelper::SendMsgToSM(pShardMsgFirst);
                    }
                }
            }
            else
            {
                CMsg* pck = this->AllocMsg(0x5015);
                *pck << byte(TYPE_OF_ITEM_LOCKED);
                this->SendMsg(pck);
                return;
            }
        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3532)
    {
        int itemId;
        *pMsg >> itemId;

        int itemSlotId;
        *pMsg >> itemSlotId;

        unsigned short itemTypeId;
        *pMsg >> itemTypeId;

        int LockedItemSlot;
        *pMsg >> LockedItemSlot;


        if (this->GetItemChar(LockedItemSlot) != NULL)
        {
            INT64 ID64 = this->GetItemChar(LockedItemSlot)->ID64;
            if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
            {
                    if (this->GetItemChar(itemSlotId)->InstanceItem != NULL)
                    {
                        if (this->GetItemChar(itemSlotId)->InstanceItem->RefItemID == itemId && this->GetItemChar(itemSlotId)->InstanceItem->pCRefObjItem->TID.m_type_id_value == 0xD6ED)
                        {
                            this->SetLiveDeleteItem(itemSlotId, 1);

                            CMsg* newpMsg = this->AllocMsg(0x5031);
                            *newpMsg << LockedItemSlot;
                            *newpMsg << ID64;
                            this->SendMsg(newpMsg);

                            CSqlCon::LockedItemList.erase(ID64);

                            CMsg* pMsg2 = this->AllocMsg(0x305C);
                            *pMsg2 << unsigned int(this->GetGameID()); //flag opt lvl
                            *pMsg2 << unsigned int(3769);

                            this->SendMsg(pMsg2);


                            CMsg* pShardMsgFirst = NEWMSG(SEND_SHARD_TO_UNLOCK_INFO, false);
                            *pShardMsgFirst << ID64;
                            CNetHelper::SendMsgToSM(pShardMsgFirst);
                        }
                    }
                    return;
            }
            else
            {
                CMsg* pck = this->AllocMsg(0x5015);
                *pck << byte(5);
                this->SendMsg(pck);
                return;
            }
        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3533)
    {

        byte ItemSlot;
        *pMsg >> ItemSlot;
        CGItem* pItem = this->GetItemChar(ItemSlot);

        byte AdvPlus;
        *pMsg >> AdvPlus;
        if (pItem != NULL)
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

            CMsg* pSmMsg = this->AllocMsg(0x5034);

            pSmMsg->Write<int>(nLen);
            pSmMsg->Write(pItemDataBuf, nLen);
            *pSmMsg << pItem->InstanceItem->RefItemID;
            *pSmMsg << pItem->InstanceItem->OptLevel;
            *pSmMsg << AdvPlus;
            this->SendMsg(pSmMsg);
            delete[] pItemDataBuf;
            CNetHelper::FreeMsg(pTmpMsg);

        }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3534) {
        byte FuseType;
        pMsg->Read<BYTE>(FuseType);

        if (FuseType == 0) {
            byte ItemSlot;
            pMsg->Read<BYTE>(ItemSlot);

            byte EnhancerSlot;
            pMsg->Read<BYTE>(EnhancerSlot);

            byte ProofSlot;
            pMsg->Read<BYTE>(ProofSlot);

            if (this->GetItemChar(ItemSlot) != NULL)
            {
                CGItem *pTargetItem = this->GetItemChar(ItemSlot);
                if (CSqlCon::LockedItemList.find(pTargetItem->InstanceItem->ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 && pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 1 && pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID3() != 13)
                {
                    if (this->GetItemChar(EnhancerSlot) != NULL)
                    {
                        if (this->ItemIsWeapon(pTargetItem->InstanceItem->pCRefObjItem->TID))
                        {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 100663296 && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                if (this->GetItemChar(ProofSlot) != NULL)
                                {
                                    CGItem *pRoofItem = this->GetItemChar(ProofSlot);
                                    if (this->ItemIsProofStone(pRoofItem->InstanceItem->pCRefObjItem->TID))
                                    {
                                        if (pRoofItem->InstanceItem->pCRefObjItem->Param1 == pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass)
                                        {
                                            SetLiveDeleteItem(EnhancerSlot, 1);
                                            /// FUSE VIA PROOF
                                            if (pTargetItem->InstanceItem->OptLevel == 0) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_1) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);
                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);

                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64);
                                                    NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) 0);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                               pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_2) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_3) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_4) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_5) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_6) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_7) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_8) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_9) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_10) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_11) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_12) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_13) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_14) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_15) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                                CMsg *NewMsg = this->AllocMsg(0x5017);
                                                NewMsg->Write<BYTE>(0); /// W PROOF
                                                NewMsg->Write<BYTE>(2); // FAIL RESULT
                                                this->SendMsg(NewMsg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (this->ItemIsArmor(pTargetItem->InstanceItem->pCRefObjItem->TID)) {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree) {
                                if (this->GetItemChar(ProofSlot) != NULL) {
                                    CGItem *pRoofItem = this->GetItemChar(ProofSlot);
                                    if (this->ItemIsProofStone(pRoofItem->InstanceItem->pCRefObjItem->TID)) {
                                        if (pRoofItem->InstanceItem->pCRefObjItem->Param1 ==
                                            pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass) {

                                            SetLiveDeleteItem(EnhancerSlot, 1);
                                            /// FUSE VIA PROOF
                                            if (pTargetItem->InstanceItem->OptLevel == 0) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_1) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);
                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) 0);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                               pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_2) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_3) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_4) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_5) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_6) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_7) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_8) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_9) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_10) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_11) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_12) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_13) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_14) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_15) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                                CMsg *NewMsg = this->AllocMsg(0x5017);
                                                NewMsg->Write<BYTE>(0); /// W PROOF
                                                NewMsg->Write<BYTE>(2); // FAIL RESULT
                                                this->SendMsg(NewMsg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (this->ItemIsAccessory(pTargetItem->InstanceItem->pCRefObjItem->TID)){
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 83886080 &&
                                this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                if (this->GetItemChar(ProofSlot) != NULL) {
                                    CGItem *pRoofItem = this->GetItemChar(ProofSlot);
                                    if (this->ItemIsProofStone(pRoofItem->InstanceItem->pCRefObjItem->TID))
                                    {
                                        if (pRoofItem->InstanceItem->pCRefObjItem->Param1 ==
                                            pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass) {

                                            SetLiveDeleteItem(EnhancerSlot, 1);
                                            /// FUSE VIA PROOF
                                            if (pTargetItem->InstanceItem->OptLevel == 0) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_1) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);
                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) 0);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                               pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_2) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_3) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_4) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_5) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_6) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_7) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_8) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_9) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_10) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_11) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_12) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_13) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_14) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_15) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                                CMsg *NewMsg = this->AllocMsg(0x5017);
                                                NewMsg->Write<BYTE>(0); /// W PROOF
                                                NewMsg->Write<BYTE>(2); // FAIL RESULT
                                                this->SendMsg(NewMsg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else if (this->ItemIsShield(pTargetItem->InstanceItem->pCRefObjItem->TID)) {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 67108864 && this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree) {
                                if (this->GetItemChar(ProofSlot) != NULL) {
                                    CGItem *pRoofItem = this->GetItemChar(ProofSlot);
                                    if (this->ItemIsProofStone(pRoofItem->InstanceItem->pCRefObjItem->TID)) {
                                        if (pRoofItem->InstanceItem->pCRefObjItem->Param1 ==
                                            pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass) {
                                            SetLiveDeleteItem(EnhancerSlot, 1);
                                            /// FUSE VIA PROOF
                                            if (pTargetItem->InstanceItem->OptLevel == 0) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_1) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);
                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);
                                                    this->SendMsg(NewMsg);
                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();
                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) 0);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                               pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_2) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::SUCCESS); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(eAlchemyType::WITH_PROOF); /// W PROOF
                                                    NewMsg->Write<BYTE>(eAlchemyResultType::FAILED); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_3) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_4) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_5) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_6) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_7) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_8) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_9) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_10) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_11) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_12) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_13) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_14) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                                int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                                if (chance < SuccesRateOptLevel_15) {
                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                } else {

                                                    SetLiveDeleteItem(ProofSlot, 1);

                                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                                    NewMsg->Write<BYTE>(0); /// W PROOF
                                                    NewMsg->Write<BYTE>(1); // FAIL RESULT
                                                    NewMsg->Write<BYTE>(ItemSlot);
                                                    NewMsg->Write<BYTE>(EnhancerSlot);
                                                    NewMsg->Write<BYTE>(ProofSlot);

                                                    NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel - 1);
                                                    NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                                    byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                                    this->SendMsg(NewMsg);

                                                    pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel-1);
                                                    pTargetItem->RefreshItemStats();

                                                    byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                                }

                                                this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                            }
                                            else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                                CMsg *NewMsg = this->AllocMsg(0x5017);
                                                NewMsg->Write<BYTE>(0); /// W PROOF
                                                NewMsg->Write<BYTE>(2); // FAIL RESULT
                                                this->SendMsg(NewMsg);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (FuseType == 1) {
            byte ItemSlot;
            pMsg->Read<BYTE>(ItemSlot);

            byte EnhancerSlot;
            pMsg->Read<BYTE>(EnhancerSlot);

            if (this->GetItemChar(ItemSlot) != NULL) {
                CGItem *pTargetItem = this->GetItemChar(ItemSlot);
                if (CSqlCon::LockedItemList.find(pTargetItem->InstanceItem->ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID1() == 3 && pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID2() == 1 && pTargetItem->InstanceItem->pCRefObjItem->TID.getTypeID3() != 13)
                {
                    if (this->GetItemChar(EnhancerSlot) != NULL)
                    {
                        if (this->ItemIsWeapon(pTargetItem->InstanceItem->pCRefObjItem->TID)) {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 100663296 && this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                SetLiveDeleteItem(EnhancerSlot, 1);
                                if (pTargetItem->InstanceItem->OptLevel == 0) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_1) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_2) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    }
                                    else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_3) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_4) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_5) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus(
                                                (BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                       pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_6) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);
                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_7) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_8) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_9) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_10) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_11) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_12) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_13) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_14) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_15) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                    NewMsg->Write<BYTE>(1); /// W PROOF
                                    NewMsg->Write<BYTE>(2); // FAIL RESULT
                                    this->SendMsg(NewMsg);
                                }
                            }
                        }
                        else if (this->ItemIsArmor(pTargetItem->InstanceItem->pCRefObjItem->TID))
                        {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 16909056 && this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                SetLiveDeleteItem(EnhancerSlot, 1);
                                if (pTargetItem->InstanceItem->OptLevel == 0) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_1) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_2) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    }
                                    else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_3) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_4) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_5) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus(
                                                (BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                       pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_6) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);
                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_7) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_8) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_9) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_10) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_11) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_12) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_13) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_14) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_15) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                    NewMsg->Write<BYTE>(1); /// W PROOF
                                    NewMsg->Write<BYTE>(2); // FAIL RESULT
                                    this->SendMsg(NewMsg);
                                }
                            }
                        }
                        else if (this->ItemIsAccessory(pTargetItem->InstanceItem->pCRefObjItem->TID)) {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 83886080 && this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                SetLiveDeleteItem(EnhancerSlot, 1);
                                if (pTargetItem->InstanceItem->OptLevel == 0) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_1) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_2) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    }
                                    else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_3) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_4) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_5) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus(
                                                (BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                       pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_6) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);
                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_7) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_8) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_9) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_10) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_11) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_12) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_13) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_14) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_15) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                    NewMsg->Write<BYTE>(1); /// W PROOF
                                    NewMsg->Write<BYTE>(2); // FAIL RESULT
                                    this->SendMsg(NewMsg);
                                }
                            }
                        }
                        else if (this->ItemIsShield(pTargetItem->InstanceItem->pCRefObjItem->TID)) {
                            CGItem *pEnhancerItem = this->GetItemChar(EnhancerSlot);
                            int Degree = GetDegreeLevel(pTargetItem->InstanceItem->pCRefObjItem->m_btItemClass);
                            if (pEnhancerItem->InstanceItem->pCRefObjItem->Param3 == 67108864 && this->ItemIsEnhancer(pEnhancerItem->InstanceItem->pCRefObjItem->TID) && pEnhancerItem->InstanceItem->pCRefObjItem->Param1 == Degree)
                            {
                                SetLiveDeleteItem(EnhancerSlot, 1);
                                if (pTargetItem->InstanceItem->OptLevel == 0) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_1) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 1) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_2) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    }
                                    else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 2) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_3) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 3) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_4) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 4) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_5) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus(
                                                (BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,
                                                                       pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 5) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_6) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);
                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 6) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_7) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 7) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_8) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 8) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_9) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 9) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_10) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 10) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_11) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 11) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_12) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 12) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_13) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);

                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 13) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_14) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 14) {
                                    int chance = rand() % 100; // 0 - 99 arasında rastgele bir sayı
                                    if (chance < SuccesRateOptLevel_15) {
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W PROOF
                                        NewMsg->Write<BYTE>(0); // SUCCESS RESULT
                                        NewMsg->Write<BYTE>(ItemSlot);
                                        NewMsg->Write<BYTE>(EnhancerSlot);


                                        NewMsg->Write<BYTE>(pTargetItem->InstanceItem->OptLevel + 1);
                                        NewMsg->Write<int>(pTargetItem->InstanceItem->RefItemID);
                                        byte AdvOptLevel = CSqlCon::GetItemBindingOpt(pTargetItem->ID64); NewMsg->Write<BYTE>(AdvOptLevel);

                                        this->SendMsg(NewMsg);

                                        pTargetItem->InstanceItem->SetPlus((BYTE) pTargetItem->InstanceItem->OptLevel + 1);
                                        pTargetItem->RefreshItemStats();

                                        byte NewPlus = pTargetItem->InstanceItem->OptLevel+AdvOptLevel; this->Send3040(ItemSlot, NewPlus);
                                        this->UpdateItemPlusInDatabase(pTargetItem->ID64,pTargetItem->InstanceItem->OptLevel);
                                    } else {
                                        SetLiveDeleteItem(ItemSlot, 1);
                                        CMsg *NewMsg = this->AllocMsg(0x5017);
                                        NewMsg->Write<BYTE>(1); /// W/O PROOF
                                        NewMsg->Write<BYTE>(1); // FAIL RESULT
                                        this->SendMsg(NewMsg);
                                    }
                                }
                                else if (pTargetItem->InstanceItem->OptLevel == 15) {
                                    CMsg *NewMsg = this->AllocMsg(0x5017);
                                    NewMsg->Write<BYTE>(1); /// W PROOF
                                    NewMsg->Write<BYTE>(2); // FAIL RESULT
                                    this->SendMsg(NewMsg);
                                }
                            }
                        }
                    }
                }
            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3535)
    {
        this->TeleportToTown();

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3536) /// resurrect and teleport
    {
        BYTE WorldID;
        *pMsg >> WorldID;

        unsigned short wRegionID;
        *pMsg >> wRegionID;

        float X;
        float Y;
        float Z;
        *pMsg >> X;
        *pMsg >> Y;
        *pMsg >> Z;

            this->Ressurect(300, 300);
            if (this->CanSendMessage()) {


                if (!this->MoveTo(WorldID + 0x10000, wRegionID, X, Y, Z, 2)) {
                    this->MoveTo(WorldID + 0x10000, wRegionID, X, Y, Z, 1);
                }
            }

        return;
    }
    else if (*pMsg->m_wpMsgId == 0x3538)
    {
        byte uniquetype;
        *pMsg >> uniquetype;
        SPosInfo minePos;
        this->GetPosInfo(minePos);
        SWorldID mineWorldID;
        this->GetWorldID(mineWorldID);

        if(uniquetype == 0)
        {
            CGObjMob::CreateMob(46406, mineWorldID.dwWorldID, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
        }
        else if(uniquetype == 1)
        {
            CGObjMob::CreateMob(46407, mineWorldID.dwWorldID, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
        }
        else if(uniquetype == 2)
        {
            CGObjMob::CreateMob(46408, mineWorldID.dwWorldID, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
        }
        else if(uniquetype == 3)
        {
            CGObjMob::CreateMob(46409, mineWorldID.dwWorldID, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
        }
        else if(uniquetype == 4)
        {
            CGObjMob::CreateMob(46410, mineWorldID.dwWorldID, (uint16_t)minePos.wRegionID, (float)minePos.fltX, (float)minePos.fltY, (float)minePos.fltZ, (float)5);
        }
       return;
    }
    else if (*pMsg->m_wpMsgId == 0x3537)
    {
        std::string SenderName;
        pMsg->ReadString(SenderName);

        int is = m_PCInventory.m_nSlotsCount;
        for (int i = 0; i < 13; i++)
        {
            if(i != 8)
            {
                if (this->GetItemChar(i) != NULL)
                {
                    CGItem* pItem = this->GetItemChar(i);
                    if (pItem != NULL)
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

                        CMsg* pSmMsg = this->AllocMsg(0x5038);
                        pSmMsg->Write<byte>(i);
                        pSmMsg->Write<int>(nLen);
                        pSmMsg->Write(pItemDataBuf, nLen);
                        *pSmMsg << pItem->InstanceItem->RefItemID;
                        pSmMsg->WriteString(SenderName);
                        this->SendMsg(pSmMsg);
                        delete[] pItemDataBuf;
                        CNetHelper::FreeMsg(pTmpMsg);

                    }
                }
            }

        }
        /*
        m_PCInventory.GetItem()
        if (pItem != NULL)
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

            CMsg* pSmMsg = this->AllocMsg(0x5033);
            pSmMsg->Write<BYTE>(GlobalType);
            pSmMsg->Write<BYTE>(GlobalSlot);

            pSmMsg->Write<USHORT>(GlobalItemType);
            pSmMsg->Write<int>(GlobalItemID);
            pSmMsg->WriteStringW(Message);

            pSmMsg->Write<int>(nLen);
            pSmMsg->Write(pItemDataBuf, nLen);
            *pSmMsg << pItem->InstanceItem->RefItemID;
            this->SendMsg(pSmMsg);
            delete[] pItemDataBuf;

            CNetHelper::FreeMsg(pTmpMsg);

        }*/
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x705D)
    {
        byte eChatType;
        pMsg->Read<BYTE>(eChatType);

        byte btChatIndex;
        pMsg->Read<BYTE>(btChatIndex);

        std::string strReceiverName;
        if(eChatType == PRIVATE)
        {
            pMsg->ReadString(strReceiverName);
        }

        std::string strAsciiText;
        pMsg->ReadString(strAsciiText);

        byte LinkedItemSlot;
        pMsg->Read<BYTE>(LinkedItemSlot);


        int dbid = this->GetDBID();
        CMsg* pShardMsgFirst = NEWMSG(0x5068, false);
        pShardMsgFirst->Write<int>(dbid);
        pShardMsgFirst->Write<byte>(eChatType);
        pShardMsgFirst->Write<byte>(btChatIndex);
        if(eChatType == PRIVATE) {
            pShardMsgFirst->WriteString(strReceiverName);
        }
        pShardMsgFirst->WriteString(strAsciiText);
        pShardMsgFirst->Write<byte>(LinkedItemSlot);
        CNetHelper::SendMsgToSM(pShardMsgFirst);


        /*      CGItem* pItem = this->GetItemChar(ItemSlot);

              if (pItem != NULL)
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

                      CMsg* pSmMsg = this->AllocMsg(0x5033);
                      pSmMsg->Write<BYTE>(GlobalType);
                      pSmMsg->Write<BYTE>(GlobalSlot);

                      pSmMsg->Write<USHORT>(GlobalItemType);
                      pSmMsg->Write<int>(GlobalItemID);
                      pSmMsg->WriteStringW(Message);

                      pSmMsg->Write<int>(nLen);
                      pSmMsg->Write(pItemDataBuf, nLen);
                      *pSmMsg << pItem->InstanceItem->RefItemID;
                      this->SendMsg(pSmMsg);
                      delete[] pItemDataBuf;

                      CNetHelper::FreeMsg(pTmpMsg);

              }
      */
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x705C)
    {
        byte GlobalType;
        pMsg->Read<BYTE>(GlobalType);

        byte GlobalSlot;
        pMsg->Read<BYTE>(GlobalSlot);

        USHORT GlobalItemType;
        pMsg->Read<USHORT>(GlobalItemType);

        int GlobalItemID;
        pMsg->Read<int>(GlobalItemID);

        std::wstring Message;
        pMsg->ReadStringW(Message);

        if (GlobalType == 0)
        {
            CMsg* NewMsg = this->AllocMsg(0x5033);
            NewMsg->Write<BYTE>(GlobalType);
            NewMsg->Write<BYTE>(GlobalSlot);

            NewMsg->Write<USHORT>(GlobalItemType);
            NewMsg->Write<int>(GlobalItemID);
            NewMsg->WriteStringW(Message);
            this->SendMsg(NewMsg);
        }
        else if (GlobalType == 1)
        {
            byte ItemSlot;
            *pMsg >> ItemSlot;
            CGItem* pItem = this->GetItemChar(ItemSlot);

            if (pItem != NULL)
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

                CMsg* pSmMsg = this->AllocMsg(0x5033);
                pSmMsg->Write<BYTE>(GlobalType);
                pSmMsg->Write<BYTE>(GlobalSlot);

                pSmMsg->Write<USHORT>(GlobalItemType);
                pSmMsg->Write<int>(GlobalItemID);
                pSmMsg->WriteStringW(Message);

                pSmMsg->Write<int>(nLen);
                pSmMsg->Write(pItemDataBuf, nLen);
                *pSmMsg << pItem->InstanceItem->RefItemID;
                this->SendMsg(pSmMsg);
                delete[] pItemDataBuf;

                CNetHelper::FreeMsg(pTmpMsg);

            }
        }
        return;
    }
    else if (*pMsg->m_wpMsgId == 0x7074) {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;

        byte action;
        *pMsg >> action;
        if (action == 1) {
            byte num;
            *pMsg >> num;

            if (num == 1) {
                byte action2;
                *pMsg >> action2;

                int test;
                *pMsg >> test;

                CGObjMob* pPC = reinterpret_cast<CGObjMob*>(
                        g_pCGame->GetGameObjById(test));
                if (pPC != NULL) {
                    std::string charname = pPC->GetCodeName();
                    if (pPC->Monsterclass == 3) {
                        size_t pos_f = charname.find("_STR");
                        size_t pos_f2 = charname.find("_INT");
                        int Str = this->m_pObjDataInstance->Strength;
                        int Int = this->m_pObjDataInstance->Intellect;
                        if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
                        {
                            CMsg* newpMsg = this->AllocMsg(0x3563);
                            byte type = 0;
                            *newpMsg << type;
                            this->SendMsg(newpMsg);
                            newpMsg->m_wWriteDataArrayPos = 0;

                            return;
                        }
                        else if (pos_f2 != -1 && Str > Int) {

                            CMsg* newpMsg = this->AllocMsg(0x3563);
                            byte type = 1;
                            *newpMsg << type;
                            this->SendMsg(newpMsg);
                            newpMsg->m_wWriteDataArrayPos = 0;
                            return;
                        }
                    }
                }
            }
            else if (num == 4) {


                int test;
                *pMsg >> test;
                byte action2;
                *pMsg >> action2;
                if (action2 == 0) {
                    if (pData.active.find(test) == pData.active.end())
                    {
                        return;
                    }
                    //if (this->Motion == 2)

                    //{
                    //    CMsg* newpMsg = this->AllocMsg(0x3563);
                    //    byte type = 1;
                    //    *newpMsg << type;
                    //    this->SendMsg(newpMsg);
                    //    return;
                    //}
                }

                else if (action2 == 1) {
                    int test2;
                    *pMsg >> test2;
                    if (pData.active.find(test) == pData.active.end())
                    {
                        return;
                    }
                    /*     if (this->Motion == 2)
                         {
                             CMsg* newpMsg = this->AllocMsg(0x3563);
                             byte type = 1;
                             *newpMsg << type;
                             this->SendMsg(newpMsg);
                             return;
                         }*/

                    CGObjMob* pPC = reinterpret_cast<CGObjMob*>(
                            g_pCGame->GetGameObjById(test2));
                    if (pPC != NULL) {
                        std::string charname = pPC->GetCodeName();


                        if (pPC->Monsterclass == 3) {
                            size_t pos_f = charname.find("_STR");
                            size_t pos_f2 = charname.find("_INT");
                            int Str = this->m_pObjDataInstance->Strength;
                            int Int = this->m_pObjDataInstance->Intellect;
                            if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
                            {
                                CMsg* newpMsg = this->AllocMsg(0x3563);
                                byte type = 0;
                                *newpMsg << type;
                                this->SendMsg(newpMsg);

                                return;
                            }
                            else if (pos_f2 != -1 && Str > Int) {

                                CMsg* newpMsg = this->AllocMsg(0x3563);
                                byte type = 1;
                                *newpMsg << type;
                                this->SendMsg(newpMsg);

                                return;
                            }
                        }
                    }
                }
            }
        }
        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if (*pMsg->m_wpMsgId == 0x7150)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte type;

        *pMsg >> type;
        if (type == 2)
        {
            byte unk1;
            byte unk2;
            byte ItemSlot;
            byte StoneSlot;
            *pMsg >> unk1 >> unk2 >> ItemSlot >> StoneSlot;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }
        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if (*pMsg->m_wpMsgId == 0x716A)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte type;
        *pMsg >> type;
        if (type == 1)
        {
            byte unk2;
            byte ItemSlot;
            byte StoneSlot;
            *pMsg >> unk2 >> ItemSlot >> StoneSlot;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }
        else if (type == 2)
        {
            byte unk2;
            byte ItemSlot;
            byte StoneSlot;
            byte unks;
            *pMsg >> unk2 >> ItemSlot >> StoneSlot >> unks;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }
        else if (type == 3)
        {
            byte unk2;
            byte ItemSlot;
            byte StoneSlot;
            *pMsg >> unk2 >> ItemSlot >> StoneSlot;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }
        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;

    }
    else if (*pMsg->m_wpMsgId == 0x7151)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte type;

        *pMsg >> type;
        if (type == 2)
        {
            byte unk1;
            byte unk2;
            byte ItemSlot;
            byte StoneSlot;
            *pMsg >> unk1 >> unk2 >> ItemSlot >> StoneSlot;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }

        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if (*pMsg->m_wpMsgId == 0x7157)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte type;

        *pMsg >> type;
        if (type == 1)
        {

            byte ItemSlot;

            *pMsg >> ItemSlot;
            if (this->GetItemChar(ItemSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(ItemSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }

        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if (*pMsg->m_wpMsgId == 0x7034)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;

        E_INVENTORY_OP_TYPE type;
        *pMsg >> type;
        switch (type) {
            case E_INVENTORY_OP_TYPE::DEPOSIT_ITEM:
            {
                byte slot;
                byte slot_to;
                int movingcount;
                *pMsg >> slot >> slot_to >> movingcount;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;
            case E_INVENTORY_OP_TYPE::ADD_EXCHANGE:
            {
                byte slot;
                *pMsg >> slot;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;
            case E_INVENTORY_OP_TYPE::DROP_ITEM:
            {
                byte slot;
                *pMsg >> slot;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;
            case E_INVENTORY_OP_TYPE::SELL_ITEM:
            {
                byte slot;
                byte slot_to;
                int movingcount;
                *pMsg >> slot >> slot_to >> movingcount;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;

            case E_INVENTORY_OP_TYPE::MOVE_ITEM_PC_PET:
            {
                int petuqID;
                byte slot;
                byte petslot;
                *pMsg >> petuqID >> slot >> petslot;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;

            case E_INVENTORY_OP_TYPE::PICK_ITEM_BY_OTHER:
            {
                byte slot;
                byte slot_to;
                int pp;
                *pMsg >> slot >> slot_to >> pp;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;

            case E_INVENTORY_OP_TYPE::GUILD_CHEST_DEPOSIT_ITEM:
            {
                byte slot;
                byte slot_to;
                int pp;
                *pMsg >> slot >> slot_to >> pp;
                if (this->GetItemChar(slot) != NULL)
                {
                    INT64 ID64 = this->GetItemChar(slot)->ID64;
                    if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        *pck << byte(TYPE_OF_ITEM_LOCKED);
                        this->SendMsg(pck);
                        return;
                    }
                    if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                    {
                        CMsg* pck = this->AllocMsg(0x5015);
                        std::string Notice = "VFILTER_ITEM_POWERUP";
                        *pck << byte(0) << Notice;
                        this->SendMsg(pck);
                        return;
                    }
                }
            }
                break;
        }

        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if(*pMsg->m_wpMsgId == 0x70BA)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte updateType;
        *pMsg >> updateType;
        if(updateType == 2)
        {
            byte Slot;    //within Stall (0-9)
            byte SourceSlot; //from ownerInventory
            unsigned short StackCount;
            INT64 Price;
            unsigned int FleaMarketNetworkTidGroup;
            unsigned short unkUShort0;
            *pMsg >> Slot;
            *pMsg >> SourceSlot;
            *pMsg >> StackCount;
            *pMsg >> Price;
            *pMsg >> FleaMarketNetworkTidGroup;
            *pMsg >> unkUShort0;
            if (this->GetItemChar(SourceSlot) != NULL)
            {
                INT64 ID64 = this->GetItemChar(SourceSlot)->ID64;
                if (CSqlCon::LockedItemList.find(ID64) != CSqlCon::LockedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    *pck << byte(TYPE_OF_ITEM_LOCKED);
                    this->SendMsg(pck);
                    return;
                }
                if (CSqlCon::TimedItemList.find(ID64) != CSqlCon::TimedItemList.end())
                {
                    CMsg* pck = this->AllocMsg(0x5015);
                    std::string Notice = "VFILTER_ITEM_POWERUP";
                    *pck << byte(0) << Notice;
                    this->SendMsg(pck);
                    return;
                }
            }
        }


        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }
    else if (*pMsg->m_wpMsgId == 0x704F)
    {
        int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
        byte state;
        *pMsg >> state;
        if (state == 2)
        {
            if (GoldPetPtr != NULL)
            {

                return;

            }
        }
        pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    }

    reinterpret_cast<void(__thiscall*)(CGObjPC*, CMsg*)>(0x0050eee0)(this, pMsg);
}
void CGObjPC::UpdateItemPlusInDatabase(UINT64 ID64, byte Plus)
{
    char szQuery[256] = { 0 };
    sprintf(szQuery, "UPDATE SRO_VT_SHARD.._Items SET OptLevel = %d WHERE ID64 = %lld",
            Plus, ID64);

    if (!CSqlCon::TryExecNonQuery(szQuery))
    {
        printf("%s - What the fuck ?! Query failed, query = %s\n", __FUNCTION__, szQuery);
    }
    else
    {
        ZeroMemory(szQuery, sizeof(szQuery));
    }
}
void CGObjPC::SetCharState(char a2) {
    reinterpret_cast<void(__thiscall *)(CGObjPC *, char, int, float)>(0x004A9C80)(this, a2, 255, 0.f);
}
void CGObjPC::FuncJobExp(unsigned int p1, unsigned int p2, unsigned int p3)
{
    //printf("%p ! %p ! %p\n", p1, p2, p3);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int, unsigned int)>(0x004e27c0)(this, p1, p2, p3);

}

#define OFFSET			0x004e2830
void CGObjPC::FuncJobExpTest(unsigned int p1)
{
    DWORD dwCallAddr = OFFSET;

    __asm pushad;
    __asm pushfd;

    __asm push p1;
    __asm mov edi, dword ptr[this];
    __asm call dwCallAddr;

    __asm popfd;
    __asm popad;

}

void CGObjPC::FuncJobExpTest2(unsigned int p1)
{
    //printf("%p ! %d\n", p1,  p1);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int)>(0x004e6820)(this, p1);

}

void CGObjPC::Fun_385(CMsg *pMsg) {
    int m_wReadDataArrayPos1 = pMsg->m_wReadDataArrayPos;
    if (*pMsg->m_wpMsgId == 0x7074) {



        byte action;
        *pMsg >> action;
        if (action == 1) {
            byte num;
            *pMsg >> num;

            if (num == 1) {
                byte action2;
                *pMsg >> action2;

                int test;
                *pMsg >> test;

                CGObjMob *pPC = reinterpret_cast<CGObjMob *>(
                        g_pCGame->GetGameObjById(test));
                if (pPC != NULL) {
                    std::string charname = pPC->GetCodeName();
                    if (pPC->Monsterclass == 3) {
                        size_t pos_f = charname.find("_STR");
                        size_t pos_f2 = charname.find("_INT");
                        int Str = this->m_pObjDataInstance->Strength;
                        int Int = this->m_pObjDataInstance->Intellect;
                        if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
                        {
                            CMsg *newpMsg = this->AllocMsg(0x3563);
                            byte type = 0;
                            *newpMsg << type;
                            this->SendMsg(newpMsg);
                            newpMsg->m_wWriteDataArrayPos = 0;

                            return;
                        } else if (pos_f2 != -1 && Str > Int) {

                            CMsg *newpMsg = this->AllocMsg(0x3563);
                            byte type = 1;
                            *newpMsg << type;
                            this->SendMsg(newpMsg);
                            newpMsg->m_wWriteDataArrayPos = 0;
                            return;
                        }
                    }
                }
            } else if (num == 4) {


                int tests;
                *pMsg >> tests;
                byte action2;
                *pMsg >> action2;
                if (action2 == 0) {
                    if (this->GetMotionState() == MOTIONSTATE_SKILL)
                        return;
                    //CGObjMob *pPC = reinterpret_cast<CGObjMob *>(
                    //    CGame::GetObjByGameID(test));
                    //if (pPC != NULL) {
                    //    std::string charname = pPC->GetCodeName();
                    //    printf("%s \n", charname.c_str());
                    //    int test = pPC->GetRefObjID();
                    //}
                }

                else if (action2 == 1) {
                    int test2;
                    *pMsg >> test2;
                    CGObjMob *pPC = reinterpret_cast<CGObjMob *>(
                            g_pCGame->GetGameObjById(test2));
                    if (pPC != NULL) {
                        std::string charname = pPC->GetCodeName();


                        if (pPC->Monsterclass == 3) {
                            size_t pos_f = charname.find("_STR");
                            size_t pos_f2 = charname.find("_INT");
                            int Str = this->m_pObjDataInstance->Strength;
                            int Int = this->m_pObjDataInstance->Intellect;
                            if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
                            {
                                CMsg *newpMsg = this->AllocMsg(0x3563);
                                byte type = 0;
                                *newpMsg << type;
                                this->SendMsg(newpMsg);

                                return;
                            } else if (pos_f2 != -1 && Str > Int) {

                                CMsg *newpMsg = this->AllocMsg(0x3563);
                                byte type = 1;
                                *newpMsg << type;
                                this->SendMsg(newpMsg);

                                return;
                            }
                        }



                        /*          if (test == 44531)
                                      return;*/
                    }
                }

            }
        }

    }
    pMsg->m_wReadDataArrayPos = m_wReadDataArrayPos1;
    reinterpret_cast<void(__thiscall *)(CGObjPC *, CMsg *)>(0x004b21e0)(this, pMsg);
}
void CGObjPC::Fun_383(CMsg *pMsg) {

    reinterpret_cast<void(__thiscall *)(CGObjPC *, CMsg *)>(0x004b21b0)(this, pMsg);

    if (*pMsg->m_wpMsgId == 0x7070) {

        /*   unsigned int test;
           *pMsg >> test;

           CGObjMob *pPC = reinterpret_cast<CGObjMob *>(
               CGame::GetObjByGameID(test));
           if (pPC != NULL) {
               std::string charname = pPC->GetCodeName();
               int test = pPC->GetRefObjID();
               if (test == 44531) {
                   pMsg->m_wReadDataArrayPos = pMsg->m_wWriteDataArrayPos;
               }
           }*/
    }
}

void CGObjPC::Func_93(int p1, int p2) {
    reinterpret_cast<void(__thiscall*)(CGObjPC*, int, int)>(0x004e4c60)(this, p1, p2);
}
void CGObjPC::FuncWhat() {
    //printf("%p %p \n", p1, p2);
    reinterpret_cast<void(__thiscall*)(CGObjPC*)>(0x0050f1f0)(this);
}
void CGObjPC::Func_201() {
    reinterpret_cast<void(__thiscall*)(CGObjPC*)>(0x004f1640)(this);
}
void CGObjPC::Func_91(int param_1, int param_2, int param_3, int param_4)
{
    printf("%d %d %d %d \n", param_1, param_2, param_3, param_4);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, int, int, int, int)>(0x004e4b60)(this, param_1, param_2, param_3, param_4);
}
void CGObjPC::GroupSpawn(unsigned int p1, int p2)
{
    //printf("%d %p \n", p1, p2);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, int)>(0x004e5b00)(this, p1, p2);

}

void CGObjPC::DealDamage(int nReasonMask, int nAmount)
{
    reinterpret_cast<void(__thiscall*)(CGObjPC*,LPVOID , int , int unkZero_1, DWORD , int unkZero_2)>(0x0052AA30)(this, NULL, nAmount, 0, nReasonMask, 0);
}
void CGObjPC::KillLoggerFunction(CGObjPC* param_1, int param_2)
{
    //printf("%d \n", param_2);
    if (param_2 == 53)
    {
        SWorldID mineWorldIDtest;
        param_1->GetWorldID(mineWorldIDtest);
        if (mineWorldIDtest.wLayerID != 29)
        {
            CMsg* pMsg = this->AllocMsg(0x5013);// FLETER NEREDE
            std::string KillerCharName = param_1->GetCharName();

            int KillerCharID = param_1->GetDBID();
            SWorldID mineWorldID;
            param_1->GetWorldID(mineWorldID);

            std::string GuildName;
            if (param_1->MyGuild != NULL)
            {
                GuildName = param_1->MyGuild->IntanceGuild->MyGuildName;
            }
            else
            {
                GuildName = "dummy";
            }

            *pMsg << KillerCharName << KillerCharID << param_1->RegionID << (int)mineWorldID.wLayerID << param_1->GetJobState() << GuildName;


            this->SendMsg(pMsg); /// kesene paket gönder
        }
    }
    return reinterpret_cast<void(__thiscall*)(CGObjPC*, CGObjPC*, int)>(0x004a7540)(this, param_1, param_2);
}

bool CGObjPC::ItemIsWeapon(TypeId TID)
{
    if(TID.getTypeID1() == 3 && TID.getTypeID2() == 1 && TID.getTypeID3() == 6)
    {
        return true;
    }
    return false;
}
bool CGObjPC::ItemIsArmor(TypeId TID)
{
    if (TID.getTypeID1() == 3 && TID.getTypeID2() == 1
        && (TID.getTypeID3() == 1 || TID.getTypeID3() == 2 || TID.getTypeID3() == 3 || TID.getTypeID3() == 9 || TID.getTypeID3() == 10  || TID.getTypeID3() == 11)) /// ARMOR
    {
        return true;
    }
    return false;
}
bool CGObjPC::ItemIsAccessory(TypeId TID)
{
    if(TID.getTypeID1() == 3 && TID.getTypeID2() == 1 && (TID.getTypeID3() == 5 || TID.getTypeID3() == 12))
    {
        return true;
    }
    return false;
}
bool CGObjPC::ItemIsShield(TypeId TID)
{
    if(TID.getTypeID1() == 3 && TID.getTypeID2() == 1 && TID.getTypeID3() == 4)
    {
        return true;
    }
    return false;
}
bool CGObjPC::ItemIsEnhancer(TypeId TID)
{
    if(TID.getTypeID1() == 3 && TID.getTypeID2() == 3 && TID.getTypeID3() == 10 && TID.getTypeID4() == 6)
    {
        return true;
    }
    return false;
}
void CGObjPC::Send3040(byte ItemSlot, byte NewOptLevel)
{
    CMsg *pMsg = this->AllocMsgForPeer(0x3040);
    *pMsg << ItemSlot;
    *pMsg << BYTE(2); //flag opt lvl
    *pMsg << NewOptLevel;
    this->SendMsg(pMsg);
}
bool CGObjPC::ItemIsProofStone(TypeId TID)
{
    if(TID.getTypeID1() == 3 && TID.getTypeID2() == 3 && TID.getTypeID3() == 10 && TID.getTypeID4() == 8)
    {
        return true;
    }
    return false;
}
BOOL CGObjPC::CanSendMessage()
{
    if (this->m_pObjDataInstance == NULL)
        return FALSE;

    //Get msg block state.

    SObjectStateFlags* state = MEMUTIL_READ_BY_PTR_OFFSET(this, 0x30, SObjectStateFlags*);

    //TEST
    if (this->m_pLifeState->m_btLifeState == LIFESTATE_ALIVE
        && this->m_pLifeState->m_btTeleportState == TELEPORTSTATE_NONE
        && state->btMsgBlockState != 1)
        return TRUE;

    return FALSE;
}

int CGObjPC::GetDBID() const
{
    if (this->m_pObjDataInstance == NULL)
        return -1;

    return this->m_pObjDataInstance->CharDBID;
}

CMsg* CGObjPC::AllocMsg(unsigned int param_1)
{
    return	CallVirtual<CMsg*(__thiscall*)(CGObjPC*, unsigned int)>(this, 158)(this, param_1);
    //	printf("p1 = %p   p2 = %d \n", this, param_1);
    //return reinterpret_cast<CMsgStreamBuffer * (__thiscall*)(CGObjPC*, unsigned int)>(0x004E0830)(this, param_1);
}
#define BROADCAST_MSG_TO_NEARBY_PLAYERS_FN_OFFSET	0x00484D90
void CGObjPC::BroadcastMsgToNearbyPlayers(CMsg* pMsg)
{
    const DWORD dwCallAddr = BROADCAST_MSG_TO_NEARBY_PLAYERS_FN_OFFSET;
    SWorldID* pstWorldID = new SWorldID();

    __asm mov esi, this;
    __asm mov edi, pMsg;
    __asm mov ecx, pstWorldID;
    __asm call dwCallAddr;
}
void CGObjPC::SendMsg(CMsg* param_1)
{
    reinterpret_cast<void(__thiscall*)(CGObjPC*, CMsg*)>(0x004E0860)(this, param_1);
}
void CGObjPC::UseBuffMaybe(unsigned int param_1, unsigned int param_2)
{
    //printf("p1 = %p   p2 = %d \n", param_1, param_2);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int)>(0x004ad320)(this, param_1, param_2);
}

void CGObjPC::FUN_004e5a10(unsigned int param_1, unsigned int param_2)
{
    printf("%d %d \n", param_1, param_2);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int)>(0x004e5a10)(this, param_1, param_2);
}
void CGObjPC::UpdateHwan(char hwanLevel)
{
    reinterpret_cast<void(__thiscall*)(CGObjPC*, char)>(0x004A9F40)(this, hwanLevel);
}



void CGObjPC::UpdatePVPCapeType(BYTE CapeType)
{
    // Avoid unnecesary updates
    if (CapeType <= 5)
        reinterpret_cast<void(__thiscall*)(CGObjPC*, BYTE)>(0x004F0E90)(this, CapeType);
}


bool CGObjPC::MoveTo(uint32_t param_1, unsigned short param_2, float param_3, float param_4, float param_5, unsigned int param_6)
{
    //printf("%d , %d, %.2lf, %.2lf, %.2lf, %d \n", param_1, param_2, param_3, param_4, param_5, param_6);
    return reinterpret_cast<bool(__thiscall*)(CGObjPC*, uint32_t*, unsigned short, float, float, float, unsigned int)>(0x004df590)(this, &param_1, param_2, param_3, param_4, param_5, param_6);
}

void CGObjPC::SetLiveDeleteItem(int slot, int reducecount)
{
    int operation_result = 0;
    return 	CallVirtual<void(__thiscall*)(CGObjPC*, int, int, int, int*, int, int)>(this, 140)(this, 0, slot, reducecount, &operation_result, 4, 0);
}
void CGObjPC::SetLiveItem(int slot, const char* itemcode)
{
    return CallVirtual<void(__thiscall*)(CGObjPC*, int, const char*)>(this, 141)(this, slot, itemcode);
}
#define UPDATE_SILK_FN_OFFSET						0x004F0940
void CGObjPC::UpdateSilk(int nSilk, int nSilkGift, int nSilkPoint, bool bSendPacket)
{
    const DWORD dwCallAddr = UPDATE_SILK_FN_OFFSET;

    __asm movzx eax, bSendPacket;

    __asm push eax;
    __asm mov esi, this;
    __asm mov ecx, nSilkPoint;
    __asm mov edx, nSilkGift;
    __asm mov eax, nSilk;

    __asm call dwCallAddr;
}

// 0052bf90
void CGObjPC::EngageBuffSkill(CGObjPC* pPC, const char* szSkillCodeName128)
{
    const DWORD dwCGObjPC_EngageBuffSkill_Addr = 0x004F1800;

    const double dblUnk1 = 1.00;
    const int nUnk2 = 1;

    __asm push nUnk2;
    __asm push szSkillCodeName128;
    __asm fld dblUnk1;
    __asm mov ecx, pPC;
    __asm call dwCGObjPC_EngageBuffSkill_Addr;
}
void CGObjPC::LiveSkill(int SkillID)
{
    DWORD32* Skillinfo = reinterpret_cast<DWORD32 * (__stdcall*)()>(0x5AA0B0)();
    Skillinfo[2] = SkillID;
    reinterpret_cast<int(__stdcall*)(DWORD32, DWORD32*)>(0x59B480)((DWORD32)this + 0xA30, Skillinfo);
}
unsigned __int16 CGObjPC::AddItem(const char* Codename, __int32 Amount, bool RandomizeStats, __int8 OptLevel)
{
    __int16 operation_result = 0;
    CallVirtual<void(__thiscall*)(CGObjPC*, __int32, const char*, __int32, __int16*, __int8, __int32, __int32, __int8, __int32, __int32, __int8, __int32)>(this, 139)(this, 0, Codename, Amount, &operation_result, 0, RandomizeStats ? 1 : 0, 0, OptLevel, 0, 1, 0, 0);
    return operation_result;
}
void CGObjPC::UpdateGold(__int64 Offset)
{
    // Check if offset is higher than int.MaxValue to avoid send a bugged message
    if (Offset > 0x7FFFFFFF)
        UpdateGold(Offset, 25, true, false);
    else
        UpdateGold(Offset, 25, true, true);
}
void CGObjPC::RemoveGold(__int64 Offset)
{
    // Check if offset is higher than int.MaxValue to avoid send a bugged message
    if (Offset > 0x7FFFFFFF)
        UpdateGold(-Offset, 25, true, false);
    else
        UpdateGold(-Offset, 25, true, true);
}
void CGObjPC::UpdateGold(__int64 Offset, __int32 Unknown, bool Realtime, bool ShowMessage)
{
    CallVirtual<void(__thiscall*)(CGObjPC*, __int64, __int32, __int32, __int32)>(this, 91)(this, Offset, Unknown, (Realtime ? 1 : 0), (ShowMessage ? 1 : 0));
}

void CGObjPC::SetLivePlus(unsigned int param_1, unsigned int param_2, unsigned int param_3, unsigned int param_4, unsigned int param_5, unsigned int param_6)
{
    //printf("%d , %d, %d, %d, %d, %d\n", param_1, param_2, param_3, param_4, param_5, param_6);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)>(0x004eeb70)(this, param_1, param_2, param_3, param_4, param_5, param_6);
    //CallVirtual<void(__thiscall*)(CGObjPC*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)>(this, 140)(this, param_1, param_2, param_3, param_4, param_5);
}
//004eeb70
#define TELEPORT_TO_TARGET_POINT_FN_OFFSET			0x004DF290
void CGObjPC::TeleportToTown()
{
    try
    {
        const DWORD dwCallAddr = TELEPORT_TO_TARGET_POINT_FN_OFFSET;

        __asm pushad;
        __asm pushfd;

        __asm push 0;
        __asm mov eax, dword ptr[this];
        __asm call dwCallAddr;

        __asm popfd;
        __asm popad;
    }
    catch (...)
    {
        BS_INFO("%s - exception occoured on teleport to town", __FUNCTION__);
    }
}
void CGObjPC::B7150(CMsg* Offset)
{
    //printf("%p = msgID = %p \n", Offset, *Offset->m_wpMsgId);
    //*((int*)this + 0x2212) = 1;
    //*((int*)this + 0x2216) = 1;

    reinterpret_cast<void(__thiscall*)(CGObjPC*, CMsg*)>(0x00506e20)(this, Offset);
}


void CGObjPC::FUN_00586700(int param_1, int param_2, int param_3, int param_4)
{
    //printf("%d == %p == %d %d\n", param_1, param_2, param_3, param_4);
    //return;

    reinterpret_cast<void(__thiscall*)(CGObjPC*, int, int, int, int)>(0x004ea6a0)(this, param_1, param_2, param_3, param_4);
}

void CGObjPC::FUN_004f0750(int nSilk, int nSilkGift, int nSilkPoint)
{
    printf ("%d %d %d \n", nSilk, nSilkGift, nSilkPoint);
    reinterpret_cast<void(__thiscall*)(CGObjPC*, int, int, int)>(0x004f0750)(this, nSilk, nSilkGift, nSilkPoint);
}
#define ON_IGOBJ_ATTACK_REQ_FN_OFFSET        0x004aa640

CRegionAttackRestrictionsMgr::FN_IOBJ_CAN_ATTACK CRegionAttackRestrictionsMgr::s_pfnIGObj_CanAttack;

void CRegionAttackRestrictionsMgr::Initialize()
{
    s_pfnIGObj_CanAttack = reinterpret_cast<FN_IOBJ_CAN_ATTACK>(
            ON_IGOBJ_ATTACK_REQ_FN_OFFSET
    );

    DetourTransactionBegin();
    DetourAttach(&(PVOID&)s_pfnIGObj_CanAttack, CRegionAttackRestrictionsMgr::MyIGObj_CanAttack);
    DetourTransactionCommit();
}


BOOL __fastcall CRegionAttackRestrictionsMgr::MyIGObj_CanAttack(IGObj* pObjFirst, void* /* dummy edx */, IGObj* pObjSecond, int a3, WORD& pwResultCode)
{
    //Dunno if this ever happens, just in case better check !
    if (pObjSecond == NULL)
        return s_pfnIGObj_CanAttack(pObjFirst, pObjSecond, a3, pwResultCode);

    try
    {
        IGObj* pAttacker = NULL;
        IGObj* pVictim = NULL;
        CGObjPC* pPC = NULL;

        if (pObjFirst != NULL)
            pAttacker = (pObjFirst->IsCOS() ? pObjFirst->GetOwner() : pObjFirst);

        if (pObjSecond != NULL)
            pVictim = (pObjSecond->IsCOS() ? pObjSecond->GetOwner() : pObjSecond);

        if (pVictim == NULL)
            return s_pfnIGObj_CanAttack(pObjFirst, pObjSecond, a3, pwResultCode);

        if (pAttacker == NULL)
            return s_pfnIGObj_CanAttack(pObjFirst, pObjSecond, a3, pwResultCode);

        if (pObjFirst->IsPC() && pObjSecond->IsMonster())
        {
            CGObjMob* Mob = reinterpret_cast<CGObjMob*>(
                    g_pCGame->GetGameObjById(pObjSecond->GetGameID()));
            CGObjPC* pPC = reinterpret_cast<CGObjPC*>(
                    g_pCGame->GetGameObjById(pObjFirst->GetGameID()));
            if (Mob != NULL) {
                std::string charname = Mob->GetCodeName();
                if (Mob->Monsterclass == 3) {
                    size_t pos_f = charname.find("_STR");
                    size_t pos_f2 = charname.find("_INT");
                    int Str = pPC->m_pObjDataInstance->Strength;
                    int Int = pPC->m_pObjDataInstance->Intellect;
                    if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
                    {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }
                    else if (pos_f2 != -1 && Str > Int) {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }
                }
            }

            if (pPC != NULL && Mob != NULL)
            {
                if (pPC->PvpCape == 3)
                {
                    std::string strUniqueCodeName;
                    RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(Mob->GetRefObjID());
                    if (pRefObj != NULL)
                        strUniqueCodeName = pRefObj->m_strObjectName;

                    if (strUniqueCodeName.find("cape3") != std::string::npos)
                    {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }

                }
                else if (pPC->PvpCape == 1)
                {
                    std::string strUniqueCodeName;
                    RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(Mob->GetRefObjID());
                    if (pRefObj != NULL)
                        strUniqueCodeName = pRefObj->m_strObjectName;

                    if (strUniqueCodeName.find("cape1") != std::string::npos)
                    {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }
                }

            }

        }

        if (pObjFirst->IsMonster() && pObjSecond->IsPC())
        {
            //CGObjMob* mob = (CGObjMob*)g_pCGame->GetGameObjById(pObjSecond->GetGameID());
            CGObjPC* pPC = reinterpret_cast<CGObjPC*>(
                    g_pCGame->GetGameObjById(pObjSecond->GetGameID()));

            CGObjMob* Mob = reinterpret_cast<CGObjMob*>(
                    g_pCGame->GetGameObjById(pObjFirst->GetGameID()));


            if (pPC != NULL && Mob != NULL)
            {
                if (pPC->PvpCape == 3)
                {
                    std::string strUniqueCodeName;
                    RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(Mob->GetRefObjID());
                    if (pRefObj != NULL)
                        strUniqueCodeName = pRefObj->m_strObjectName;

                    if (strUniqueCodeName.find("cape3") != std::string::npos)
                    {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }

                }
                else if (pPC->PvpCape == 1)
                {
                    std::string strUniqueCodeName;
                    RefObjCommon* pRefObj = REFDATA_MGR.GetRefObj(Mob->GetRefObjID());
                    if (pRefObj != NULL)
                        strUniqueCodeName = pRefObj->m_strObjectName;

                    if (strUniqueCodeName.find("cape1") != std::string::npos)
                    {
                        pwResultCode = 0x3006;
                        return FALSE;
                    }
                }

            }

        }
        /*
        this will print invaild target so you cant attack it
            pwResultCode = 0x3006;
                        return FALSE;

        */

    }
    catch (std::exception& e)
    {
        printf("%s - Exception occoured %s", __FUNCTION__, e.what());
    }
    catch (...)
    {
        printf("%s - Exception occoured %s", __FUNCTION__);
    }


    return s_pfnIGObj_CanAttack(pObjFirst, pObjSecond, a3, pwResultCode);
}
void CGObjPC::SetGrantName(std::string* grantname)
{
    unsigned int CharID = this->GetDBID();
    __asm
    {
    push grantname;
    push 0x25;
    push this;

    mov ecx, CharID;
    xor edx, edx;

    mov eax, 0x005C80A0;
    call eax;
    }
    }

    CSkillManager* CGObjPC::GetSkillManager()
    {
        return (CSkillManager*)MEMUTIL_ADD_PTR(this, 0xA30);
    }

    void CGObjPC::CancelBuff(int nRefSkillID)
    {
        const DWORD dwCancelBuffCallAddr = 0x0059F0C0;

        __asm pushad;
        __asm pushfd;

        //pSkillMgr -> EDX
        __asm mov edx, this;
        __asm add edx, 0xA30;

        //Call function
        __asm mov ebx, nRefSkillID;
        __asm mov eax, edx;
        __asm call dwCancelBuffCallAddr;

        __asm popfd;
        __asm popad;
    }

