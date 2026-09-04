#include <memory/hook.h>
#include "Settings.h"
#include "IniReader.h"
#include "Process.h"
#include <Windows.h>
#include <string>
#include <BSObj/BSObj.h>
#include <SqlConnection/sqlCon.h>

DatabaseCfgStruct* CSettings::m_Settings = NULL;

#define GS_CONFIG_PATH									".\\JTGameServerCfg.ini"

#define PK_MIN_LEVEL_OFFSET												0x005295DB
#define PK_MIN_LEVEL_FOR_ITEM_DROP_OFFSET								0x004E6A34



void CSettings::LoadIniSettings()
{
    CIniReader* pReader = NULL;

    //==============================================================================================================================
    //GameServer common
    pReader = new CIniReader(GS_CONFIG_PATH);

    //s_pstGameServerConfig->strAddonDbConn = pReader->ReadStringA(strGsSection, "AddonDbConnStr", "[None]");
    m_Settings->strFilterContentDbConn = pReader->ReadStringA("SqlConnection", "SqlConnectionString", "[None]");
    //printf("%s\n", m_Settings->strFilterContentDbConn.c_str());

    m_Settings->SERVER_MAX_LEVEL = pReader->ReadInt("Server", "SERVER_MAX_LEVEL", 100);

    m_Settings->CH_MAX_MASTERY_LEVEL = pReader->ReadInt("Server", "CH_MAX_MASTERY_LEVEL", 330);
    m_Settings->EU_MAX_MASTERY_LEVEL = pReader->ReadInt("Server", "EU_MAX_MASTERY_LEVEL", 200);


    m_Settings->MIN_PK_LEVEL = pReader->ReadInt("Server", "MIN_PK_LEVEL", 20);
    m_Settings->MIN_PK_LEVEL_FOR_DROP_ITEM = pReader->ReadInt("Server", "MIN_PK_LEVEL_FOR_DROP_ITEM", 10);


    m_Settings->STALL_EXCHANGE_GOLD_LIMIT = pReader->ReadInt("Server", "STALL_EXCHANGE_GOLD_LIMIT", 999999999);

    m_Settings->HIGH_RATES_CONFIG = pReader->ReadBool("Server", "HIGH_RATES_CONFIG", false);

    m_Settings->GRAP_PET_INVENTORY_SIZE = pReader->ReadInt("Server", "GRAP_PET_INVENTORY_SIZE", 28);


    m_Settings->FIX_EXPLOIT_INVISIBLE_INVINCIBLE = pReader->ReadBool("Exploit", "FIX_EXPLOIT_INVISIBLE_INVINCIBLE", false);


    m_Settings->AGENT_SERVER_CAPACITY = pReader->ReadInt("Exploit", "FIX_AGENT_SERVER_CAPACITY", 1000);
    m_Settings->EXCHANGE_ATTACK_CANCEL = pReader->ReadBool("Exploit", "EXCHANGE_ATTACK_CANCEL", false);
    m_Settings->GUILD_POINTS = pReader->ReadBool("Exploit", "GUILD_POINTS", false);

    m_Settings->FIX_GRAP_PET_PAGE = pReader->ReadBool("Exploit", "FIX_GRAP_PET_PAGE", false);


    m_Settings->PARTY_LEVEL_MIN = pReader->ReadInt("Server", "PARTY_LEVEL_MIN", 5);
    m_Settings->PARTY_MOB_MEMBERS_REQUIRED = pReader->ReadInt("Server", "PARTY_MOB_MEMBERS_REQUIRED", 2);
    m_Settings->PARTY_MOB_SPAWN_PROBABILITY = pReader->ReadInt("Server", "PARTY_MOB_SPAWN_PROBABILITY", 50);
    m_Settings->PENALTY_DROP_PROBABILITY = pReader->ReadInt("Server", "PENALTY_DROP_PROBABILITY", 5);
    m_Settings->RESURRECT_SAME_POINT_LEVEL_MAX = pReader->ReadInt("Server", "RESURRECT_SAME_POINT_LEVEL_MAX", 10);
    m_Settings->NPC_RETURN_DEAD_LEVEL_MAX = pReader->ReadInt("Server", "NPC_RETURN_DEAD_LEVEL_MAX", 20);
    m_Settings->BEGINNER_MARK_LEVEL_MAX = pReader->ReadInt("Server", "BEGINNER_MARK_LEVEL_MAX", 19);
    m_Settings->DROP_ITEM_MAGIC_PROBABILITY = pReader->ReadInt("Server", "DROP_ITEM_MAGIC_PROBABILITY", 30);
    m_Settings->PENALTY_DROP_LEVEL_MIN = pReader->ReadInt("Server", "PENALTY_DROP_LEVEL_MIN", 10);

    m_Settings->JOB_LEVEL_MAX = pReader->ReadInt("Job", "JOB_LEVEL_MAX", 7);
    m_Settings->DISABLE_MOB_SPAWN = pReader->ReadBool("Job", "DISABLE_MOB_SPAWN", false);
    m_Settings->TEMPLE_LEVEL = pReader->ReadInt("Job", "TEMPLE_LEVEL", 105);


    m_Settings->MEMBERS_LIMIT_LEVEL1 = pReader->ReadInt("Guild", "MEMBERS_LIMIT_LEVEL1", 15);
    m_Settings->MEMBERS_LIMIT_LEVEL2 = pReader->ReadInt("Guild", "MEMBERS_LIMIT_LEVEL2", 20);
    m_Settings->MEMBERS_LIMIT_LEVEL3 = pReader->ReadInt("Guild", "MEMBERS_LIMIT_LEVEL3", 25);
    m_Settings->MEMBERS_LIMIT_LEVEL4 = pReader->ReadInt("Guild", "MEMBERS_LIMIT_LEVEL4", 35);
    m_Settings->MEMBERS_LIMIT_LEVEL5 = pReader->ReadInt("Guild", "MEMBERS_LIMIT_LEVEL5", 50);
    m_Settings->STORAGE_SLOTS_MIN = pReader->ReadInt("Guild", "STORAGE_SLOTS_MIN", 30);
    m_Settings->STORAGE_SLOTS_INCREASE = pReader->ReadInt("Guild", "STORAGE_SLOTS_INCREASE", 30);
    m_Settings->UNION_LIMIT = pReader->ReadInt("Guild", "UNION_LIMIT", 8);
    m_Settings->UNION_CHAT_PARTICIPANTS = pReader->ReadInt("Guild", "UNION_CHAT_PARTICIPANTS", 12);

    m_Settings->MIN_GUILD_LEVEL_FOR_MERCENARY_SPAWN = pReader->ReadInt("Guild", "MIN_GUILD_LEVEL_FOR_MERCENARY_SPAWN", 3);

    m_Settings->ALLOW_NON_GM_MERCENARY_SPAWN = pReader->ReadBool("Guild", "ALLOW_NON_GM_MERCENARY_SPAWN", false);

    m_Settings->DISABLE_GRANT_NAME_CONDITIONS = pReader->ReadBool("Guild", "DISABLE_GRANT_NAME_CONDITIONS", false);

    m_Settings->DISBAND_PENALTY_TIME = pReader->ReadInt("Academy", "DISBAND_PENALTY_TIME", 604800);

    m_Settings->FUSING_DELAY = pReader->ReadInt("Alchemy", "FUSING_DELAY", 3);

    m_Settings->STONE_ASTRAL_VALUE = pReader->ReadInt("Alchemy", "STONE_ASTRAL_VALUE", 4);


    m_Settings->CTF_ITEM_WIN_REWARD = pReader->ReadStringA("Event", "CTF_ITEM_WIN_REWARD", "ITEM_ETC_E080723_ICETROPHY");
    m_Settings->CTF_ITEM_WIN_REWARD_AMOUNT = pReader->ReadInt("Event", "CTF_ITEM_WIN_REWARD_AMOUNT", 1);


    m_Settings->CTF_ITEM_KILL_REWARD = pReader->ReadStringA("Event", "CTF_ITEM_KILL_REWARD", "ITEM_ETC_E080723_ICETROPHY");
    m_Settings->CTF_ITEM_KILL_REWARD_AMOUNT = pReader->ReadInt("Event", "CTF_ITEM_KILL_REWARD_AMOUNT", 1);


    m_Settings->BA_ITEM_REWARD = pReader->ReadStringA("Event", "BA_ITEM_REWARD", "ITEM_ETC_ARENA_COIN");
    m_Settings->BA_ITEM_REWARD_GJ_W_AMOUNT = pReader->ReadInt("Event", "BA_ITEM_REWARD_GJ_W_AMOUNT", 7);
    m_Settings->BA_ITEM_REWARD_GJ_L_AMOUNT = pReader->ReadInt("Event", "BA_ITEM_REWARD_GJ_L_AMOUNT", 2);
    m_Settings->BA_ITEM_REWARD_PR_W_AMOUNT = pReader->ReadInt("Event", "BA_ITEM_REWARD_PR_W_AMOUNT", 5);
    m_Settings->BA_ITEM_REWARD_PR_L_AMOUNT = pReader->ReadInt("Event", "BA_ITEM_REWARD_PR_L_AMOUNT", 1);

}
#define NPC_SET_AVAILABLE_INTERACTIONS_FUNC_OFFSET						0x004C6350
#define NPC_ADD_AVAILABLE_INTERACTION_FUNC_OFFSET						0x004C3F50

fnCGObjNPCNPC_SetAvailableInteractions pfnOrigCGObjNPCNPC_SetAvailableInteractions =
        reinterpret_cast<fnCGObjNPCNPC_SetAvailableInteractions>(NPC_SET_AVAILABLE_INTERACTIONS_FUNC_OFFSET);

fnCGObjNPCNPC_AddAvailableInteraction pfnOrigCGObjNPCNPC_AddAvailableInteraction =
        reinterpret_cast<fnCGObjNPCNPC_AddAvailableInteraction>(NPC_ADD_AVAILABLE_INTERACTION_FUNC_OFFSET);


bool __fastcall MyCGObjNPCNPC_SetAvailableInteractions(IGObj* pObj, void* /* dummy edx */, int a2, int a3, int a4, int a5, int a6)
{
    bool bOrigResult = pfnOrigCGObjNPCNPC_SetAvailableInteractions(pObj, a2, a3, a4, a5, a6);

    const char* szCodeName128 = pObj->GetCodeName();
    //printf("[%s] NPC CodeName128 = [%s], ptr [0x%p]", __FUNCTION__, szCodeName128, pObj);

    std::list<SCustomNpcInteractionDbRecord> lst = CSqlCon::s_CustomNpcInteractions;

    std::list<SCustomNpcInteractionDbRecord>::iterator it = lst.begin();
    std::list<SCustomNpcInteractionDbRecord>::iterator end = lst.end();

    for (; it != end; it++)
    {
        SCustomNpcInteractionDbRecord record = (SCustomNpcInteractionDbRecord)(*it);
        if (strcmp(record.szCodeName128, szCodeName128) == 0)
        {
            //DWORD res = reinterpret_cast<int(__thiscall*)(IGObj*, int)>(0x4C3F50)(pObj, record.nInteractionID);
            void* res = pfnOrigCGObjNPCNPC_AddAvailableInteraction(pObj, record.nInteractionID);
            BS_INFO("[%s] Added custom interaction mode for npc [%s] interaction id [%d] result 0x%x.", __FUNCTION__,
                   szCodeName128, record.nInteractionID, res);
        }
    }

    return bOrigResult;
}

void CSettings::Patches()
{
    //PK
    DetourTransactionBegin();
    DetourAttach(&(PVOID&)pfnOrigCGObjNPCNPC_SetAvailableInteractions, MyCGObjNPCNPC_SetAvailableInteractions);
    DetourTransactionCommit();

    uint8_t byteValue;
    uint32_t uintValue;

    // Server
    if (ReadMemoryValue<uint8_t>(0x004E52C7 + 2, byteValue))
    {
        uint8_t newValue = m_Settings->SERVER_MAX_LEVEL;
        BS_INFO(" - SERVER_LEVEL_MAX WITH PET (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004E52C7 + 2, newValue); // Character
        WriteMemoryValue<uint8_t>(0x004D641B + 3, newValue); // Pet
        WriteMemoryValue<uint16_t>(0x004E5471 + 4, (newValue - 1) * 4); // Exp bug fix
    }

    if (ReadMemoryValue<uint32_t>(0x0059C5E6 + 1, uintValue))
    {
        uint32_t newValue = m_Settings->CH_MAX_MASTERY_LEVEL;
        BS_INFO(" - RACE_CH_TOTAL_MASTERIES (%u) -> (%u)", uintValue, newValue);
        WriteMemoryValue<uint32_t>(0x0059C5E6 + 1, newValue);
    }


    if (ReadMemoryValue<uint32_t>(0x00B46130, uintValue))
    {
        uint32_t newValue = m_Settings->EU_MAX_MASTERY_LEVEL;
        BS_INFO(" - RACE_EU_TOTAL_MASTERIES (%u) -> (%u)", uintValue, newValue);
        WriteMemoryValue<uint32_t>(0x00B46130, newValue);
    }


    if (ReadMemoryValue<uint8_t>(0x005295DA + 1, byteValue))
    {
        uint8_t newValue = m_Settings->MIN_PK_LEVEL;
        BS_INFO(" - SERVER_PK_LEVEL_REQUIRED (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x005295DA + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x004E6A33 + 1, byteValue))
    {
        uint8_t newValue = m_Settings->MIN_PK_LEVEL_FOR_DROP_ITEM;
        BS_INFO(" - SERVER_PENALTY_DROP_LEVEL_MIN (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004E6A33 + 1, newValue);
    }


    if (ReadMemoryValue<uint8_t>(0x00471B00 + 2, byteValue) && ReadMemoryValue<uint32_t>(0x00471B07 + 1, uintValue))
    {
        unsigned __int64 newValue = 500000000000;
        newValue = newValue & 0xFFFFFFFFFF; // Limit value to 5 bytes
        BS_INFO(" - SERVER_STALL_PRICE_LIMIT (%llu) -> (%llu)", ((unsigned __int64)byteValue << 32) | uintValue, newValue);
        // Stall
        WriteMemoryValue<uint8_t>(0x00471B00 + 2, newValue >> 32);
        WriteMemoryValue<uint32_t>(0x00471B07 + 1, newValue);
        WriteMemoryValue<uint8_t>(0x00472FF5 + 2, newValue >> 32);
        WriteMemoryValue<uint32_t>(0x00473008 + 1, newValue);
        WriteMemoryValue<uint8_t>(0x0047ABD8 + 2, newValue >> 32);
        WriteMemoryValue<uint32_t>(0x0047ABE3 + 1, newValue);
        // Exchange will take the highest UX value
        if (ReadMemoryValue<uint32_t>(0x00480F5E + 4, uintValue))
        {
            if (newValue > 4000000000u)
                newValue = 4000000000u;
            BS_INFO(" - SERVER_EXCHANGE_GOLD_LIMIT (%u) -> (%u)", uintValue, (uint32_t)newValue);
            WriteMemoryValue<uint32_t>(0x00480F5E + 4, newValue);
            WriteMemoryValue<uint32_t>(0x004D8F1A + 2, newValue);
            WriteMemoryValue<uint32_t>(0x004D8F22 + 2, newValue);
            WriteMemoryValue<uint32_t>(0x004F7734 + 2, newValue);
            WriteMemoryValue<uint32_t>(0x004F7746 + 4, newValue);
        }
    }



    if (m_Settings->HIGH_RATES_CONFIG == true)
    {
        BS_INFO(" - FIX_HIGH_RATES_CONFIG -> (%s)", "True");
        WriteMemoryValue<uint8_t>(0x0042714C + 2, 0x42); // ExpRatio
        WriteMemoryValue<uint8_t>(0x004271F5 + 2, 0x42); // ExpRatioParty
        WriteMemoryValue<uint8_t>(0x004272A0 + 2, 0x42); // DropItemRatio
        WriteMemoryValue<uint8_t>(0x00427349 + 2, 0x42); // DropGoldAmountCoef
    }


    if (ReadMemoryValue<uint32_t>(0x004D6F9F, uintValue))
    {
        uint32_t newValue = m_Settings->GRAP_PET_INVENTORY_SIZE;
        BS_INFO(" - GRAB_PET_INVENTORY_SIZE (%d) -> (%d)", uintValue, newValue);
        WriteMemoryValue<uint32_t>(0x004D6F9F, newValue);
    }


    if (m_Settings->FIX_GRAP_PET_PAGE == true)
    {
        BYTE btPayload = 0xEB;
        BS_INFO(" - FIX_GRAP_PET_PAGE -> (True)");
        WriteMemoryValue<uint8_t>(0x004FBD8A, btPayload);
    }



    if (m_Settings->FIX_EXPLOIT_INVISIBLE_INVINCIBLE ==  true)
    {
        BS_INFO(" - FIX_EXPLOIT_INVISIBLE_INVINCIBLE -> (%s)", "True");
        for (int i = 0; i < 2; i++)
            WriteMemoryValue<uint8_t>(0x00515B78 + i, 0x90); // NOP jnz
    }

    if (m_Settings->EXCHANGE_ATTACK_CANCEL)
    {
        BS_INFO(" - FIX_EXCHANGE_ATTACK_CANCEL");
        for (int i = 0; i < 2; i++)
            WriteMemoryValue<uint8_t>(0x00515578 + i, 0x90); // NOP call
    }

    if (m_Settings->GUILD_POINTS)
    {
        BS_INFO(" - FIX_GUILD_POINTS -> (True)");
        // Redirect code flow to DLL
        //placeHook(0x005C4135, addr_from_this(&AsmEdition::OnDonateGuildPoints));
    }

    // Fix
    if (ReadMemoryValue<uint32_t>(0x004744BC + 1, uintValue))
    {
        uint32_t newValue = m_Settings->AGENT_SERVER_CAPACITY;
        BS_INFO(" - FIX_AGENT_SERVER_CAPACITY (%u) -> (%u)", uintValue, newValue);
        WriteMemoryValue<uint32_t>(0x004744BC + 1, newValue);
        WriteMemoryValue<uint32_t>(0x004744C7 + 1, newValue);
    }


    if (ReadMemoryValue<uint8_t>(0x00513FEC + 1, byteValue))
    {
        uint8_t newValue = m_Settings->PARTY_LEVEL_MIN;
        BS_INFO(" - SERVER_PARTY_LEVEL_MIN (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00513FEC + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x00558F20 + 4, byteValue))
    {
        uint8_t newValue = m_Settings->PARTY_MOB_MEMBERS_REQUIRED;
        BS_INFO(" - SERVER_PARTY_MOB_MEMBERS_REQUIRED (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00558F20 + 4, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x005608E2 + 2, byteValue))
    {
        uint8_t newValue = m_Settings->PARTY_MOB_SPAWN_PROBABILITY;
        BS_INFO(" - SERVER_PARTY_MOB_SPAWN_PROBABILITY (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x005608E2 + 2, newValue);
    }

    if (ReadMemoryValue<uint8_t>(0x004E696D + 1, byteValue))
    {
        uint8_t newValue = m_Settings->PENALTY_DROP_PROBABILITY;
        BS_INFO(" - SERVER_PENALTY_DROP_PROBABILITY (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004E696D + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x0051017F + 1, byteValue))
    {
        uint8_t newValue = m_Settings->RESURRECT_SAME_POINT_LEVEL_MAX;
        BS_INFO(" - SERVER_RESURRECT_SAME_POINT_LEVEL_MAX (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0051017F + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x004F36F3 + 1, byteValue))
    {
        uint8_t newValue = m_Settings->NPC_RETURN_DEAD_LEVEL_MAX;
        BS_INFO(" - SERVER_NPC_RETURN_DEAD_LEVEL_MAX (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004F36F3 + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x004E4F0F + 4, byteValue))
    {
        uint8_t newValue = m_Settings->BEGINNER_MARK_LEVEL_MAX;
        BS_INFO(" - SERVER_BEGINNER_MARK_LEVEL_MAX (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004E4F0F + 4, newValue);
        WriteMemoryValue<uint8_t>(0x00518B99 + 3, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x00727784 + 2, byteValue))
    {
        uint8_t newValue = m_Settings->DROP_ITEM_MAGIC_PROBABILITY;
        BS_INFO(" - SERVER_DROP_ITEM_MAGIC_PROBABILITY (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00727784 + 2, newValue);
    }


    // Job
    if (ReadMemoryValue<uint8_t>(0x0060DE69 + 3, byteValue))
    {
        uint8_t newValue = m_Settings->JOB_LEVEL_MAX;
        BS_INFO(" - JOB_LEVEL_MAX (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0060DE69 + 3, newValue);
    }
    if (m_Settings->DISABLE_MOB_SPAWN == true)
    {
        BS_INFO(" - JOB_DISABLE_MOB_SPAWN -> (%s)", "True");
        WriteMemoryValue<uint16_t>(0x0060C4AB, 0xC031); // mov eax,esi -> xor eax,eax
    }
    if (ReadMemoryValue<uint8_t>(0x0051AE71 + 1, byteValue))
    {
        uint8_t newValue = m_Settings->TEMPLE_LEVEL;
        BS_INFO(" - JOB_TEMPLE_LEVEL (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0051AE71 + 1, newValue);
        WriteMemoryValue<uint8_t>(0x0051ABE8 + 1, newValue);
    }

    {
        uint32_t addr = 0x00ADE8DC;
        if (ReadMemoryValue<uint32_t>(addr, uintValue))
        {
            uint32_t newValue = m_Settings->MEMBERS_LIMIT_LEVEL1;
            BS_INFO(" - GUILD_MEMBERS_LIMIT_LEVEL1 (%u) -> (%u)", uintValue, newValue);
            WriteMemoryValue<uint32_t>(addr, newValue);
        }
        if (ReadMemoryValue<uint32_t>(addr + 4, uintValue))
        {
            uint32_t newValue = m_Settings->MEMBERS_LIMIT_LEVEL2;
            BS_INFO(" - GUILD_MEMBERS_LIMIT_LEVEL2 (%u) -> (%u)", uintValue, newValue);
            WriteMemoryValue<uint32_t>(addr + 4, newValue);
        }
        if (ReadMemoryValue<uint32_t>(addr + 8, uintValue))
        {
            uint32_t newValue = m_Settings->MEMBERS_LIMIT_LEVEL3;
            BS_INFO(" - GUILD_MEMBERS_LIMIT_LEVEL3 (%u) -> (%u)", uintValue, newValue);
            WriteMemoryValue<uint32_t>(addr + 8, newValue);
        }
        if (ReadMemoryValue<uint32_t>(addr + 12, uintValue))
        {
            uint32_t newValue = m_Settings->MEMBERS_LIMIT_LEVEL4;
            BS_INFO(" - GUILD_MEMBERS_LIMIT_LEVEL4 (%u) -> (%u)", uintValue, newValue);
            WriteMemoryValue<uint32_t>(addr + 12, newValue);
        }
        if (ReadMemoryValue<uint32_t>(addr + 16, uintValue))
        {
            uint32_t newValue = m_Settings->MEMBERS_LIMIT_LEVEL5;
            BS_INFO(" - GUILD_MEMBERS_LIMIT_LEVEL5 (%u) -> (%u)", uintValue, newValue);
            WriteMemoryValue<uint32_t>(addr + 16, newValue);
        }
    }

    if (ReadMemoryValue<uint32_t>(0x00C6B5F8, uintValue))
    {
        uint32_t newValue = m_Settings->STORAGE_SLOTS_MIN;
        BS_INFO(" - GUILD_STORAGE_SLOTS_MIN (%d) -> (%d)", uintValue, newValue);
        WriteMemoryValue<uint32_t>(0x00C6B5F8, newValue);
        // Get value increased on second level
        uint32_t increaseValue;
        if (ReadMemoryValue<uint32_t>(0x00C6B5F8 + 4, increaseValue))
        {
            uint32_t increaseNewValue = m_Settings->STORAGE_SLOTS_INCREASE;
            BS_INFO(" - GUILD_STORAGE_SLOTS_INCREASE (%d) -> (%d)", increaseValue - uintValue, increaseNewValue);
            for (int i = 0; i < 3; i++)
                WriteMemoryValue<uint32_t>(0x00C6B5F8 + 4 + (i * 4), newValue + (i + 1) * increaseNewValue);
        }
    }
    if (ReadMemoryValue<uint8_t>(0x005B8EA1 + 1, byteValue))
    {
        uint8_t newValue = m_Settings->UNION_LIMIT;
        BS_INFO(" - GUILD_UNION_LIMIT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x005B8EA1 + 1, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x005C4B42 + 4, byteValue))
    {
        uint8_t newValue = m_Settings->UNION_CHAT_PARTICIPANTS;
        BS_INFO(" - GUILD_UNION_CHAT_PARTICIPANTS (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x005C4B42 + 4, newValue);
    }


    if (ReadMemoryValue<uint8_t>(0x004FD02B, byteValue))
    {
        uint8_t newValue = m_Settings->MIN_GUILD_LEVEL_FOR_MERCENARY_SPAWN;
        BS_INFO(" - MIN_GUILD_LEVEL_FOR_MERCENARY_SPAWN (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x004FD02B, newValue);
    }

    if (m_Settings->ALLOW_NON_GM_MERCENARY_SPAWN)
    {
        //short jmp, skip check if summoner is guild master
        BYTE btInstruction = 0xEB;
        WriteMemoryValue(0x004FD045, btInstruction);
    }

    if (m_Settings->DISABLE_GRANT_NAME_CONDITIONS)
    {
        //Short jmp, skip check if guild level is >= 4 and the player is guild master
        BYTE btInstruction = 0xEB;
        WriteMemoryValue<uint8_t>(0x00517C20, btInstruction);
        WriteMemoryValue<uint8_t>(0x005C75AB, btInstruction);

        BS_INFO(" - DISABLE_GULD_NAMES -> (%d)", btInstruction);
    }

    // Alchemy
    if (ReadMemoryValue<uint8_t>(0x0052ADAA + 6, byteValue))
    {
        uint8_t newValue = m_Settings->FUSING_DELAY;
        BS_INFO(" - ALCHEMY_FUSING_DELAY (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0052ADAA + 6, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x00506D92 + 2, byteValue))
    {
        uint8_t newValue = m_Settings->STONE_ASTRAL_VALUE;
        BS_INFO(" - ALCHEMY_STONE_ASTRAL_VALUE (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00506D92 + 2, newValue);
        WriteMemoryValue<uint8_t>(0x00506DD2 + 1, newValue);
    }

    LPCSTR lpszCodename = NULL;
    // Event
    {
        std::string currentValue = ReadMemoryString(0x00646D43);
        if (!currentValue.empty())
        {
            size_t newValueLen = m_Settings->CTF_ITEM_WIN_REWARD.size();
            // Check value it's not empty and shorter than 128 bytes
            if (newValueLen != 0 && newValueLen <= 128)
            {
                BS_INFO(" - EVENT_CTF_ITEM_WIN_REWARD (%s) -> (%s)", currentValue.c_str(), m_Settings->CTF_ITEM_WIN_REWARD.c_str());
                // Set char* pointer to the new value
                //WriteMemoryValue<uint32_t>(0x00646D43, (uint32_t)m_Settings->CTF_ITEM_WIN_REWARD.c_str()); // Winning Reward
                lpszCodename = static_cast<LPCSTR>(m_Settings->CTF_ITEM_WIN_REWARD.c_str());
                MEMUTIL_WRITE_VALUE(LPCSTR, 0x00646D43, lpszCodename);
      
                //WriteMemoryValue<uint32_t>(0x00876935 + 6, (uint32_t)m_Settings->CTF_ITEM_WIN_REWARD.c_str()); // Just in case, something about Quest reward required probably
            }
        }
    }
    if (ReadMemoryValue<uint8_t>(0x00646D41, byteValue))
    {
        uint8_t newValue = m_Settings->CTF_ITEM_WIN_REWARD_AMOUNT;
        BS_INFO(" - EVENT_CTF_ITEM_WIN_REWARD_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00646D40 + 1, newValue);
    }
    {
        std::string currentValue = ReadMemoryString(0x005F19AA);
        if (!currentValue.empty())
        {

            size_t newValueLen = m_Settings->CTF_ITEM_KILL_REWARD.size();
            // Check value it's not empty and shorter than 128 bytes
            if (newValueLen != 0 && newValueLen <= 128)
            {
                BS_INFO(" - EVENT_CTF_ITEM_KILL_REWARD (%s) -> (%s)", currentValue.c_str(), m_Settings->CTF_ITEM_KILL_REWARD.c_str());
                // Set char* pointer to the new value
               // WriteMemoryValue<uint32_t>(0x005F19AA, (uint32_t)m_Settings->CTF_ITEM_KILL_REWARD.c_str()); // Killing Reward
                lpszCodename = static_cast<LPCSTR>(m_Settings->CTF_ITEM_KILL_REWARD.c_str());
                MEMUTIL_WRITE_VALUE(LPCSTR, 0x005F19AA, lpszCodename);

            }
        }
    }
    if (ReadMemoryValue<uint8_t>(0x005F1998, byteValue))
    {
        uint8_t newValue = m_Settings->CTF_ITEM_KILL_REWARD_AMOUNT;
        BS_INFO(" - EVENT_CTF_ITEM_KILL_REWARD_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x005F1998, newValue);
    }
    {
        std::string currentValue = ReadMemoryString(0x006691C6 + 1);
        if (!currentValue.empty())
        {
            size_t newValueLen = m_Settings->BA_ITEM_REWARD.size();
            // Check value it's not empty and shorter than 128 bytes
            if (newValueLen != 0 && newValueLen <= 128)
            {
                BS_INFO(" - EVENT_BA_ITEM_REWARD (%s) -> (%s)", currentValue.c_str(), m_Settings->BA_ITEM_REWARD.c_str());
                // Set char* pointer to the new value
                WriteMemoryValue<uint32_t>(0x006691C6 + 1, (uint32_t)m_Settings->BA_ITEM_REWARD.c_str());
            }
        }
    }
    if (ReadMemoryValue<uint8_t>(0x00669158 + 4, byteValue))
    {
        uint8_t newValue = m_Settings->BA_ITEM_REWARD_GJ_W_AMOUNT;
        BS_INFO(" - EVENT_BA_ITEM_REWARD_GJ_W_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00669158 + 4, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x00669173 + 4, byteValue))
    {
        uint8_t newValue = m_Settings->BA_ITEM_REWARD_GJ_L_AMOUNT;
        BS_INFO(" - EVENT_BA_ITEM_REWARD_GJ_L_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x00669173 + 4, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x0066915F + 4, byteValue))
    {
        uint8_t newValue = m_Settings->BA_ITEM_REWARD_PR_W_AMOUNT;
        BS_INFO(" - EVENT_BA_ITEM_REWARD_PR_W_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0066915F + 4, newValue);
    }
    if (ReadMemoryValue<uint8_t>(0x0066917A + 4, byteValue))
    {
        uint8_t newValue = m_Settings->BA_ITEM_REWARD_PR_L_AMOUNT;
        BS_INFO(" - EVENT_BA_ITEM_REWARD_PR_L_AMOUNT (%d) -> (%d)", byteValue, newValue);
        WriteMemoryValue<uint8_t>(0x0066917A + 4, newValue);
    }


  //tf("---Initialized Settings----\n \n");
}

void CSettings::Initialize()
{
    m_Settings = new DatabaseCfgStruct;
    LoadIniSettings();
    Patches();
    m_Settings->GsID = GetProcessInstanceId();


}

int CSettings::GetProcessInstanceId()
{
    // Check unique process instances using the executable path
    std::string path = GetExecutablePath();
    StringReplaceAll(path, "\\", "/"); // Replace special symbols used on mutex

    // Find available id
    int id = 0;
    if (path == "SR_GameServer") // 1
    {
        id = 1;
        return id;
    }
    else if (path == "SR_GameServer2")
    {
        id = 2;
        return id;
    }
    else if (path == "SR_GameServer3")
    {
        id = 3;
        return id;
    }
    else
    {
        return id;
    }
}

DatabaseCfgStruct* CSettings::GetGameServerConfig()
{
    return m_Settings;
}




std::string acp_encode(const wchar_t* text, size_t length)
{
    if (length == 0) return std::string();
    int size_needed = WideCharToMultiByte(CP_ACP, 0, text, length, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, text, length, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}


std::string acp_encode(const std::wstring& wstr)
{
    return acp_encode(&wstr);
}

std::string acp_encode(const std::wstring* wstr)
{
    return acp_encode(wstr->c_str(), wstr->length());
}


std::wstring acp_decode(const char* str, size_t length)
{
    if (length == 0) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], length, NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, &str[0], length, &wstrTo[0], size_needed);
    return wstrTo;
}


std::wstring acp_decode(const std::string& str)
{
    return acp_decode(&str);
}

std::wstring acp_decode(const std::string* str)
{
    return acp_decode(str->c_str(), str->length());
}
