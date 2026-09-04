#include <Settings.h>
#include <DevNew/CustomNPCEvent.h>
#include "sqlCon.h"



CDbConnection* CSqlCon::m_connectionstr;
CRegionRestrictionDBSet* CSqlCon::s_pRegionRestrictionDbSet;
std::list<SServerAutoCapebyRegionID> CSqlCon::AutoCapeList;
std::list<SServerAutoCapebyWorldID> CSqlCon::AutoCapeListWorldId;
std::map<INT64, STimedItemPlusDbRecord> CSqlCon::TimedItemList;

std::map<INT64, STimedDevillPlusDbRecord> CSqlCon::STimedDevillList;


std::map<int, _RefAbilityByItemOptLevel> CSqlCon::RefAbilitybyItemOptLevel;
std::map<int, _RefSkillByItemOptLevel> CSqlCon::RefSkillByItemOptLevel;
std::map<int, _ServerFortressDpsInfo> CSqlCon::ServerFortressDpsInfo;
std::list<SCustomNpcInteractionDbRecord> CSqlCon::s_CustomNpcInteractions;

bool CSqlCon::TryExecNonQuery(char* szQuery)
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

void CSqlCon::Initialize()
{
    DatabaseCfgStruct* pstGameServerConfig = CSettings::GetGameServerConfig();

    //s_pAddonDbConnection = new CDbConnection(pstGameServerConfig->strAddonDbConn);
    m_connectionstr = new CDbConnection(pstGameServerConfig->strFilterContentDbConn);

    CAutoCriticalSection* pACS = new CAutoCriticalSection();
    s_pRegionRestrictionDbSet = new CRegionRestrictionDBSet(m_connectionstr, pACS);


    if (!m_connectionstr->Connect())
    {
        std::cout << __FUNCTION__ << " - Failed to connect to the filter content DB" << std::endl;
        //s_pAddonDbConnection->Disconnect();
        m_connectionstr->Disconnect();
    }
;
    /*ServerAutoCapebyRegionID();
    ServerAutoCapebyWorldID();    */
    LoadLockedItems();
    GetCustomNpcInteractionRecords(s_CustomNpcInteractions);
   // TimedPlusItems();
   // TimedDevillPlusItems();
   // LoadRefAbilitybyItemOptLevel();
   // LoadRefSkillByItemOptLevel();

    //LoadFortressDPSInfo();
}
CRegionRestrictionDBSet* CSqlCon::GetRegionRestrictionDbSet()
{
    return s_pRegionRestrictionDbSet;
}
bool CSqlCon::LoadFortressDPSInfo()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT StructObjID FROM _ServerFortressDpsInfo";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        _ServerFortressDpsInfo record;
        ZeroMemory(&record, sizeof(_ServerFortressDpsInfo));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.ObjID, 0, &cb);
        printf("%s - _ServerFortressDpsInfo %d\n", __FUNCTION__, record.ObjID);
        ServerFortressDpsInfo.insert((std::make_pair(record.ObjID, record)));
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

bool CSqlCon::ServerAutoCapebyWorldID()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT ID, WorldID FROM _ServerAutoCapebyWorldID";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        SServerAutoCapebyWorldID record;
        ZeroMemory(&record, sizeof(SServerAutoCapebyWorldID));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.nID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_USHORT, (SQLPOINTER)&record.wWorldID, 0, &cb);
        printf("%s - Load AutoCape regions %d\n", __FUNCTION__, record.wWorldID);
        AutoCapeListWorldId.push_back(record);
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

bool CSqlCon::ServerAutoCapebyRegionID()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT ID, RegionID FROM VFilter_User.._ServerAutoCapebyRegionID";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        SServerAutoCapebyRegionID record;
        ZeroMemory(&record, sizeof(SServerAutoCapebyRegionID));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.nID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_USHORT, (SQLPOINTER)&record.wRegionID, 0, &cb);
        printf("%s - Load AutoCape regions %d\n", __FUNCTION__, record.wRegionID);
        AutoCapeList.push_back(record);
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

bool CSqlCon::LoadRefSkillByItemOptLevel()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT Link, RefSkillID FROM SRO_VT_SHARD.._RefSkillByItemOptLevel";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        _RefSkillByItemOptLevel record;
        ZeroMemory(&record, sizeof(_RefSkillByItemOptLevel));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.Link, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_LONG, (SQLPOINTER)&record.RefSkillID, 0, NULL);

        //printf("%s - pluss Items %d %d\n", __FUNCTION__, record.Link, record.RefSkillID);
        RefSkillByItemOptLevel.insert((std::make_pair(record.Link, record)));
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}
bool CSqlCon::LoadRefAbilitybyItemOptLevel()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT ID, RefItemID, ItemOptLevel FROM SRO_VT_SHARD.._RefAbilityByItemOptLevel";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        _RefAbilityByItemOptLevel record;
        ZeroMemory(&record, sizeof(_RefAbilityByItemOptLevel));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.ID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_LONG, (SQLPOINTER)&record.RefItemID, 0, NULL);
        SQLGetData(hStmt, 3, SQL_C_SBIGINT, (SQLPOINTER)&record.OptLevel, 0, &cb);

  
        //printf("%s - pluss Items %d %d %d\n", __FUNCTION__, record.ID, record.RefItemID, record.OptLevel);
        RefAbilitybyItemOptLevel.insert((std::make_pair(record.ID, record)));
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}
bool CSqlCon::TimedPlusItems()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT CharID,OrjPlus,ID64,EndTime FROM _TimedItemPlus";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        STimedItemPlusDbRecord record;
        ZeroMemory(&record, sizeof(STimedItemPlusDbRecord));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.CharID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_LONG, (SQLPOINTER)&record.CurrentPlus, 0, NULL);
        SQLGetData(hStmt, 3, SQL_C_SBIGINT, (SQLPOINTER)&record.dwItemID64, 0, &cb);

        SQLGetData(hStmt, 4, SQL_C_UBIGINT, (SQLPOINTER)&record.endTime, 0, &cb);

        printf("%s - pluss Items %d %lld %d %ld\n", __FUNCTION__, record.CharID, record.CurrentPlus, record.dwItemID64, record.endTime);
        TimedItemList.insert((std::make_pair(record.dwItemID64, record)));
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

bool CSqlCon::TimedDevillPlusItems()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT CharID,OrjPlus,ID64,EndTime FROM _TimedDevillPlus";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        STimedDevillPlusDbRecord record;
        ZeroMemory(&record, sizeof(STimedDevillPlusDbRecord));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.CharID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_LONG, (SQLPOINTER)&record.CurrentPlus, 0, NULL);
        SQLGetData(hStmt, 3, SQL_C_SBIGINT, (SQLPOINTER)&record.dwItemID64, 0, &cb);

        SQLGetData(hStmt, 4, SQL_C_UBIGINT, (SQLPOINTER)&record.endTime, 0, &cb);

        printf("%s - devil Items %d %lld %d %ld\n", __FUNCTION__, record.CharID, record.CurrentPlus, record.dwItemID64, record.endTime);
        STimedDevillList.insert((std::make_pair(record.dwItemID64, record)));
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}


BYTE CSqlCon::GetItemBindingOpt(INT64 ID64)
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;
    BYTE OptList = 0;
    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return OptList;
    }

    SQLRETURN retCode;
    char szQuery[1024] = { 0 };
    sprintf(szQuery,
            "SELECT nOptValue FROM SRO_VT_SHARD.._BindingOptionWithItem with(nolock) where nItemDBID = %lld and bOptType = 2", ID64);

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return OptList;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return OptList;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return OptList;
    }

    SQLINTEGER cb;

    do
    {
        BYTE nOptID;

        SQLGetData(hStmt, 1, SQL_C_TINYINT, (SQLPOINTER)&nOptID, 0, NULL); // Sütun indeksi 1 olmalı
        printf("%s - aaa %d \n", __FUNCTION__, nOptID);
        OptList = nOptID;
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return OptList;
}




std::map<INT64, BYTE> CSqlCon::LockedItemList;
bool CSqlCon::LoadLockedItems()
{
    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;
    CDbConnection* m_pDbConnection = m_connectionstr;

    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT ItemID64 FROM _LockedItemList";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        INT64 ItemID64;
        SQLGetData(hStmt, 1, SQL_C_SBIGINT, (SQLPOINTER)&ItemID64, 0, NULL);

        LockedItemList.insert((std::make_pair(ItemID64, 0)));

        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);

    m_pACS->Leave();

    return true;
}

bool CSqlCon::GetCustomNpcInteractionRecords(std::list<SCustomNpcInteractionDbRecord>& result)
{

    CAutoCriticalSection* m_pACS = new CAutoCriticalSection();

    SQLHANDLE hStmt = SQL_NULL_HSTMT;

    CDbConnection* m_pDbConnection = m_connectionstr;


    if (!m_pDbConnection->AllocStmt(hStmt))
    {
        std::cout << __FUNCTION__ << " - Failed to allocate stmt" << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        return false;
    }

    SQLRETURN retCode;
    const LPCSTR szQuery = "SELECT ID, CodeName128, InteractionID FROM ___CustomNpcInteraction";

    m_pACS->Enter();

    retCode = SQLExecDirectA(hStmt, (SQLCHAR*)szQuery, SQL_NTS);
    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLExecDirect failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    retCode = SQLFetch(hStmt);

    if (retCode == SQL_NO_DATA)
    {
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return true;
    }

    if (!SQL_SUCCEEDED(retCode))
    {
        std::cout << __FUNCTION__ << " - SQLFetch failed, query = " << szQuery << std::endl;
        m_pDbConnection->FreeStmt(hStmt);
        m_pACS->Leave();
        return false;
    }

    SQLINTEGER cb;

    do
    {
        SCustomNpcInteractionDbRecord record;
        ZeroMemory(&record, sizeof(SCustomNpcInteractionDbRecord));

        SQLGetData(hStmt, 1, SQL_C_LONG, (SQLPOINTER)&record.nID, 0, &cb);
        SQLGetData(hStmt, 2, SQL_C_CHAR, (SQLPOINTER)&record.szCodeName128, sizeof(record.szCodeName128), &cb);
        SQLGetData(hStmt, 3, SQL_C_LONG, (SQLPOINTER)&record.nInteractionID, 0, &cb);

        result.push_back(record);
        retCode = SQLFetch(hStmt);
    } while (SQL_SUCCEEDED(retCode));

    m_pDbConnection->FreeStmt(hStmt);
    m_pACS->Leave();
    return true;
}

