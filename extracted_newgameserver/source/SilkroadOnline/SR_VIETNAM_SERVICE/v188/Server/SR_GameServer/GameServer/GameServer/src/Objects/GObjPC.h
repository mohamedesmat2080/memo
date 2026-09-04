//
// Created by Kurama on 12/11/2022.
//
#pragma once
#include "InstanceObj.h"
#include "IGObj.h"
#include "GObjChar.h"
#include <memory/hook.h>
#include "CGObjCOS_GoldPet.h"
#include "Guild.h"
#include "SkillManager.h"
//Size: 0x0040


#pragma pack(push, 1)
struct SObjectStateFlags
{
    //Dunno exact count atm fck it
    BYTE first;
    BYTE second;
    BYTE third;
    BYTE fourth;
    BYTE btMsgBlockState;
};
#pragma pack(pop)
class CGObjPC : public CGObjChar
{
public:
    typedef IGObj* (__thiscall* FN_GETITEMATSTORAGESLOT)(CGObjPC* pPC, int nSlotIndex);
    typedef void(__thiscall* FN_OFFSET_EXP_POINT)(CGObjPC*, DWORD dwExpSourceObjUniqueID, long long qwLevelExp, long long qwSkillExp, DWORD& dwUnk);
    static FN_OFFSET_EXP_POINT s_pfnOffsetExpPoint;
    static FN_GETITEMATSTORAGESLOT s_pfnGetItemAtStorageSlot;
    IGObj* GetItemAtStorageSlot(int nSlotIndex);
    static void Setup();
    void GiveExp(DWORD dwExpSourceObjUniqueID, long long qwLevelExp, long long qwSkillExp, DWORD& dwUnk);
    void Ressurect(float fRecoverHpMult, float fRecoverMpMult);
    void UpdateHpMp(int nHp, int nMp);
    void FUN_0048b810(void* p1, char p3);

    void UseItemFunction(unsigned int *pMsg);
    void Fun_383(CMsg *test);

    void Func_93(int p1, int p2);

    void FuncWhat();

    void Func_201();

    void Func_91(int param_1, int param_2, int param_3, int param_4);

    void GroupSpawn(unsigned int p1, int p2);

    unsigned int Func_3911();

    void KillLoggerFunction(CGObjPC* param_1, int param_2);
    BOOL CanSendMessage();
    int GetDBID() const;
    CMsg* AllocMsg(unsigned int param_1);
    void SendMsg(CMsg *param_1);
    void BroadcastMsgToNearbyPlayers(CMsg *pMsg);
    void UseBuffMaybe(unsigned int param_1, unsigned int param_2);
    void FUN_004e5a10(unsigned int param_1, unsigned int param_2);

    //void ReadMsg(CMsg *param_1);

    void SetGrantName(std::string* grantname);
    void CancelBuff(int nRefSkillID);
    void UpdateHwan(char hwanLevel);
    void FUNCMovement(unsigned int param);
    CSkillManager* GetSkillManager();
    void FUN_00485bb0(unsigned int param1, unsigned int param2, unsigned int param3);


    void Fun_004a9b70(unsigned int p1, unsigned int p2);

    void FUN348(unsigned int param);

    void FUN343(unsigned int param1, unsigned int param2, unsigned int param3);



    void FUN_004aa410();
    void setspeed(float p1, float p2);
    void FUN_004b3000(unsigned int p1, unsigned int p2, float p3);

    void fun(unsigned int a1, unsigned int a2, unsigned int a3);

    void ReaderPacket(CMsg *pMsg);
    void SetCharState(char a2);
    void UpdateItemPlusInDatabase(UINT64 ID64, byte Plus);
    void FuncJobExp(unsigned int p1, unsigned int p2, unsigned int p3);
    void FuncJobExpTest(unsigned int p1);
    void FuncJobExpTest2(unsigned int p1);
    void Func_373(CMsg* p1, unsigned int p2);
    void Func_386(CMsg* p1);
    void CharacterStats();
    int Testnew2(int p2, int p3, float p4);
    void LivedpsMobAttackRecorder(int param_1, int p2, int p3);
    void NewX(int param_1);
    void Fun_385(CMsg *param_1);
    void UpdatePVPCapeType(BYTE CapeType);
    bool MoveTo(uint32_t param_1, unsigned short param_2, float param_3, float param_4, float param_5, unsigned int param_6);
    void SetLiveDeleteItem(int slot, int reducecount);
    void SetLiveItem(int slot, const char* itemcode);
    void UpdateSilk(int nSilk, int nSilkGift, int nSilkPoint, bool bSendPacket);
    void FUN_004f0750(int nSilk, int nSilkGift, int nSilkPoint);
    void EngageBuffSkill(CGObjPC* pPC, const char* szSkillCodeName128);
    void LiveSkill(int SkillID);
    unsigned __int16 AddItem(const char* Codename, __int32 Count, bool RandomizeStats, __int8 OptLevel);

    void UpdateGold(__int64 Offset);
    void RemoveGold(__int64 Offset);
    void DealDamage(int nReasonMask, int nAmount);
    void SetLivePlus(unsigned int param_1, unsigned int param_2, unsigned int param_3, unsigned int param_4, unsigned int param_5, unsigned int param_6);
    void TeleportToTown();
    void B7150(CMsg* Offset);
    void FUN_00586700(int param_1, int param_2, int param_3, int param_4);
private:
    void UpdateGold(__int64 Amount, __int32 Unknown, bool Realtime, bool ShowMessage);

public:

    char pad_1CA0[28]; //0x1CA0
    CGuild* MyGuild; //0x1CBC
    char pad_1CC0[8]; //0x1CC0
    void* CMessenger; //0x1CC8
    char pad_1CCC[76]; //0x1CCC
    CGObjCOS_GoldPet* GoldPetPtr; //0x1D18
    char pad_1D1C[88]; //0x1D1C
    CGStorage Storage; //0x1D74
    char pad_1D98[400]; //0x1D98
    uint8_t N00000CF4; //0x1F28
    uint8_t N00004499; //0x1F29
    uint8_t Walking; //0x1F2A
    uint8_t N0000449C; //0x1F2B
    char pad_1F2C[8]; //0x1F2C
    uint8_t N00000CF7; //0x1F34
    uint8_t N000044FB; //0x1F35
    uint8_t OnPet; //0x1F36
    uint8_t N000044FC; //0x1F37
    char pad_1F38[172]; //0x1F38
    CGStorage CGStoragee; //0x1FE4
    char pad_2008[484]; //0x2008
    int PvpCape; //0x21EC
    char pad_21F0[280]; //0x21F0


    //char pad_1CA0[28]; //0x1CA0
    //CGuild* MyGuild; //0x1CBC
    //char pad_1CC0[8]; //0x1CC0
    //void* CMessenger; //0x1CC8
    //char pad_1CCC[76]; //0x1CCC
    //CGObjCOS_GoldPet* GoldPetPtr; // 0x1D18
    //char pad_1D1C[88]; //0x1D1C
    //CGStorage Storage; //0x1D74
    //char pad_1D98[588]; //0x1D98
    //CGStorage CGStoragee; //0x1FE4
    //char pad_2008[768]; //0x2008

    //char pad_1CA0[28]; //0x1CA0
    //CGuild* CGuild; //0x1CBC
    //char pad_1CC0[8]; //0x1CC0
    //void* CMessenger; //0x1CC8
    //char pad_1CCC[76]; //0x1CCC
    //void* GoldPetPtr; //0x1D18
    //char pad_1D1C[88]; //0x1D1C
    //CGStorage Storage; //0x1D74
    //char pad_1D98[588]; //0x1D98
    //CGStorage CGStoragee; //0x1FE4
    //char pad_2008[744]; //0x2008

private:
BEGIN_FIXTURE()
        ENSURE_SIZE(0x2308)
        ENSURE_OFFSET(MyGuild, 0x1CBC)
        ENSURE_OFFSET(CMessenger, 0x1CC8)
        ENSURE_OFFSET(Storage, 0x1D74)
        ENSURE_OFFSET(CGStoragee, 0x1FE4)
        ENSURE_OFFSET(GoldPetPtr, 0x1d18)
    END_FIXTURE()

    RUN_FIXTURE(CGObjPC)


    bool ItemIsWeapon(TypeId TID);

    bool ItemIsArmor(TypeId TID);

    bool ItemIsAccessory(TypeId TID);

    bool ItemIsShield(TypeId TID);

    bool ItemIsEnhancer(TypeId TID);

    bool ItemIsProofStone(TypeId TID);

    void Send3040(byte ItemSlot, byte NewOptLevel);
}; //Size: 0x2308


class CGObjPC;
class IGObj;


class CRegionAttackRestrictionsMgr
{
private:
    typedef BOOL(__thiscall* FN_IOBJ_CAN_ATTACK)(IGObj* pObjFirst, IGObj* pObjSecond, int a3, WORD& pwResultCode);

    static FN_IOBJ_CAN_ATTACK s_pfnIGObj_CanAttack;

private:
    static BOOL __fastcall MyIGObj_CanAttack(IGObj* pObjFirst, void*, IGObj* pObjSecond, int a3, WORD& pwResultCode);
public:
    static void Initialize();
};