#include <IFChatViewer.h>
#include <IFMainPopup.h>
#include <IFChatOptionBoard.h>
#include <IFWholeChat.h>
#include <CustomData/CustomSettingManager.h>
#include <CustomData/CustomDataManager.h>
#include <CustomData/CustomCICPlayer.h>
#include <SecondBar/IFExtQuickSlotSecond.h>
#include <SecondBar/IFExtQuickSlotSecondOption.h>
#include <SecondBar/IFExtQuickSlotCustomOption.h>
#include <SecondBar/IFExtQuickSlotCustom.h>
#include "IFExtQuickSlot.h"
#include "SRIFLib/NIFUnderMenuBar.h"

#include "hooks/Hooks.h"
#include "Util.h"
#include "imgui_windows/ImGui_Windows.h"

#include "IFMenuGuide.h"
#include "Menu/IFMenu.h"
#include "Menu/IFGrantName.h"
#include "support/hook.h"
#include "Menu/IFTitleManager.h"
#include "Menu/IFTitleManagerSlot.h"
#include "Menu/IFIconManager.h"
#include "Menu/IFIconManagerSlot.h"
#include "Menu/IFDynamicRanking.h"
#include "Menu/IFDynamicRankingSlot.h"

#include "Menu/IFUniqueHistory.h"
#include "Menu/IFUniqueHistorySlot.h"

#include "Menu/IFEventRegister.h"
#include "Menu/IFEventSchedule.h"

#include "Menu/IFAchievements.h"
#include "Menu/IFAchievementsSlot.h"
#include "Menu/IFChangelog.h"

#include "Guides/IFChest.h"
#include "Guides/IFChestGuide.h"
#include "Guides/IFChestSlot.h"

#include "CustomInterface/IFCustomMessageBox.h"
#include "CustomInterface/IFDps.h"
#include "CustomInterface/IFPartyMemberViewer.h"
#include "CustomInterface/IFSavedLocation.h"
#include "CustomInterface/IFMovePartyMember.h"


#include "Social/IFFacebookGuide.h"
#include "Social/IFSocial.h"
#include <NewItemMall/IFVItemMall.h>
#include <NewItemMall/IFVSelectMall.h>
#include <NewItemMall/IFVItemMallBuyItem.h>
#include <NewItemMall/IFVAvatarMall.h>
#include <NewItemMall/IFVAvatarMallBuyItemList.h>
#include <NewItemMall/IFVAvatarMallBuyItemListSlot.h>
#include <DailyLogin/IFDailyLogin.h>
#include <DailyLogin/IFDailyLoginGuide.h>
#include <CustomInterface/IFPopupList2.h>
#include <Macro/IFMacroSCSlot.h>
#include <Macro/IFMacroMenuAutoScrollSlot.h>
#include <MacroAlchemy/IFMacroAlchemySlot.h>
#include <MacroAlchemy/IFAlchemyMacroGuide.h>
#include <MacroAlchemy/IFAlchemyMacro.h>
#include <GlobalItemLinking/GlobalItemLinking.h>
#include <GlobalItemLinking/MemoryHelper.h>
#include <CustomInterface/IFKillCounter.h>
#include <MacroAlchemy/IFAlchemyMacroBlueSlot.h>

#include <LockItems/IFNewMsgBox.h>
#include <LockItems/IFItemLocker.h>
#include <LockItems/IFItemLockSlot.h>
#include <LockItems/IFItemUnlocker.h>
#include <LockItems/IFItemUnlockSlot.h>
#include <CustomInterface/IFFortressWar.h>
#include <CustomInterface/IFCustomEmojiList.h>
#include <IFTargetWindowPlayer.h>
#include <ExtraUI/IFItemTranslationWnd.h>
#include <ExtraUI/IFItemTranslationSlot.h>
#include <SecondPW/IFSecondaryPassword.h>
#include <IFTargetWindowJobPlayer.h>
#include <DailyLogin/IFAttendance.h>
#include <ExtraUI/IFTargetPlayerEquip.h>
#include <ExtraUI/IFSettings.h>
#include <ExtraUI/IFMSFPS.h>
#include <ExtraUI/IFCounterWnd.h>


#include "Web/IFWeb.h"
#include "Web/IFWebGuide.h"
#include "CustomInterface/IFSoxEffect.h"
#include "Macro/IFMacroGuide.h"
#include "Macro/IFMacro.h"
#include "Macro/IFMacroMenu.h"
#include "Macro/IFMacroMenuAutoPotion.h"
#include "Macro/IFMacroSlot.h"
#include "Macro/IFMacroSlotWep.h"
#include "Macro/IFMacroMenuAutoSkill.h"
#include "Macro/IFMacroSlotSkill.h"
#include "Macro/IFMacroMenuAutoHunt.h"
#include "CustomInterface/IFTeamCounter.h"
#include "CustomInterface/IFJobCounter.h"

extern "C" _declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hModule, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {

        // Setup all the required hooks
        Setup();
        m_Settings->getInstance();
        m_CustomDataManager->getInstance();
        m_Player->getInstance();

        g_MemoryHelper = new MemoryHelper();
        g_global = new GlobalItemLinking();
//        g_global->Initialize();

        // Inject all the new objects
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMenu));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMenuGuide));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFGrantName));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFTitleManagerSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFTitleManager));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFIconManagerSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFIconManager));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFDynamicRankingSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFDynamicRanking));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFUniqueHistorySlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFUniqueHistory));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFEventRegisterSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFEventRegister));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFEventScheduleSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFEventSchedule));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFAchievements));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFAchievementsSlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFChangelog));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFCustomMessageBox));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFExtQuickSlotSecond));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFExtQuickSlotSecondOption));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFExtQuickSlotCustom));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFExtQuickSlotCustomOption));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFChestGuide));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFChest));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFChestSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFFacebookGuide));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFSocial));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFDps));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFPartyMemberViewer));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFSavedLocation));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMovePartyMember));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFVItemMallSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVItemMallSlotNews));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVItemMall));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVSelectMall));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVItemMallBuyItem));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVAvatarMall));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVAvatarMallBuyItemListSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVAvatarMallBuyItemList));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFVItemMallFavList));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFWebGuide));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFWeb));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFDailyLogin));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFDailyLoginGuide));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFSoxEffect));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroGuide));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacro));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenu));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoPotion));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoSkill));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroSlotWep));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroSlotSkill));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoHunt));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuPickFilter));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFPopupList));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFPopupList2));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoSkillPartySlot));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoScrollSlot));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroSCSlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroAlchemySlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFAlchemyMacroGuide));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFAlchemyMacro));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFKillCounter));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFTeamCounter));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFJobCounter));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFAlchemyMacroBlueSlot));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFMacroMenuAutoHuntPartySlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFNewMsgBox));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemLocker));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemLockSlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemUnlocker));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemUnlockSlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFFortressWar));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFCustomEmojiList));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFCustomEmojiListSlot));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemTranslationSlot));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFItemTranslationWnd));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFSecondaryPassword));


        RegisterObject(&GFX_RUNTIME_CLASS(CIFAttendance));

        RegisterObject(&GFX_RUNTIME_CLASS(CIFTargetPlayerEquip));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFSettings));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFMSFPS));
        RegisterObject(&GFX_RUNTIME_CLASS(CIFCounterWnd));

        OverrideObject<CIFTargetWindowPlayer, 0x00eea5dc>();

        // Override existing objects
        OverrideObject<CIFMainPopup, 0x00eea6dc>();
        OverrideObject<CIFPlayerInfo, 0x00eea7e8>();
        OverrideObject<CIFTargetWindowJobPlayer, 0x00eea5bc>();
        OverrideObject<CIFWholeChat, 0x00eec7a8>();
#ifdef CONFIG_EXT_QUICK_SLOT
        OverrideObject<CIFExtQuickSlot, 0x00ee9a28>();
        OverrideObject<CIFExtQuickSlotOption, 0x00ee9a48>();
#endif // CONFIG_EXT_QUICK_SLOT

        OverrideObject<CIFChatOptionBoard, 0x00eec128>();
        OverrideObject<CIFChatViewer, 0x00EEC168>();
#ifdef CONFIG_OLD_MAINPOPUP
        OverrideObject<CIFMainPopup, 0x00eea6dc>();
#endif // CONFIG_OLD_MAINPOPUP

#ifdef CONFIG_CHATVIEWER
        OverrideObject<CIFChatViewer, 0x00EEC168>();
#endif // CONFIG_CHATVIEWER

#ifdef CONFIG_WHOLE_CHAT

#endif // CONFIG_WHOLE_CHAT

#ifdef CONFIG_MAINPOP_PLAYERINFO
        OverrideObject<CIFPlayerInfo, 0x00eea7e8>();
#endif // CONFIG_MAINPOP_PLAYERINFO

#ifdef CONFIG_IMGUI
       /* OnCreate(ImGui_OnCreate);
        OnEndScene(ImGui_OnEndScene);
        OnWndProc(ImGui_WndProc);

        OnPreSetSize(ImGui_OnPreSetSize);
        OnPostSetSize(ImGui_OnPostSetSize);*/
#endif // CONFIG_IMGUI

#ifdef CONFIG_OLD_UNDERBAR
        // https://www.elitepvpers.com/forum/sro-pserver-guides-releases/4256375-source-fix-old-exp-bar-writing-code.html
        replaceAddr(0x00D9841C, addr_from_this(&CNIFUnderMenuBar::Update));
#endif // CONFIG_OLD_UNDERBAR

        OnPreInitGameAssets(InstallRuntimeClasses);
    }

    return TRUE;
}
        