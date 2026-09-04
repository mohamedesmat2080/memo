
#include <stdio.h>
#include "Util.h"
#include <memory/hook.h>
#include <memory/MemoryUtility.h>
#include <memory/detours.h>

typedef int (WINAPI* fnMessageBoxA)(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
typedef int (WINAPI* fnMessageBoxW)(HWND hWnd, LPCSTR lpText, LPCWSTR lpCaption, UINT uType);
fnMessageBoxA pfnOrigMessageBoxA = NULL;
fnMessageBoxW pfnOrigMessageBoxW = NULL;


int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    //printf("MessageBoxA %s, [Text] %s\n", lpCaption, lpText);
    printf("Error Dump %s\n", lpText);
    return 1;
}

int WINAPI MyMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    //wprintf(L"MessageBoxW %s, [Text] %s\n", lpCaption, lpText);
    wprintf(L"Error Dump %s\n", lpText);
    return 1;
}


class CGObj;
class CInstance;
typedef const char* (__thiscall* fnGetCharName)(CGObj* pObj);
typedef const char* (__thiscall* fnGetNickName)(CGObj* pObj);

fnGetCharName pfnOrigGetCharName = NULL;
fnGetNickName pfnOrigGetNickName = NULL;

const char* szUnknown = "Unknown";

CInstance* GetGObjInstance(CGObj* pObj)
{
    return MEMUTIL_READ_BY_PTR_OFFSET(pObj, 0x34, CInstance*);
}

const char* __fastcall MyGetCharName(CGObj* pObj, LPVOID /* dummy edx */)
{
    if (pObj == NULL)
        return szUnknown;

    if (GetGObjInstance(pObj) == NULL)
        return szUnknown;

    return pfnOrigGetCharName(pObj);
}

const char* __fastcall MyGetNickName(CGObj* pObj, LPVOID /* dummy edx */)
{
    if (pObj == NULL)
        return szUnknown;

    if (GetGObjInstance(pObj) == NULL)
        return szUnknown;

    return pfnOrigGetNickName(pObj);
}


//Null instance fix
#define GOBJ_GET_CHAR_NAME_FUNC_OFFSET									0x004A66D0
#define GOBJ_GET_NICK_NAME_FUNC_OFFSET									0x004DDC50


extern "C" _declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {

        HMODULE hUser32 = GetModuleHandleA("User32.dll");
        pfnOrigMessageBoxA = reinterpret_cast<fnMessageBoxA>(GetProcAddress(hUser32, "MessageBoxA"));
        pfnOrigMessageBoxW = reinterpret_cast<fnMessageBoxW>(GetProcAddress(hUser32, "MessageBoxW"));

        DetourTransactionBegin();
        DetourAttach(&(PVOID&)pfnOrigMessageBoxA, MyMessageBoxA);
        DetourAttach(&(PVOID&)pfnOrigMessageBoxW, MyMessageBoxW);
        DetourTransactionCommit();


        pfnOrigGetCharName = reinterpret_cast<fnGetCharName>(GOBJ_GET_CHAR_NAME_FUNC_OFFSET);
        pfnOrigGetNickName = reinterpret_cast<fnGetNickName>(GOBJ_GET_NICK_NAME_FUNC_OFFSET);

        //DetourTransactionBegin();
        //DetourAttach(&(PVOID&)pfnOrigGetCharName, MyGetCharName);
        //DetourAttach(&(PVOID&)pfnOrigGetNickName, MyGetNickName);
        //DetourTransactionCommit();

        Init();
    }

    return TRUE;
}
