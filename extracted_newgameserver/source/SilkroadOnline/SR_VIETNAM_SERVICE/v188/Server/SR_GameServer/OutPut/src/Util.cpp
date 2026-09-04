//
// Created by Kurama on 12/19/2022.
//

#include <Objects/GObjPC.h>
#include <Objects/GObjMob.h>
#include <Settings.h>
#include <Objects/GObjSiegeStruct.h>
#include <SqlConnection/sqlCon.h>
#include <RegionRestrictionMgr.h>
#include "Util.h"

#include "memory/hook.h"

#include "BSObj/BSObj.h"

#include "MainProcess.h"
#include "../../GameServer/CustomNetwork/MsgStreamBuffer.h"
#include <memory/MemoryUtility.h>
#include <NetHelper.h>
#include <Objects/CustomTimedJobManager.h>

#define GREEN_BOOK_NOP_OFFSET_1											0x004142E2
#define GREEN_BOOK_NOP_OFFSET_2											0x0041474D

#define QUEST_RAISE_EVENT_DISABLE_MSG_OFFSET							0x005805C8

void SendRestartRequest(int p1, CGObjPC * p2) {
    reinterpret_cast<void(__stdcall*)(int, CGObjPC*)>(0x004e2830)(p1, p2);
}

void Init() {
#ifdef CONFIG_DEBUG_CONSOLE
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);
#endif // CONFIG_DEBUG_CONSOLE

    BS_INFO("JTGuard, SR_GameServer!");
//    BS_INFO("******************************************************************************")

    RenderNop((void*)GREEN_BOOK_NOP_OFFSET_1, 8);
    RenderNop((void*)GREEN_BOOK_NOP_OFFSET_2, 5);
    RenderNop((void*)QUEST_RAISE_EVENT_DISABLE_MSG_OFFSET, 19);

    CSettings::Initialize();
    CGObjPC::Setup();
    CSqlCon::Initialize();
    CRegionRestrictionMgr::Initialize();
    CRegionAttackRestrictionsMgr::Initialize();

    CNetHelper::Initialize();

    vftableHook(0x00af59fc, 376, addr_from_this(&CGObjPC::ReaderPacket));

    vftableHook(0x00adeffc, 9, addr_from_this(&CMainProcess::_OnProcessMessage));
    replaceAddr(0x00aee3e8, addr_from_this(&CGObjMob::MobDamageMeter));//CGObjMob

    //replaceAddr(0x00aee2a0, addr_from_this(&CGObjMob::LivedpsMobAttackRecorder));//CGObjMob

    vftableHook(0x00af59fc, 324, addr_from_this(&CGObjPC::KillLoggerFunction));

    //vftableHook(0x00af59fc, 92, addr_from_this(&CGObjPC::GiveExp));

    //vftableHook(0x00af59fc, 293, addr_from_this(&CGObjPC::GroupSpawn));

    //vftableHook(0x00af59fc, 91, addr_from_this(&CGObjPC::Func_91));




    //vftableHook(0x00af59fc, 386, addr_from_this(&CGObjPC::Func_386));


 

    //replaceOffset(0x0050f941, addr_from_this(&CGObjPC::FuncWhat));


  

    //replaceAddr(0x00af5eec, addr_from_this(&CGObjPC::Fun_004a9b70));

    vftableHook(0x00aeddac, 324, addr_from_this(&CGObjMob::Mobkillquest));


    //vftableHook(0x00aeddac, 294, addr_from_this(&CGObjPC::FUN_004e5a10));

  /*  RenderNop((void*)0x00524a55, 3);
    RenderNop((void*)0x00524a58, 6);
*/

    //replaceAddr(0x00aec794, addr_from_this(&CGStorage::ApplySetItemOptLevel2ThisGroup));

 //   replaceOffset(0x005238fc, addr_from_this(&CGStorage::Testx));
    //replaceOffset(0x00506dad, addr_from_this(&CEnchant_Reinforce::AddPlus));
    //replaceOffset(0x00506dd8, addr_from_this(&CEnchant_Reinforce::AddPlus));
    //replaceOffset(0x00506de3, addr_from_this(&CEnchant_Reinforce::AddPlus));


   /* vftableHook(0x00af59fc, 322, addr_from_this(&CGObjPC::FuncJobExp));*/


   
    vftableHook(0x00af59fc, 391, addr_from_this(&CGObjPC::Func_3911)); /// spawn
    //vftableHook(0x00af59fc, 373, addr_from_this(&CGObjPC::Func_373)); /// spawn
    CCustomTimedJobManager::CreateConsumeThread();



    replaceAddr(0x00af0b4c, addr_from_this(&CGObjSiegeStruct::SiegeStructDamageMeter));

    //391 is  spawn
    replaceAddr(0x00af2594, addr_from_this(&CGObjCOS_GoldPet::FUN_004d2ad0));

}