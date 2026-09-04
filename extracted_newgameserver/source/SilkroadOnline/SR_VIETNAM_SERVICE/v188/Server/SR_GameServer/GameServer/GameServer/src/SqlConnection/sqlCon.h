#pragma once

#include <RegionRestrictionDBSet.h>
#include "DbConnection.h"
#include "AutoCriticalSection.h"
#include "../../../../OutPut/src/Util.h"

struct _RefSkillByItemOptLevel
{
    int Link;
    int RefSkillID;
};
struct _RefAbilityByItemOptLevel
{
    int ID;
    int RefItemID;
    BYTE OptLevel;
};

struct STimedItemPlusDbRecord
{
    int CharID;
    INT64 dwItemID64;
    long endTime;
    int CurrentPlus;
};
struct STimedDevillPlusDbRecord
{
    int CharID;
    INT64 dwItemID64;
    long endTime;
    int CurrentPlus;
};

struct SServerAutoCapebyRegionID
{
    int nID;
    WORD wRegionID;
};
struct SServerAutoCapebyWorldID
{
    int nID;
    WORD wWorldID;
};

struct SServerJobMobExpList
{
    int nID;
    int MobID;
    int MobClass;
    int Exp;
};

struct _ServerFortressDpsInfo
{
    int ObjID;
};
struct SCustomNpcInteractionDbRecord
{
    int nID;
    char szCodeName128[128];
    int nInteractionID;
};

class CSqlCon
{
public:
    static std::map<INT64, STimedDevillPlusDbRecord> STimedDevillList;
    static std::map<INT64, STimedItemPlusDbRecord> TimedItemList;
    static std::map<int, _RefAbilityByItemOptLevel> RefAbilitybyItemOptLevel;
    static std::map<int, _RefSkillByItemOptLevel> RefSkillByItemOptLevel;

    static std::map<int, _ServerFortressDpsInfo> ServerFortressDpsInfo;

    static CDbConnection* m_connectionstr;
    static CRegionRestrictionDBSet* s_pRegionRestrictionDbSet;
    static bool TryExecNonQuery(char* szQuery);
    static void Initialize();
    static CRegionRestrictionDBSet* GetRegionRestrictionDbSet();
    static bool LoadFortressDPSInfo();
    static bool ServerAutoCapebyWorldID();
    static std::list<SServerAutoCapebyRegionID> AutoCapeList;
    static std::list<SServerAutoCapebyWorldID> AutoCapeListWorldId;
    static std::list<SCustomNpcInteractionDbRecord> s_CustomNpcInteractions;



    static bool ServerAutoCapebyRegionID();


    static bool LoadRefSkillByItemOptLevel();

    static bool LoadRefAbilitybyItemOptLevel();

    static bool TimedPlusItems();
    static bool TimedDevillPlusItems();




    static bool LoadLockedItems();
    static std::map<INT64, BYTE> LockedItemList;

    static BYTE GetItemBindingOpt(INT64 ID64);

    static bool GetCustomNpcInteractionRecords(std::list<SCustomNpcInteractionDbRecord> &result);
};