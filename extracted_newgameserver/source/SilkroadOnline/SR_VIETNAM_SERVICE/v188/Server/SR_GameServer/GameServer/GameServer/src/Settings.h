#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
// Windows Header Files
#include <sstream>
#include <list>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <map>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <sqlucode.h>
#include <Interface/IGObj.h>

#include "memory/detours.h"
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "ODBC32.lib")


typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define X_DEBUG TRUE

//Assembly mnemonics
#define ASM_NOP 0x90
#define ASM_JMP 0xE9 //LONG
#define ASM_CALL 0xE8 //LONG

struct DatabaseCfgStruct
{
    //std::string strAddonDbConn;
    std::string strFilterContentDbConn;
    std::string databasename;
    int SERVER_MAX_LEVEL;
    int CH_MAX_MASTERY_LEVEL;
    int EU_MAX_MASTERY_LEVEL;
    int MIN_PK_LEVEL;
    int MIN_PK_LEVEL_FOR_DROP_ITEM;
    unsigned __int64 STALL_EXCHANGE_GOLD_LIMIT;
    bool HIGH_RATES_CONFIG;
    bool FIX_EXPLOIT_INVISIBLE_INVINCIBLE;
    bool FIX_GRAP_PET_PAGE;
    int AGENT_SERVER_CAPACITY;
    int GRAP_PET_INVENTORY_SIZE;
    int PARTY_LEVEL_MIN;
    int PARTY_MOB_MEMBERS_REQUIRED;
    int PARTY_MOB_SPAWN_PROBABILITY;
    int PENALTY_DROP_PROBABILITY;
    int RESURRECT_SAME_POINT_LEVEL_MAX;
    int NPC_RETURN_DEAD_LEVEL_MAX;
    int BEGINNER_MARK_LEVEL_MAX;
    int DROP_ITEM_MAGIC_PROBABILITY;
    int PENALTY_DROP_LEVEL_MIN;

    int JOB_LEVEL_MAX;
    bool DISABLE_MOB_SPAWN;
    int TEMPLE_LEVEL;
    bool EXCHANGE_ATTACK_CANCEL;
    bool GUILD_POINTS;
    int MEMBERS_LIMIT_LEVEL1;
    int MEMBERS_LIMIT_LEVEL2;
    int MEMBERS_LIMIT_LEVEL3;
    int MEMBERS_LIMIT_LEVEL4;
    int MEMBERS_LIMIT_LEVEL5;
    int STORAGE_SLOTS_MIN;
    int STORAGE_SLOTS_INCREASE;
    int UNION_LIMIT;
    int UNION_CHAT_PARTICIPANTS;

    int MIN_GUILD_LEVEL_FOR_MERCENARY_SPAWN;
    bool ALLOW_NON_GM_MERCENARY_SPAWN;
    bool DISABLE_GRANT_NAME_CONDITIONS;

    int DISBAND_PENALTY_TIME;

    int FUSING_DELAY;
    int STONE_ASTRAL_VALUE;

    std::string CTF_ITEM_WIN_REWARD;
    int CTF_ITEM_WIN_REWARD_AMOUNT;
    std::string CTF_ITEM_KILL_REWARD;
    int CTF_ITEM_KILL_REWARD_AMOUNT;
    std::string BA_ITEM_REWARD;
    int BA_ITEM_REWARD_GJ_W_AMOUNT;
    int BA_ITEM_REWARD_GJ_L_AMOUNT;
    int BA_ITEM_REWARD_PR_W_AMOUNT;
    int BA_ITEM_REWARD_PR_L_AMOUNT;

    int GsID;
};
typedef bool(__thiscall* fnCGObjNPCNPC_SetAvailableInteractions)(IGObj*, int a2, int a3, int a4, int a5, int a6);
typedef void* (__thiscall* fnCGObjNPCNPC_AddAvailableInteraction)(IGObj*, int nInteractionID);


class CSettings
{
public:
    CSettings();

    static DatabaseCfgStruct* m_Settings;
    static void LoadIniSettings();
    static void Patches();
    static void Initialize();
    static int GetProcessInstanceId();
    static DatabaseCfgStruct* GetGameServerConfig();

    static void StringReplaceAll(std::string& Value, const std::string& From, const std::string& To)
    {
        if (From.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = Value.find(From, start_pos)) != std::string::npos)
        {
            Value.replace(start_pos, From.length(), To);
            start_pos += To.length(); // Increase the same length in case 'To' contains 'From', like replacing 'x' with 'yx'
        }
    }
};


namespace AsmEdition
{
    // Jump back to the code flow from donating guild points
    static DWORD jmpAddr_DonateGP = 0x005C413A;
    // Hook on guild point increasing (0x005C4135)
    static _declspec(naked) void OnDonateGuildPoints()
    {
        // Rebuild asm
        __asm
        {
        mov ecx, dword ptr[eax + 0x3c] // rebuild
        add ecx, esi // rebuild
        cmp ecx, 0x7FFFFFFF // compare ecx with int.MaxValue
        jbe _continue // go to _continue if ecx <= int.MaxValue
        mov ecx, 0x7FFFFFFF // set ecx as int.MaxValue
        jmp _continue // go to _continue
        }
        // Contine code flow
        _continue:
        __asm jmp jmpAddr_DonateGP;
    }
}





#define TO_NWSTRING(x) acp_n_decode(x)
#define TO_WSTRING(x) acp_decode(x)

#define TO_STRING(x) acp_encode(x)
#define TO_NSTRING(x) acp_n_encode(x)


// primitive to std::string
std::string acp_encode(const wchar_t* text, size_t length);
// std::wstring to std::string
std::string acp_encode(const std::wstring& wstr);
std::string acp_encode(const std::wstring* wstr);




// primitive to std::wstring
std::wstring acp_decode(const char* str, size_t length);


// std::string to std::wstring
std::wstring acp_decode(const std::string& str);
std::wstring acp_decode(const std::string* str);
