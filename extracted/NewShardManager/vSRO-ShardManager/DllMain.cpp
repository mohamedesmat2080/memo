#include <Windows.h>
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)
#include "AppManager.h"
#include "Utils/Memory/hook.h"
#include "MainProcess.h"
EXTERN_DLL_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize app
		AppManager::Initialize();
		replaceAddr(0x00783238, addr_from_this(&CMainProcess::_OnProcessMessage));//CGObjMob
		CMainProcess::Initialize();

		//replaceAddr(0x00783238, addr_from_this(&CMainProcess::FUN_0040dda0));
		//vftableHook(0x00799e3c, 12, addr_from_this(&CNetEngine::FUN_0040dda0));

		//replaceOffset(0x006c2b35, addr_from_this(&CNetEngine::FUN_006c4370));
		break;
	}
	return TRUE;
}