//
// Created by YUMBUL on 19.03.2023.
//

#include <cstdio>
#include <Game.h>
#include "GObjMob.h"
#include "GObjPC.h"
#include "RegionManagerBody.h"
#include <SqlConnection/sqlCon.h>

void CGObjMob::LivedpsMobAttackRecorder7(int param_1, int p2, int p3) {

    printf("%p %p %p %p %p \n", param_1, p2, p3);
    reinterpret_cast<void(__thiscall*)(CGObjMob*, int, int, int)>(0x004aa640)(this, param_1, p2, p3);
}

void CGObjMob::LivedpsMobAttackRecorder2(int param_1, int p2, int p3, int p4, int p5) {
    printf("%d %d %d %d %d\n", param_1, p2, p3, p3, p4);
    reinterpret_cast<void(__thiscall*)(CGObjMob*, int, int, int, int, int)>(0x0052a940)(this, param_1, p2, p3, p4, p5);
}
unsigned int CGObjMob::UpdateHPMPPacket()
{
    return 0;
    unsigned int aa = reinterpret_cast<unsigned int(__thiscall*)(CGObjMob*)>(0x004a8510)(this);
    //printf("%d\n", aa);
    
    return aa;
}
void CGObjMob::LivedpsMobAttackRecorder(void* attacker, int p2, int p3, int p4, int p5) {


    //return;
   /* if (param_1 != NULL) {
        std::string charname = this->GetCodeName();

            size_t pos_f = charname.find("_STR");
            size_t pos_f2 = charname.find("_INT");
            int Str = this->m_pObjDataInstance->Strength;
            int Int = this->m_pObjDataInstance->Intellect;
            if (pos_f != -1 && Str < Int)/// e�er mob str ise ve
            {
                CMsg* newpMsg = param_1->AllocMsg(0x3563);
                byte type = 0;
                *newpMsg << type;
                param_1->SendMsg(newpMsg);

                reinterpret_cast<void(__thiscall*)(CGObjMob*, CGObjPC*, int, int, int, int)>(0x0052a8e0)(this, param_1, 0, 0, 0, 0);
            }
            else if (pos_f2 != -1 && Str > Int) {

                CMsg* newpMsg = param_1->AllocMsg(0x3563);
                byte type = 1;
                *newpMsg << type;
                param_1->SendMsg(newpMsg);

                reinterpret_cast<void(__thiscall*)(CGObjMob*, CGObjPC*, int, int, int, int)>(0x0052a8e0)(this, param_1, 0, 0, 0, 0);
            }
            else
            {
                reinterpret_cast<void(__thiscall*)(CGObjMob*, CGObjPC*, int, int, int, int)>(0x0052a8e0)(this, param_1, p2, p3, p4, p5);
            }

    }*/
    reinterpret_cast<void(__thiscall*)(CGObjMob*, void*, int, int, int, int)>(0x0052a8e0)(this, attacker, p2, 1, 1, 0);

    if (this->Monsterclass != 3)
        return;
    std::map<DWORD, SAggroMapSecondPairItem>::iterator itCur = MyMap.begin();
    std::map<DWORD, SAggroMapSecondPairItem>::iterator itEnd = MyMap.end();

    if (itCur == itEnd)
        return;

    std::map<DWORD, SAggroMapSecondPairItem>::iterator first_record =
        MyMap.begin();

    int firstPlayerGID = first_record->first;

    //Can cast to CGObjPC ?
    CGObjPC* pFirstObj = reinterpret_cast<CGObjPC*>(
        g_pCGame->GetGameObjById(firstPlayerGID));
    int sayi = 0;
    while (pFirstObj == NULL) {
        ++itCur;
        std::pair<DWORD, SAggroMapSecondPairItem> aggro_info2 = *itCur;
        SAggroMapSecondPairItem gid_dmg_pair = aggro_info2.second;
        pFirstObj = reinterpret_cast<CGObjPC*>(
            g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
        sayi++;
    }


    CMsg* pMsg = pFirstObj->AllocMsg(0x5010);

    *pMsg << this->GetRefObjID();
    *pMsg << MyMap.size() - sayi;

    while (itCur != itEnd) {
        std::pair<DWORD, SAggroMapSecondPairItem> aggro_info = *itCur;

        SAggroMapSecondPairItem gid_dmg_pair = aggro_info.second;

        CGObjPC* pPC = reinterpret_cast<CGObjPC*>(g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
        if (pPC != NULL) {
            std::string Charname = pPC->GetCharName();
            *pMsg << Charname;

        }
        else {
            std::string Testss = "";
            *pMsg << Testss;
        }
        *pMsg << gid_dmg_pair.dwDamage;


        ++itCur;
    }

    pFirstObj->SendMsg(pMsg);
    pMsg->m_wWriteDataArrayPos = 0;

    //return;
 /*   if (this->Monsterclass == 3)
    {
        for (std::map<DWORD, SAggroMapSecondPairItem>::iterator it = MyMap.begin(); it != MyMap.end(); it++)
        {
            CGObjPC* pFirstObj = reinterpret_cast<CGObjPC*>(
                g_pCGame->GetGameObjById(it->first));
            if (pFirstObj != NULL)
            {

            }
        }
    }*/
}

CGObjPC *CGObjMob::MobDamageMeter(unsigned int a2) {
    try {
        //std::cout << __FUNCTION__ << " -> ID " << pObj->GetRefObjID() << " CGObjNPC" << std::endl;
        CGObjPC *aa = reinterpret_cast<CGObjPC *(__thiscall *) (CGObjMob *, unsigned int)>(0x004c44a0)(this, a2);
        /*       bool isUniqueMonster = CRegionRestrictionMgr::IsFortressHeartStruct(this->GetRefObjID());

        if (!isUniqueMonster)
            return aa;*/
        if (this->Monsterclass != 3)
            return aa;
        std::map<DWORD, SAggroMapSecondPairItem>::iterator itCur = MyMap.begin();
        std::map<DWORD, SAggroMapSecondPairItem>::iterator itEnd = MyMap.end();

        if (itCur == itEnd)
            return aa;

        std::map<DWORD, SAggroMapSecondPairItem>::iterator first_record =
                MyMap.begin();

        int firstPlayerGID = first_record->first;

        //Can cast to CGObjPC ?
        CGObjPC *pFirstObj = reinterpret_cast<CGObjPC *>(
                g_pCGame->GetGameObjById(firstPlayerGID));
        int sayi = 0;
        while (pFirstObj == NULL) {
            ++itCur;
            std::pair<DWORD, SAggroMapSecondPairItem> aggro_info2 = *itCur;
            SAggroMapSecondPairItem gid_dmg_pair = aggro_info2.second;
            pFirstObj = reinterpret_cast<CGObjPC *>(
                    g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
            sayi++;
        }


        CMsg *pMsg = pFirstObj->AllocMsg(0x5010);

        *pMsg << this->GetRefObjID();
        *pMsg << MyMap.size() - sayi;

        while (itCur != itEnd) {
            std::pair<DWORD, SAggroMapSecondPairItem> aggro_info = *itCur;

            SAggroMapSecondPairItem gid_dmg_pair = aggro_info.second;

            CGObjPC *pPC = reinterpret_cast<CGObjPC *>(g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
            if (pPC != NULL) {
                std::string Charname = pPC->GetCharName();
                *pMsg << Charname;

            } else {
                std::string Testss = "";
                *pMsg << Testss;
            }
            *pMsg << gid_dmg_pair.dwDamage;


            ++itCur;
        }

        pFirstObj->SendMsg(pMsg);
        pMsg->m_wWriteDataArrayPos = 0;


        return aa;
    } catch (...) {
        CGObjPC *aa = reinterpret_cast<CGObjPC *(__thiscall *) (CGObjMob *, unsigned int)>(0x004c44a0)(this, a2);
        return aa;
    }
}

void CGObjMob::Mobkillquest(CGObjPC* param_1, int param_2)
{
    if (param_2 == 4)
    {
        CMsg* pMsg = param_1->AllocMsg(0x5014);
        int ObjID = this->GetRefObjID();
        *pMsg << this->Monsterclass;
        *pMsg << param_1->RegionID;
        *pMsg << ObjID;


        if(this->Monsterclass == 3)
        {
            SPosInfo minePos;
            this->GetPosInfo(minePos);
            *pMsg << (unsigned short)minePos.wRegionID;
            *pMsg << minePos.fltX;
            *pMsg << minePos.fltY;
            *pMsg << minePos.fltZ;
        }


        param_1->SendMsg(pMsg);
        pMsg->m_wWriteDataArrayPos = 0;

    }
    reinterpret_cast<void(__thiscall*)(CGObjMob*, CGObjPC*, int)>(0x004a7540)(this, param_1, param_2);
}

struct CreatedMobList
{
    uint32_t WorldID;
    int MobID;
    int UniqueID;
};
std::vector<CreatedMobList>MobList;

CGObjMob* CGObjMob::MonsterAdd(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned int p5)
{
    //printf("%d ! %d ! %d ! %d ! %d \n", p1, p2, p3, p4, p5);
    return reinterpret_cast<CGObjMob*(__thiscall*)(CGObjMob*, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int)>(0x004c10c0)(this, p1, p2, p3, p4, p5);
}
void CGObjMob::MonsterRemove(CGObjMob* p1)
{
    //printf("%d ! \n", p1);
    reinterpret_cast<void*(__thiscall*)(CGObjMob*, CGObjMob*)>(0x004c10c0)(this, p1);
}
CGObjMob* CGObjMob::CreateMob(uint32_t RefObjId, uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ, float GenerateRadius)
{
    // Create position
    NavInfo posNavInfo(PosInfo(RegionId, PosX, PosY, PosZ));

    // Check if can be solved
    if (!CRegionManagerBody::ResolveCellAndHeight(&posNavInfo))
    {
        //printf(" * [CreateMob] Couldn't resolve cell & height. | [%d - %f, %f, %f]\n", RegionId, PosX, PosY, PosZ);
        return NULL;
    }

    // Check if the data from object can be found
    void * const objBaseDataPtr = GetRefObjBaseData(RefObjId);
    if (!objBaseDataPtr)
    {
        //printf(" * [CreateMob] Couldn't find the object data. | [%d]", RefObjId);
        return NULL;
    }

    // Retrieve base data
    int objCreepType = GetCreepType(objBaseDataPtr);
    void* objRefTacticPtr = GetRefTactic(objBaseDataPtr);
    float objDir = rand() / 32767.f * 6.283185482025146f;

    CGObjMob* _eax;

    __asm
    {
    mov edx, 0x400000
    lea esi, [edx + 0x1F6EB0]	// base_addr + CreateObj(RVA)
    lea edi, [edx + 0x90B380]	// base_addr + CGameWorldMgr(RVA of Instance)
    lea eax, [posNavInfo]		// nav_info

    push 1										// unk
    push 0										// unk
    push 0										// unk
    push 0										// unk
    push objCreepType							// obj_creep_type
    push GenerateRadius							// generate_radius
    push objDir									// obj_direction
    push dword ptr ss : [objRefTacticPtr]		// obj_ref_tactic_ptr
    push 0										// obj_ref_nest_ptr
    push dword ptr ss : [objBaseDataPtr]		// obj_ref_base_data_ptr
    push 0										// unk
    push GameWorldId								// unk
    push edi									// CGameWorldMgr instance
    call esi
    mov _eax, eax
    }
    if (_eax != NULL)
    {
        CGObjPC* pPC = reinterpret_cast<CGObjPC*>(
                g_pCGame->GetGameObjById(_eax->GetGameID())
        );
        //printf("UQID = %lu \n", pPC->m_dwGameId);
        CreatedMobList test = CreatedMobList();
        test.MobID = RefObjId;
        test.WorldID = GameWorldId;
        test.UniqueID = pPC->m_dwGameId;
        MobList.push_back(test);
        //pPC->DealDamage(999999999, 888888888);
       // CSqlCommands::RecordMobSpawn(pPC->m_dwGameId, pPC->GetCodeName());
    }
    return _eax;
}

const DWORD FN_DESPAWN_OBJ_FN_OFFSET = 0x00485E50;

void CGObjMob::DeleteMonster(int RefObjId)
{
    for (std::vector<CreatedMobList>::iterator it = MobList.begin(); it != MobList.end(); ++it)
    {
        if ((*it).MobID == RefObjId)
        {
            IGObj * mob = g_pCGame->GetGameObjById((*it).UniqueID);
            if (mob != NULL)
            {
                DespawnGObj(mob);
            }
            
        }
    }
}
void CGObjMob::DeleteMonsterByWorldID(int WorldID)
{
    for (std::vector<CreatedMobList>::iterator it = MobList.begin(); it != MobList.end(); ++it)
    {
        if ((*it).WorldID == (WorldID + 0x10000))
        {
            IGObj* mob = g_pCGame->GetGameObjById((*it).UniqueID);
            if (mob != NULL)
            {
                DespawnGObj(mob);
            }

        }
    }
}
void CGObjMob::DespawnGObj(IGObj* pObj)
{
    __asm pushad;
    __asm pushfd;
    __asm mov esi, pObj;
    __asm call FN_DESPAWN_OBJ_FN_OFFSET;
    __asm popfd;
    __asm popad;
}
CGObjMob *CGObjMob::SpawnMob(uint32_t RefObjId, uint32_t GameWorldId, uint16_t RegionId, float PosX, float PosY, float PosZ, float GenerateRadius) {
    // Create position
    NavInfo posNavInfo(PosInfo(RegionId, PosX, PosY, PosZ));

    // Check if can be solved
    if (!CRegionManagerBody::ResolveCellAndHeight(&posNavInfo)) {
        //printf(" * [CreateMob] Couldn't resolve cell & height. | [%d - %f, %f, %f]\n", RegionId, PosX, PosY, PosZ);
        return NULL;
    }

    // Check if the data from object can be found
    void *const objBaseDataPtr = GetRefObjBaseData(RefObjId);
    if (!objBaseDataPtr) {
        //printf(" * [CreateMob] Couldn't find the object data. | [%d]", RefObjId);
        return NULL;
    }

    // Retrieve base data
    int objCreepType = GetCreepType(objBaseDataPtr);
    void *objRefTacticPtr = GetRefTactic(objBaseDataPtr);
    float objDir = rand() / 32767.f * 6.283185482025146f;

    CGObjMob *_eax;

    __asm {
    mov edx, 0x400000
    lea esi, [edx + 0x1F6EB0]// base_addr + CreateObj(RVA)
    lea edi, [edx + 0x90B380]// base_addr + CGameWorldMgr(RVA of Instance)
    lea eax, [posNavInfo]// nav_info

    push 1// unk
    push 0// unk
    push 0// unk
    push 0// unk
    push objCreepType// obj_creep_type
    push GenerateRadius// generate_radius
    push objDir// obj_direction
    push dword ptr ss : [objRefTacticPtr]// obj_ref_tactic_ptr
    push 0// obj_ref_nest_ptr
    push dword ptr ss : [objBaseDataPtr]// obj_ref_base_data_ptr
    push 0// unk
    push GameWorldId// unk
    push edi// CGameWorldMgr instance
    call esi
    mov _eax, eax
    }
    return _eax;
}

void* CGObjMob::GetRefObjBaseData(int RefObjId)
{
    void* _eax;

    __asm
    {
    mov edx, 0x400000
    lea edx, [edx + 0x260B0]

    mov edi, dword ptr ds : [0xD6AA14]
    push RefObjId
    call edx
    mov _eax, eax
    }
    return _eax;
}


__int32 CGObjMob::GetCreepType(void* RefObjBaseDataPtr)
{
    return *reinterpret_cast<uint8_t*>(reinterpret_cast<unsigned int>(RefObjBaseDataPtr) + 0x89);
}

void* CGObjMob::GetRefTactic(void* RefObjBaseDataPtr)
{
    void* _eax;

    __asm
    {
    mov edx, 0x400000
    lea edx, [edx + 0x120D10]

    mov eax, dword ptr ss : [RefObjBaseDataPtr]
    call edx
    mov _eax, eax
    }
    return _eax;
}

