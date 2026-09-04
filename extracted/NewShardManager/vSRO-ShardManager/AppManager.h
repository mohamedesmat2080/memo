#pragma once
#include "Database/SQLConnection.h"
#include "Database/SQLCommand.h"
#include <unordered_map>


// All states fetching can generate
enum FETCH_ACTION_STATE {
	UNKNOWN = 0,
	SUCCESS = 1,
	ACTION_UNDEFINED = 2,
	UNNEXPECTED_EXCEPTION = 3,
	PARAMS_NOT_SUPPLIED = 4,
	CHARNAME_NOT_FOUND = 5,
	FUNCTION_ERROR = 6
};

// Handlers required to make a database link
struct DatabaseLink {
	SQLConnection sqlConn;
	SQLCommand sqlCmd;
};

// Application Manager sharing info to any place in the project
class AppManager
{
private: // Private members
	// Check if app has been initialized
	static bool m_IsInitialized;
public: // Public Methods
	// Initialize manager

	// Handlers for SQL communication
	static DatabaseLink m_dbLink, m_dbLinkHelper, m_dbUniqueLog;
	// Flag to keep thread safe
	static bool m_IsRunningDatabaseFetch;
	// Keeps in memory the value assigned

	static DWORD WINAPI DatabaseFetchThread();

	static void LoadLockedItemList();

	static void InitDatabaseFetch();
	static void Initialize();

	static std::unordered_map<INT64, int> m_LockedItems;
private: // Private Helpers
	// Starts console if required
	static void InitDebugConsole();
	// Initialize the default config file if doesn't exists
	static void InitConfigFile();
	// Initialize all hooks required
	static void InitHooks();
	// Set all offsets values
	static void InitPatchValues();
};