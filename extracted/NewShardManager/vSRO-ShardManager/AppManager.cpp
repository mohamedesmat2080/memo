#include "AppManager.h"
// Console stuffs
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
// Thread signal
#include <csignal>
#include <sstream>
#include <exception>
// Utils
#include "Utils/IO/SimpleIni.h"
#include "Utils/Memory/Process.h"
#include "Utils/Memory/hook.h"
#pragma warning(disable:4244) // Bitwise operations warnings
// ASM injection
#include "AsmEdition.h"
#include "ShardNetManager.h"


/// Static stuffs
bool AppManager::m_IsInitialized;
DatabaseLink AppManager::m_dbLink, AppManager::m_dbLinkHelper, AppManager::m_dbUniqueLog;
bool AppManager::m_IsRunningDatabaseFetch;

void AppManager::Initialize()
{
	if (!m_IsInitialized)
	{
		m_IsInitialized = true;
		InitConfigFile();
		InitDebugConsole();
		InitPatchValues();
		InitHooks();
		CShardNetManager::Setup();
		InitDatabaseFetch();
	}
}
void AppManager::InitConfigFile()
{
	CSimpleIniA ini;
	// Try to load it or create a new one
	if (ini.LoadFile("JTShardCfg.ini") != SI_Error::SI_OK)
	{
		ini.SetSpaces(false);
		// Memory
		ini.SetLongValue("Settings", "CHARACTERS_MAX", 4, "; Maximum characters per account");
		ini.SetLongValue("Settings", "UNION_LIMIT", 8, "; Union participants limit");
		ini.SetLongValue("Settings", "CTF_PARTICIPANS_MIN", 8, "; Minimum participants required to start Capture The Flag");
		ini.SetLongValue("Settings", "BA_PARTICIPANS_MIN", 8, "; Minimum participants required to start Battle Arena");
		ini.SetBoolValue("Settings", "PARTY_MATCH_1HOUR_DC", true, "; Fix disconnect when party takes more than 1 hour on party match");
		ini.SetBoolValue("Settings", "GUILD_POINTS", true, "; Prevents negative values on guild points");
		// App
		ini.SetBoolValue("Settings", "DEBUG_CONSOLE", true, "; Attach debug console");
		// Save it
		ini.SaveFile("JTShardCfg.ini");
	}
}
void AppManager::InitDebugConsole()
{
	// Load file
	CSimpleIniA ini;
	ini.LoadFile("JTShardCfg.ini");

	// Check if console has been deactivated
	if (ini.GetBoolValue("Settings", "DEBUG_CONSOLE", true))
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		freopen("CONIN$", "r", stdin);
	}
}
void AppManager::InitHooks()
{
	std::cout << " * Initializing hooks..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("JTShardCfg.ini");

	// Fix
	if (ini.GetBoolValue("Settings", "GUILD_POINTS", true))
	{
		printf(" - FIX_GUILD_POINTS\r\n");
		// Redirect code flow to DLL
		if (placeHook(0x004364EE, addr_from_this(&AsmEdition::OnDonateGuildPoints)))
		{
			std::cout << "   - OnDonateGuildPoints" << std::endl;
		}
		if (placeHook(0x00438B68, addr_from_this(&AsmEdition::OnDonateGuildPointsErrorCode)))
		{
			std::cout << "   - OnDonateGuildPointsErrorCode" << std::endl;
		}
		if (placeHook(0x0043A9F6, addr_from_this(&AsmEdition::OnDonateGuildPointsErrorMsg)))
		{
			std::cout << "   - OnDonateGuildPointsErrorMsg" << std::endl;
		}
	}
}
void AppManager::InitPatchValues()
{
	std::cout << " * Initializing patches..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("JTShardCfg.ini");

	// buffers
	uint8_t byteValue;

	// Account
	if (ReadMemoryValue<uint8_t>(0x0040F47C + 2, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Settings", "CHARACTERS_MAX", 4);
		printf(" - ACCOUNT_CHARACTERS_MAX (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0040F47C + 2, newValue);
		WriteMemoryValue<uint8_t>(0x00429B83 + 2, newValue);
	}

	// Guild
	if (ReadMemoryValue<uint8_t>(0x00434311 + 1, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Settings", "UNION_LIMIT", 8);
		printf(" - GUILD_UNION_LIMIT (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00434311 + 1, newValue);
	}

	// Event
	if (ReadMemoryValue<uint8_t>(0x00672891 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Settings", "CTF_PARTICIPANS_MIN", 8);
		printf(" - EVENT_CTF_PARTICIPANS_MIN (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x00672891 + 4, newValue);
	}
	if (ReadMemoryValue<uint8_t>(0x0066A1B0 + 4, byteValue))
	{
		uint8_t newValue = ini.GetLongValue("Settings", "BA_PARTICIPANS_MIN", 8);
		printf(" - EVENT_BA_PARTICIPANS_MIN (%d) -> (%d)\r\n", byteValue, newValue);
		WriteMemoryValue<uint8_t>(0x0066A1B0 + 4, newValue);
	}

	// Fix
	if (ini.GetBoolValue("Settings", "PARTY_MATCH_1HOUR_DC", true))
	{
		printf(" - FIX_PARTY_MATCH_1HOUR_DC\r\n");
		// Disable/Jump timeout condition
		WriteMemoryValue<uint16_t>(0x0045055C, 0x30EB); // jmp,+30
		for(int i = 0; i < 3; i++)
			WriteMemoryValue<uint8_t>(0x0045055C + 2 + i, 0x90); // nop
	}
}


void AppManager::InitDatabaseFetch()
{
	std::cout << " * Initializing database connection to execute actions..." << std::endl;

	// Load file
	CSimpleIniA ini;
	ini.LoadFile("JTShardCfg.ini");

	// Create connection string
	std::wstringstream connString;
	connString << ini.GetValue("SqlConnection", "ConnectionString", "SERVER=DESKTOP-M7FGFKQ;DSN=LexaShield_User;UID=sa;PWD=1234;DATABASE=LexaShield_User");

	if (m_dbLink.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLink.sqlCmd.Open(m_dbLink.sqlConn)
		&& m_dbLinkHelper.sqlConn.Open((SQLWCHAR*)connString.str().c_str()) && m_dbLinkHelper.sqlCmd.Open(m_dbLinkHelper.sqlConn))
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AppManager::DatabaseFetchThread, 0, 0, 0);
	}
}
DWORD WINAPI AppManager::DatabaseFetchThread()
{
	// Load file
	CSimpleIniA ini;
	ini.LoadFile("JTShardCfg.ini");

	
	// Show a message about table to be fetch
	std::cout << " - Waiting 15 second before start fetching on \"" << "JTGuard " << "\"..." << std::endl;
	Sleep(15000);

	//AppManager::LoadLockedItemList();


	std::cout << " - Fetching started!" << std::endl;
	m_IsRunningDatabaseFetch = true;

	m_dbLink.sqlCmd.Clear();

	// Stops this thread loop on interruption/exit
	signal(SIGINT, [](int) {
		m_IsRunningDatabaseFetch = false;
		});

	// Start fetching actions without result
	std::wstringstream qSelectActions;
	qSelectActions << "SELECT ID, Action_ID, Data1, Data2, Data3, Data4, Data5, Data6, Data7, Data8, Data9, Data10, Data11, PlannedTime FROM _AsyncGameServerCommands";
	while (m_IsRunningDatabaseFetch)
	{
		// Try to execute query
		if (!m_dbLink.sqlCmd.ExecuteQuery((SQLWCHAR*)qSelectActions.str().c_str()))
			break;

		// Fetch one by one
		while (m_dbLink.sqlCmd.FetchData())
		{
			// Set default state
			FETCH_ACTION_STATE actionResult = FETCH_ACTION_STATE::SUCCESS;

			// Read required params
			SQLINTEGER cID, cActionID;
			SQLINTEGER CharID, MobID, GameWorldID, RegionId, PosX, PosY, PosZ, GenerateRadius;

			m_dbLink.sqlCmd.GetData(1, SQL_C_ULONG, &cID, 0, NULL);
			m_dbLink.sqlCmd.GetData(2, SQL_C_ULONG, &cActionID, 0, NULL);
			// Try to execute the action
			try {
				switch (cActionID)
				{
				case 1: // grantname
				{
					// Read & check params
					char GrantNameData1[128];
					SQLUINTEGER cParam02;
					SQLINTEGER cParam03;
					SQLUSMALLINT cParam04;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &GrantNameData1, 128, 0))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(1);
						*pMsg << CharID;
						pMsg->WriteStringA(GrantNameData1);
						

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 2:
				{
					// Read & check params
				
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &MobID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &GameWorldID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &RegionId, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_LONG, &PosX, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_LONG, &PosY, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_LONG, &PosZ, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(9, SQL_C_LONG, &GenerateRadius, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(2);
						*pMsg << MobID << GameWorldID << RegionId << PosX << PosY << PosZ << GenerateRadius;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				}
				break;
				case 3: // spawn mob by nearbymob
				{
					// Read & check params
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) && 
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &MobID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(3);
						*pMsg << (int)CharID << (int)MobID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 4: // remove monster
				{
					// Read & check params
					SQLUINTEGER MobID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &MobID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(4);
						*pMsg << MobID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 5: // remove monster by worldid
				{
					// Read & check params
					SQLUINTEGER WorldID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &WorldID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(5);
						*pMsg << WorldID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;

				case 6: // send live skill
				{
					// Read & check params
					SQLUINTEGER SkillID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &SkillID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(6);
						*pMsg << CharID << SkillID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 7: // remove live skill
				{
					// Read & check params
					SQLUINTEGER SkillID2;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &SkillID2, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(7);
						*pMsg << CharID << SkillID2;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 8: // remove live skill
				{
					// Read & check params
					char SkillCodeName[128];
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_CHAR, &SkillCodeName, 128, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(8);
						*pMsg << CharID << SkillCodeName;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;

				case 10: // TO TOWN
				{
					// Read & check params
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						)
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(10);
						*pMsg << CharID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 11: // TO TOWN
				{
					// Read & check params
					SQLUINTEGER WorldID2;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &WorldID2, 0, NULL)
						)
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(11);
						*pMsg << WorldID2;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 12: // TO TOWN
				{
					// Read & check params
					SQLUINTEGER PetUniqueID;
					SQLUINTEGER PetSkillID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &PetUniqueID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &PetSkillID, 0, NULL)
						)
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(12);
						*pMsg << PetUniqueID << PetSkillID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 13: // cape
				{
					// Read & check params
					SQLUINTEGER CapeID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &CapeID, 0, NULL)
						)
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(13);
						*pMsg << CharID << CapeID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 14: // cape
				{
					SQLINTEGER WorldID3, RegionIds, PosXx, PosYy, PosZz;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &WorldID3, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &RegionIds, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_LONG, &PosXx, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_LONG, &PosYy, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(8, SQL_C_LONG, &PosZz, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(14);
						*pMsg << CharID << WorldID3 << RegionIds << PosXx << PosYy << PosZz;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 15: // cape
				{
					// Read & check params
					SQLUINTEGER CapeID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(15);
						*pMsg << CharID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 16: // exp
				{
					// Read & check params
					SQLUINTEGER ExpRate;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) 
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &ExpRate, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(16);
						*pMsg << CharID << (unsigned int)ExpRate;
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;

				case 17: // cape
				{
					// Read & check params
					char MutateItemName[128];
					SQLINTEGER Slot;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &Slot, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_CHAR, &MutateItemName, 128, 0))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(17);
						*pMsg << CharID;
						*pMsg << Slot;
						pMsg->WriteStringA(MutateItemName);
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;

				case 18: // cape
				{
					SQLINTEGER Slot2;
					SQLINTEGER Amount;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &Slot2, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &Amount, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(18);
						*pMsg << CharID;
						*pMsg << Slot2;
						*pMsg << Amount;
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;

				case 19: // consume and switch item
				{
					char MutateItemName[128];
					SQLINTEGER MutateSlot;
					SQLINTEGER ConsumeSlot;
					SQLINTEGER ConsumeAmount;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &MutateSlot, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_CHAR, &MutateItemName, 128, 0)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_LONG, &ConsumeSlot, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_LONG, &ConsumeAmount, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(19);
						*pMsg << CharID;
						*pMsg << MutateSlot;
						pMsg->WriteStringA(MutateItemName);
						*pMsg << ConsumeSlot;
						*pMsg << ConsumeAmount;
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;

				} break;

				case 20: // live silk
				{	
					SQLINTEGER nSilk;
					SQLINTEGER nSilkGift;
					SQLINTEGER nSilkPoint;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &nSilk, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &nSilkGift, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_LONG, &nSilkPoint, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(20);
						*pMsg << CharID;
						*pMsg << nSilk;
						*pMsg << nSilkGift;
						*pMsg << nSilkPoint;
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;

				} break;
				case 21: // live gold
				{
					SQLBIGINT nGold;
					SQLINTEGER AddOrRemove;

					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_SBIGINT, &nGold, 0, NULL) && // SQL_C_SBIGINT kullanýldý
						m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &AddOrRemove, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(21);
						*pMsg << CharID;
						*pMsg << nGold;
						*pMsg << AddOrRemove;
						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}

					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;

				} break;
				case 22: // TO TOWN
				{
					// Read & check params
					SQLUINTEGER WorldID3;
					SQLUINTEGER LayerID;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &WorldID3, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &LayerID, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(22);
						*pMsg << WorldID3;
						*pMsg << LayerID;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;
				} break;
				case 131: // live silk
				{
					SQLINTEGER nItemID;
					SQLINTEGER nPlus;
					SQLINTEGER nSlot;
					SQLINTEGER nAdvLevel;
					if (m_dbLink.sqlCmd.GetData(3, SQL_C_LONG, &CharID, 0, NULL) &&
						m_dbLink.sqlCmd.GetData(4, SQL_C_LONG, &nItemID, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(5, SQL_C_LONG, &nPlus, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(6, SQL_C_LONG, &nSlot, 0, NULL)
						&& m_dbLink.sqlCmd.GetData(7, SQL_C_LONG, &nAdvLevel, 0, NULL))
					{
						CMsgStreamBuffer* pMsg = CShardNetManager::AllocMsgForGS();
						pMsg->SetMsgID(0x8888);

						*pMsg << BYTE(131);
						*pMsg << CharID;
						*pMsg << nItemID;
						*pMsg << (byte)nPlus;
						*pMsg << (byte)nSlot;
						*pMsg << (byte)nAdvLevel;

						CShardNetManager::BroadcastMsgToGameServers(pMsg);
					}
					else
						actionResult = FETCH_ACTION_STATE::PARAMS_NOT_SUPPLIED;

				} break;
				default:
					std::cout << " Error on Action_ID (" << cActionID << ") : Undefined" << std::endl;
					actionResult = FETCH_ACTION_STATE::ACTION_UNDEFINED;
					break;
				}
			}
			catch (std::exception& ex)
			{
				std::cout << " Exception on Action_ID (" << cActionID << ") : " << ex.what() << std::endl;
				actionResult = FETCH_ACTION_STATE::UNNEXPECTED_EXCEPTION;
			}

			// Update action result from table by row id
			std::wstringstream qUpdateResult;
			qUpdateResult << "DELETE FROM _AsyncGameServerCommands where ID = " << cID;
			m_dbLinkHelper.sqlCmd.ExecuteQuery((SQLWCHAR*)qUpdateResult.str().c_str());
			m_dbLinkHelper.sqlCmd.Clear();
		}
		m_dbLink.sqlCmd.Clear();

		// Making like 10 querys per second
		Sleep(1000);
	}

	// Close connection and dispose handlers
	m_dbLinkHelper.sqlConn.Close();
	m_dbLink.sqlConn.Close();

	// Stop flag
	m_IsRunningDatabaseFetch = false;
	std::cout << " - Fetching stopped!" << std::endl;

	return 0;
}

std::unordered_map<INT64, int> AppManager::m_LockedItems;

void AppManager::LoadLockedItemList()
{
	std::wstringstream qSelectActions;
	qSelectActions << L"SELECT ItemID64, Password FROM _LockedItemList";

	INT64 ItemID64 = 0;
	int Password = 0;

	if (AppManager::m_dbLink.sqlConn.IsOpen())
	{
		SQLHSTMT hStmt = AppManager::m_dbLink.sqlCmd.GetStmtHandle();

		// Prepare and execute the query
		if (SQLPrepare(hStmt, (SQLWCHAR*)qSelectActions.str().c_str(), SQL_NTS) == SQL_SUCCESS)
		{
			if (SQLExecute(hStmt) == SQL_SUCCESS)
			{
				SQLRETURN retcode;

				// Bind the columns
				SQLBindCol(hStmt, 1, SQL_C_SBIGINT, &ItemID64, sizeof(ItemID64), NULL);
				SQLBindCol(hStmt, 2, SQL_C_LONG, &Password, sizeof(Password), NULL);

				// Fetch and store the results
				while ((retcode = SQLFetch(hStmt)) != SQL_NO_DATA)
				{
					m_LockedItems[ItemID64] = Password;
				}

				// Cleanup
				SQLCloseCursor(hStmt);
			}
			else
			{
				// Sorgu baþarýsýz oldu
				std::wcout << L"Query execution failed." << std::endl;
			}
		}
		else
		{
			// Sorgu hazýrlama baþarýsýz oldu
			std::wcout << L"Query preparation failed." << std::endl;
		}

		// Release the statement handle
		AppManager::m_dbLink.sqlCmd.ReleaseStmtHandle(hStmt);
	}
	else
	{
		// Veritabaný baðlantýsý açýk deðil
		std::wcout << L"Database connection is not open." << std::endl;
	}
}

