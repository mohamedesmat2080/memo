//
// Created by YUMBUL on 19.03.2024.
//

#include <BSLib/multibyte.h>
#include <CustomData/CustomSettingManager.h>
#include <SRIFLib/NIFUnderMenuBar.h>
#include <CustomData/CustomDataManager.h>
#include <SRIFLib/NInterfaceResource.h>
#include "PSTitle.h"
#include "Game.h"
#include "TextStringManager.h"
#include "PSCharacterSelect.h"
#include "ICPlayer.h"
#include "IFOListCtrl.h"
#include "GInterface.h"
#include "IFSimpleListCtrl.h"
#include "IFStateSlot.h"
#include "IDamageText.h"
#include "IFCOSCommand.h"
#include <support/hook.h>
#include <support/Process.h>
#include <SRIFLib/NIFEnchantWnd.h>
#include <CustomData/CustomCICPlayer.h>
#include <support/MemberFunctionHook.h>

#include <iostream>
#include <ctime>

#include <stdexcept>
#include <iostream>
#include <time.h>  // time(NULL) için gerekli
#include <SecondPW/IFSecondaryPassword.h>
#include <Hwid/HWIDGenerator.h>
#include <DiscordRichPresence/DiscordManager.h>

bool CPSTitle::OnCreateIMPL(long ln) {
    bool a = reinterpret_cast<bool(__thiscall *)(CPSTitle *, long)>(0x0086b190)(this, ln);
    if(m_Settings->NewIDPWScreen)
    {

        this->m_IRM.GetResObj(1926, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1927, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1928, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1929, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1930, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1931, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(1930, 1)->SetText(L"Location");
        this->m_IRM.GetResObj(104, 1)->SetText(L"CHANNEL");

        wnd_pos servernamelist = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->GetPos();
        wnd_size servernamelistsize = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->GetSize();
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->MoveGWnd(servernamelist.x + 22, servernamelist.y);
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->SetGWndSize(servernamelistsize.width - 22, servernamelistsize.height - 14);

        wnd_pos servernamelistx = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->GetPos();
        wnd_size servernamelistsizex = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->GetSize();
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->MoveGWnd(servernamelistx.x + 38, servernamelistx.y);
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->SetGWndSize(servernamelistsizex.width + 4, servernamelistsizex.height);


        wnd_pos servernamelistxx = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10002)->GetPos();
        wnd_size servernamelistsizexx = this->GetSize();
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10002)->MoveGWnd(servernamelistxx.x + 60, servernamelistxx.y);



        this->m_IRM.GetResObj<CIFButton>(46, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj<CIFButton>(46, 1)->SetEnabledState(false);
        this->m_IRM.GetResObj<CIFButton>(46, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);
        this->m_IRM.GetResObj(45, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(104, 1)->ShowGWnd(true);
        //  this->m_IRM.GetResObj(15, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(43, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(45, 1)->SetText(L"West");
        wnd_pos tt =  this->m_IRM.GetResObj(104, 1)->GetPos();
        this->m_IRM.GetResObj(104, 1)->MoveGWnd(tt.x - 22, tt.y - 2);

        wnd_pos t5 =  this->m_IRM.GetResObj(45, 1)->GetPos();
        this->m_IRM.GetResObj(45, 1)->MoveGWnd(t5.x - 17, t5.y - 2);
        this->m_IRM.GetResObj(45, 1)->SetGWndSize(198, 17);

        wnd_pos chlist =  this->m_IRM.GetResObj(46, 1)->GetPos();
        this->m_IRM.GetResObj(46, 1)->MoveGWnd(chlist.x + 94, chlist.y - 4);


        wnd_pos logo;
        logo = this->m_IRM.GetResObj(GDR_LOGO, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_LOGO, 1)->MoveGWnd(logo.x, logo.y - 60);


        wnd_pos x;
        x = this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->TB_Func_13("clientlibrary\\charselect\\newlogin.ddj", 1, 1);
        this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->SetGWndSize(400, 190);
        this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->MoveGWnd(x.x - 56, x.y - 47);




        wnd_pos connect;
        connect = this->m_IRM.GetResObj(GDR_CONNECTBTN, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_CONNECTBTN, 1)->MoveGWnd(connect.x - 10, connect.y);


        wnd_pos exit;
        exit = this->m_IRM.GetResObj(GDR_EXIT, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_EXIT, 1)->MoveGWnd(exit.x - 6, exit.y);

        wnd_pos listbtn;
        listbtn = this->m_IRM.GetResObj(GDR_LISTBTN, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_LISTBTN, 1)->MoveGWnd(listbtn.x + 94, listbtn.y - 2);


        wnd_pos id;
        id = this->m_IRM.GetResObj(GDR_ID, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_ID, 1)->SetGWndSize(138, 17);
        this->m_IRM.GetResObj(GDR_ID, 1)->MoveGWnd(id.x - 17, id.y - 2);


        wnd_pos pw;
        pw = this->m_IRM.GetResObj(GDR_PW, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_PW, 1)->SetGWndSize(198, 17);
        this->m_IRM.GetResObj(GDR_PW, 1)->MoveGWnd(pw.x - 17, pw.y - 2);


        wnd_pos sw;
        sw = this->m_IRM.GetResObj(GDR_SWNAME, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_SWNAME, 1)->SetGWndSize(198, 17);
        this->m_IRM.GetResObj(GDR_SWNAME, 1)->MoveGWnd(sw.x - 17, sw.y - 2);


        wnd_pos id1;
        id1 = this->m_IRM.GetResObj(GDR_101ID, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_101ID, 1)->SetGWndSize(53, 16);
        this->m_IRM.GetResObj(GDR_101ID, 1)->MoveGWnd(id1.x - 22, id1.y - 5);


        wnd_pos pw1;
        pw1 = this->m_IRM.GetResObj(GDR_102PW, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_102PW, 1)->SetGWndSize(53, 16);
        this->m_IRM.GetResObj(GDR_102PW, 1)->MoveGWnd(pw1.x - 22, pw1.y - 4);


        wnd_pos sw1;
        sw1 = this->m_IRM.GetResObj(GDR_103SW, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_103SW, 1)->SetGWndSize(53, 16);
        this->m_IRM.GetResObj(GDR_103SW, 1)->MoveGWnd(sw1.x - 22, sw1.y - 3);


        wnd_pos list = this->m_IRM.GetResObj(GDR_LIST, 1)->GetPos();
        this->m_IRM.GetResObj(GDR_LIST, 1)->TB_Func_13("clientlibrary\\charselect\\server_global_window.ddj", 1, 1);
        this->m_IRM.GetResObj(GDR_LIST, 1)->SetGWndSize(312, 340);
        this->m_IRM.GetResObj(GDR_LIST, 1)->MoveGWnd(list.x - 36, list.y);

        wnd_pos sliderctrl = this->m_IRM.GetResObj(54, 1)->GetPos();
        this->m_IRM.GetResObj(54, 1)->MoveGWnd(sliderctrl.x + 69, sliderctrl.y - 3);



        wnd_pos serverlist = this->m_IRM.GetResObj(51, 1)->GetPos();
        wnd_size serverlistsize = this->m_IRM.GetResObj(51, 1)->GetSize();
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->MoveGWnd(serverlist.x, serverlist.y +14);
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->SetGWndSize(serverlistsize.width+71, serverlistsize.height-14);
        this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->N00001C53 = serverlistsize.width + 71;


    }
    else
    {
        this->m_IRM.GetResObj(1926, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(1927, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(1928, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(1929, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(1930, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(1931, 1)->ShowGWnd(false);


    }

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\pstitle.txt");
    m_IRM.CreateInterfaceSection("SecondaryPass", this);

    m_IRM.GetResObj<CIFSecondaryPassword>(5000,1)->ShowGWnd(false);
    m_IRM.GetResObj<CIFSecondaryPassword>(5000,1)->UpdateMenuSize();
//    m_CustomDataManager->AvatarMallItemList.clear();
 //   m_CustomDataManager->CustomItemMallItemList.clear();
    m_CustomDataManager->_ActiveTitleNameNew.clear();
    m_CustomDataManager->_ActiveTitleColors.clear();
    m_CustomDataManager->m_IconsData.clear();
    m_CustomDataManager->MediaIcons.clear();
    m_CustomDataManager->m_LeftCharIcons.clear();
    m_CustomDataManager->m_RightCharIcons.clear();
    m_CustomDataManager->m_RefAchievement.clear();

    m_CustomDataManager->m_RefFellowPetSystem.clear();
    m_CustomDataManager->UniqueTargetHashmap.clear();
    m_CustomDataManager->UniqueTargetHashmapPlayer.clear();
    m_CustomDataManager->g_GroupSpawn_Type = 0;
    m_CustomDataManager->g_despawned_objects.clear();

    m_CustomDataManager->m_EventMapSettings.clear();
    m_CustomDataManager->HideEffects.clear();


    m_Player->m_AchievementsCondition.clear();
    m_Player->m_Achievements.clear();



    m_Player->m_MacroAutoSkillData.clear();
    m_Player->m_FellowSkillData.clear();

    m_Player->m_MagicPopTimerRunning = false;
    m_Player->m_MagicPopSettings = 700;


    m_Player->Enabled_AutoStr = false;
    m_Player->Enabled_AutoInt = false;
    m_Player->Enabled_AutoMastery = false;
    m_Player->Selected_AutoMasteryId = 0;
    m_Player->Enabled_AutoSkill = false;
    m_Player->Selected_AutoSkillId = 0;


    m_Player->PetSkillTimerRunning = false;


    m_Player->ReverseSlot = 9999;

    m_Player->m_MyNewAutoHunting_Enabled = false;


    m_Player->FocusedId = 0;
    m_Player->MailAddress = std::n_wstring();
   // this->ShowMessage(L"Welcome the New Server server protected by VFILTER V2.0", 0xFFFFF0);
    if(!m_Settings->PSTitleIsLoaded)
    {
        CMsgStreamBuffer buf(0xA150);
        SendMsg(buf);

        std::n_string IconPath = "interface\\worldmap\\wmap_sign_questnpc.ddj";
        m_CustomDataManager->MapIcon = Fun_CacheTexture_Create(std::n_string(IconPath));

        std::n_string PingIconPath = "clientlibrary\\common\\wmap_sign_location.ddj";
        m_CustomDataManager->PingIcon = Fun_CacheTexture_Create(std::n_string(PingIconPath));

          m_CustomDataManager->emojiList.insert(std::make_pair(L":01", "clientlibrary\\emoji\\emoji01.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":02", "clientlibrary\\emoji\\emoji02.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":03", "clientlibrary\\emoji\\emoji03.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":04", "clientlibrary\\emoji\\emoji04.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":05", "clientlibrary\\emoji\\emoji05.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":06", "clientlibrary\\emoji\\emoji06.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":07", "clientlibrary\\emoji\\emoji07.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":08", "clientlibrary\\emoji\\emoji08.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":09", "clientlibrary\\emoji\\emoji09.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":10", "clientlibrary\\emoji\\emoji10.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":11", "clientlibrary\\emoji\\emoji11.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":12", "clientlibrary\\emoji\\emoji12.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":13", "clientlibrary\\emoji\\emoji13.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":14", "clientlibrary\\emoji\\emoji14.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":15", "clientlibrary\\emoji\\emoji15.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":16", "clientlibrary\\emoji\\emoji16.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":17", "clientlibrary\\emoji\\emoji17.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":18", "clientlibrary\\emoji\\emoji18.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":19", "clientlibrary\\emoji\\emoji19.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":20", "clientlibrary\\emoji\\emoji20.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":21", "clientlibrary\\emoji\\emoji21.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":22", "clientlibrary\\emoji\\emoji22.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":23", "clientlibrary\\emoji\\emoji23.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":24", "clientlibrary\\emoji\\emoji24.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":25", "clientlibrary\\emoji\\emoji25.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":26", "clientlibrary\\emoji\\emoji26.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":27", "clientlibrary\\emoji\\emoji27.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":28", "clientlibrary\\emoji\\emoji28.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":29", "clientlibrary\\emoji\\emoji29.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":30", "clientlibrary\\emoji\\emoji30.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":31", "clientlibrary\\emoji\\emoji31.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":32", "clientlibrary\\emoji\\emoji32.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":33", "clientlibrary\\emoji\\emoji33.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":34", "clientlibrary\\emoji\\emoji34.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":35", "clientlibrary\\emoji\\emoji35.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":36", "clientlibrary\\emoji\\emoji36.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":37", "clientlibrary\\emoji\\emoji37.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":38", "clientlibrary\\emoji\\emoji38.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":39", "clientlibrary\\emoji\\emoji39.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":40", "clientlibrary\\emoji\\emoji40.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":41", "clientlibrary\\emoji\\emoji41.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L".1.", "clientlibrary\\emoji\\emoji42.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":43", "clientlibrary\\emoji\\emoji43.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":44", "clientlibrary\\emoji\\emoji44.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":45", "clientlibrary\\emoji\\emoji45.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":46", "clientlibrary\\emoji\\emoji46.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":47", "clientlibrary\\emoji\\emoji47.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":48", "clientlibrary\\emoji\\emoji48.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":49", "clientlibrary\\emoji\\emoji49.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":50", "clientlibrary\\emoji\\emoji50.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":51", "clientlibrary\\emoji\\emoji51.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":52", "clientlibrary\\emoji\\emoji52.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":53", "clientlibrary\\emoji\\emoji53.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":54", "clientlibrary\\emoji\\emoji54.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":55", "clientlibrary\\emoji\\emoji55.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":56", "clientlibrary\\emoji\\emoji56.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":57", "clientlibrary\\emoji\\emoji57.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":58", "clientlibrary\\emoji\\emoji58.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":59", "clientlibrary\\emoji\\emoji59.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":60", "clientlibrary\\emoji\\emoji60.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":61", "clientlibrary\\emoji\\emoji61.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":62:", "clientlibrary\\emoji\\emoji62.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":63", "clientlibrary\\emoji\\emoji63.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":64", "clientlibrary\\emoji\\emoji64.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":65", "clientlibrary\\emoji\\emoji65.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L":66", "clientlibrary\\emoji\\emoji66.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^01", "clientlibrary\\emoji\\pepe11.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^02", "clientlibrary\\emoji\\pepe12.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^03", "clientlibrary\\emoji\\pepe13.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^04", "clientlibrary\\emoji\\pepe14.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^05", "clientlibrary\\emoji\\pepe15.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^06", "clientlibrary\\emoji\\pepe01.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^07", "clientlibrary\\emoji\\pepe02.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^08", "clientlibrary\\emoji\\pepe03.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^09", "clientlibrary\\emoji\\pepe04.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^10", "clientlibrary\\emoji\\pepe05.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^11", "clientlibrary\\emoji\\pepe06.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^12", "clientlibrary\\emoji\\pepe07.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^13", "clientlibrary\\emoji\\pepe08.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^14", "clientlibrary\\emoji\\pepe09.ddj"));
          m_CustomDataManager->emojiList.insert(std::make_pair(L"^15", "clientlibrary\\emoji\\pepe10.ddj"));

       m_CustomDataManager->emojiListData[":01"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji01.ddj");
       m_CustomDataManager->emojiListData[":02"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji02.ddj");
       m_CustomDataManager->emojiListData[":03"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji03.ddj");
       m_CustomDataManager->emojiListData[":04"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji04.ddj");
       m_CustomDataManager->emojiListData[":05"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji05.ddj");
       m_CustomDataManager->emojiListData[":06"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji06.ddj");
       m_CustomDataManager->emojiListData[":07"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji07.ddj");
       m_CustomDataManager->emojiListData[":08"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji08.ddj");
       m_CustomDataManager->emojiListData[":09"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji09.ddj");
       m_CustomDataManager->emojiListData[":10"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji10.ddj");
       m_CustomDataManager->emojiListData[":11"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji11.ddj");
       m_CustomDataManager->emojiListData[":12"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji12.ddj");
       m_CustomDataManager->emojiListData[":13"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji13.ddj");
       m_CustomDataManager->emojiListData[":14"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji14.ddj");
       m_CustomDataManager->emojiListData[":15"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji15.ddj");
       m_CustomDataManager->emojiListData[":16"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji16.ddj");
       m_CustomDataManager->emojiListData[":17"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji17.ddj");
       m_CustomDataManager->emojiListData[":18"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji18.ddj");
       m_CustomDataManager->emojiListData[":19"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji19.ddj");
       m_CustomDataManager->emojiListData[":20"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji20.ddj");
       m_CustomDataManager->emojiListData[":21"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji21.ddj");
       m_CustomDataManager->emojiListData[":22"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji22.ddj");
       m_CustomDataManager->emojiListData[":23"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji23.ddj");
       m_CustomDataManager->emojiListData[":24"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji24.ddj");
       m_CustomDataManager->emojiListData[":25"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji25.ddj");
       m_CustomDataManager->emojiListData[":26"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji26.ddj");
       m_CustomDataManager->emojiListData[":27"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji27.ddj");
       m_CustomDataManager->emojiListData[":28"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji28.ddj");
       m_CustomDataManager->emojiListData[":29"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji29.ddj");
       m_CustomDataManager->emojiListData[":30"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji30.ddj");
       m_CustomDataManager->emojiListData[":31"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji31.ddj");
       m_CustomDataManager->emojiListData[":32"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji32.ddj");
       m_CustomDataManager->emojiListData[":33"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji33.ddj");
       m_CustomDataManager->emojiListData[":34"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji34.ddj");
       m_CustomDataManager->emojiListData[":35"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji35.ddj");
       m_CustomDataManager->emojiListData[":36"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji36.ddj");
       m_CustomDataManager->emojiListData[":37"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji37.ddj");
       m_CustomDataManager->emojiListData[":38"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji38.ddj");
       m_CustomDataManager->emojiListData[":39"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji39.ddj");
       m_CustomDataManager->emojiListData[":40"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji40.ddj");
       m_CustomDataManager->emojiListData[":41"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji41.ddj");
       m_CustomDataManager->emojiListData[".1."] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji42.ddj");
       m_CustomDataManager->emojiListData[":43"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji43.ddj");
       m_CustomDataManager->emojiListData[":44"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji44.ddj");
       m_CustomDataManager->emojiListData[":45"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji45.ddj");
       m_CustomDataManager->emojiListData[":46"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji46.ddj");
       m_CustomDataManager->emojiListData[":47"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji47.ddj");
       m_CustomDataManager->emojiListData[":48"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji48.ddj");
       m_CustomDataManager->emojiListData[":49"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji49.ddj");
       m_CustomDataManager->emojiListData[":50"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji50.ddj");
       m_CustomDataManager->emojiListData[":51"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji51.ddj");
       m_CustomDataManager->emojiListData[":52"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji52.ddj");
       m_CustomDataManager->emojiListData[":53"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji53.ddj");
       m_CustomDataManager->emojiListData[":54"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji54.ddj");
       m_CustomDataManager->emojiListData[":55"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji55.ddj");
       m_CustomDataManager->emojiListData[":56"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji56.ddj");
       m_CustomDataManager->emojiListData[":57"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji57.ddj");
       m_CustomDataManager->emojiListData[":58"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji58.ddj");
       m_CustomDataManager->emojiListData[":59"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji59.ddj");
       m_CustomDataManager->emojiListData[":60"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji60.ddj");
       m_CustomDataManager->emojiListData[":61"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji61.ddj");
       m_CustomDataManager->emojiListData[":62"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji62.ddj");
       m_CustomDataManager->emojiListData[":63"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji63.ddj");
       m_CustomDataManager->emojiListData[":64"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji64.ddj");
       m_CustomDataManager->emojiListData[":65"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji65.ddj");
       m_CustomDataManager->emojiListData[":66"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\emoji66.ddj");
       m_CustomDataManager->emojiListData["^01"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe11.ddj");
       m_CustomDataManager->emojiListData["^02"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe12.ddj");
       m_CustomDataManager->emojiListData["^03"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe13.ddj");
       m_CustomDataManager->emojiListData["^04"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe14.ddj");
       m_CustomDataManager->emojiListData["^05"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe15.ddj");
       m_CustomDataManager->emojiListData["^06"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe01.ddj");
       m_CustomDataManager->emojiListData["^07"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe02.ddj");
       m_CustomDataManager->emojiListData["^08"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe03.ddj");
       m_CustomDataManager->emojiListData["^09"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe04.ddj");
       m_CustomDataManager->emojiListData["^10"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe05.ddj");
       m_CustomDataManager->emojiListData["^11"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe06.ddj");
       m_CustomDataManager->emojiListData["^12"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe07.ddj");
       m_CustomDataManager->emojiListData["^13"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe08.ddj");
       m_CustomDataManager->emojiListData["^14"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe09.ddj");
       m_CustomDataManager->emojiListData["^15"] = Fun_CacheTexture_Create("clientlibrary\\emoji\\pepe10.ddj");


    }
    return a;
}

void CPSTitle::PressConnectButton()
{
    if(this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->IsVisible())
        return;
    reinterpret_cast<void(__thiscall *)(CPSTitle *)>(0x00869460)(this);
    this->current_state = 0;

}
void CPSTitle::PressButtonServerList()
{
    if(this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->IsVisible())
        return;
    reinterpret_cast<void(__thiscall *)(CPSTitle *)>(0x00864c80)(this);

}
enum eResultType
{
    ENTER_THE_PASSWORD = 0,
    CREATE_THE_PASSWORD = 1,
    CREATED_SUCCESS_ENTER_THE_PASSWORD = 2,
    WRONG_PASSWORD = 3,
    PW_TRUE = 4,
    PASSWORD_CHANGED_ENTER_THE_PASSWORD = 5,
};
bool CPSTitle::OnServerPacketRecv(CMsgStreamBuffer *msg) {

    if (msg->msgid() == 0xB003)//notice
    {
        int Color;
        std::n_string notice;
        *msg >> notice >> Color;

        this->ShowMessage(TO_WSTRING(notice).c_str(), Color);
        //printf("showmesggg");
        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x1215)
    {
        byte GlobalOperationFlag;
        *msg >> GlobalOperationFlag;

        if(GlobalOperationFlag == 1)
        {
            byte GlobalOperationType;
            std::n_string GlobalOperationName;
            *msg >> GlobalOperationType >> GlobalOperationName >> GlobalOperationFlag;
        }
        byte ShardFlag;
        *msg >> ShardFlag;

        while(ShardFlag == 1)
        {
            unsigned short ShardID;
            std::n_string ShardName;
            unsigned short ShardCurrent;

            unsigned short ShardCapacity;
            byte ShardStatus;
            byte GlobalOperationID;
            *msg >> ShardID >> ShardName >> ShardCurrent >> ShardCapacity >> ShardStatus >> GlobalOperationID;
            *msg >> ShardFlag;

            if(ShardStatus == 0)
            {
                this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_black.ddj", 1, 1);
                this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_black.ddj", 1, 1);

            }
            else if(ShardStatus == 1)
            {
                double yuzde_degeri;
                if (ShardCapacity == 0) {
                    yuzde_degeri = 0;
                } else {
                    yuzde_degeri = (static_cast<double>(ShardCurrent) / ShardCapacity) * 100;

                }
                if(yuzde_degeri < 50)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_green.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_green.ddj", 1, 1);
                }
                else if(yuzde_degeri >= 50 && yuzde_degeri < 80)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_yellow.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_yellow.ddj", 1, 1);
                }
                else if(yuzde_degeri >= 80)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_red.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_red.ddj", 1, 1);

                }
            }

        }
        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x1216) /// for duck
    {
        int sCount;
        *msg >> sCount;

        int i = 0;
        while (i < sCount && sCount > 0) {
            i++;
            byte ShardStatus;
            *msg >> ShardStatus;

            unsigned short ShardCapacity;
            unsigned short ShardCurrent;
            *msg >> ShardCapacity;
            *msg >> ShardCurrent;

            if(ShardStatus == 0)
            {
                this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_black.ddj", 1, 1);
                this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_black.ddj", 1, 1);

            }
            else if(ShardStatus == 1)
            {
                double yuzde_degeri;
                if (ShardCapacity == 0) {
                    yuzde_degeri = 0;
                } else {
                    yuzde_degeri = (static_cast<double>(ShardCurrent) / ShardCapacity) * 100;

                }
                if(yuzde_degeri < 50)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_green.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_green.ddj", 1, 1);
                }
                else if(yuzde_degeri >= 50 && yuzde_degeri < 80)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_yellow.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_yellow.ddj", 1, 1);
                }
                else if(yuzde_degeri >= 80)
                {
                    this->m_IRM.GetResObj(1929, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_red.ddj", 1, 1);
                    this->m_IRM.GetResObj(1927, 1)->TB_Func_13("clientlibrary\\charselect\\login_window_eu_located_red.ddj", 1, 1);

                }
            }

        }
        msg->m_currentReadBytes = 0;
    }
    else if (msg->msgid() == 0x1210) {
        //if (g_CGame->langId == 2) {
        //    const wchar_t *msg = L"Bu sunucu Lexa Shield tarafından korunmaktadır.";
        //    this->ShowMessage(msg, 0x87ceeb);
        //} else if (g_CGame->langId == 5) {
        //    this->ShowMessage(L"This server protected by Lexa Shield.", 0x87ceeb);
        //}

        if (theApp.GetCurrentProcess() != NULL) {
            std::n_wstring tag = L"VSROR_Client";
            SetWindowText(theApp.GetHWnd(), TO_STRING(tag).c_str());
            SetWindowLong(theApp.GetHWnd(), GWL_STYLE,
                          GetWindowLong(theApp.GetHWnd(), GWL_STYLE) | WS_MINIMIZEBOX);
        }


        *msg >> m_Settings->OldLogin; //1
        *msg >> m_Settings->OldExpBar;//2
        *msg >> m_Settings->OldAlchemy;//3
        *msg >> m_Settings->GrantName;//4
        *msg >> m_Settings->IconManager;//5
        *msg >> m_Settings->IconManagerRight;//6
        *msg >> m_Settings->TitleManager;//7
        *msg >> m_Settings->TitleManagerColor;//8
        *msg >> m_Settings->RankingWnd;//9
        *msg >> m_Settings->UniqueHistoryWnd; //10
        *msg >> m_Settings->EventRegisterWnd; //11
        *msg >> m_Settings->EventScheduleWnd;//12
        *msg >> m_Settings->AchievementsWnd;//13
        *msg >> m_Settings->SecondarySlot;//14
        *msg >> m_Settings->MoveSkillBoard >>m_Settings->EnableServerInfoSkill >> m_Settings->EnableOldMainPopup >> m_Settings->HideOldTitleWhileNewTitle;
        if(m_Settings->OldLogin)
        {

            replaceOffset(0x00860990, addr_from_this(&CPSCharacterSelect::aCPSCharacterSelect));
            replaceOffset(0x0085cb8b, addr_from_this(&CCameraWorking::AddKeyframeForSelectIdol));
        }
        if(m_Settings->OldExpBar)
        {
            /// old undermenu var
            // replaceAddr(0x00dd182c, addr_from_this(&CIFCOSManager::OnCreateIMPL));
            // replaceAddr(0x00dba97c, addr_from_this(&CIFCOSCommand::OnCreateIMPL));

            replaceOffset(0x007a2858, addr_from_this(&CIFCOSCommand::FUN_007a20a0));
            replaceOffset(0x0081145c, addr_from_this(&CIFCOSCommand::FUN_007a20a0));

            PatchMe(0x00BA2156 + 7, 0XB0);

            PatchMe(0x00BA2156 + 6, 0x36);
        }
        if(m_Settings->OldAlchemy)
        {
            /// old alchemy

            static const char *alchemy_str = "icon\\alchemy_slot_nothing.ddj";
            replaceAddr(0x00574389+1, (int)(alchemy_str));
            replaceAddr(0x00574CC8+1, (int)(alchemy_str));
            replaceAddr(0x0057510C+1, (int)(alchemy_str));
            replaceAddr(0x0057584C+1, (int)(alchemy_str));
            replaceAddr(0x00577159+1, (int)(alchemy_str));
            replaceAddr(0x00578005+1, (int)(alchemy_str));
            replaceAddr(0x0057871B+1, (int)(alchemy_str));
            replaceAddr(0x0057A89C+1, (int)(alchemy_str));
            replaceAddr(0x0059C98A+1, (int)(alchemy_str));
        }

        if (m_Settings->EnableServerInfoSkill) {
            replaceOffset(0x00803d96, addr_from_this(&CIFStateSlot::FUN_00806d50));
        }

        *msg >> m_Settings->ItemComparison;
        if(m_Settings->ItemComparison)
        {

           replaceOffset(0x00682AFC, addr_from_this(&CIFSlotWithHelp::AppendAdvancedInfo));
           replaceOffset(0x00682D6E, addr_from_this(&CIFSlotWithHelp::AppendAdvancedInfo));
           replaceOffset(0x00682FBE, addr_from_this(&CIFSlotWithHelp::AppendAdvancedInfo));
           replaceOffset(0x0068320E, addr_from_this(&CIFSlotWithHelp::AppendAdvancedInfo));
        }
        *msg >> m_Settings->AutoSortButton;

        *msg >> m_Settings->EnablePtMemberViewer;

        *msg >> m_Settings->EnableAutoSkill;
        if(m_Settings->EnableAutoSkill)
        {
            static const char *skillboard = "clientlibrary\\resinfo\\ifskillboard.txt";
            replaceAddr(0x0069c4f4+1, (int)(skillboard));
        }
        *msg >> m_Settings->MaxMasteryLevel;

        *msg >> m_Settings->ServerMaxLevel;

        WriteMemoryValue<byte>(0x008A99A2 + 2, m_Settings->ServerMaxLevel);// Level up limit
        WriteMemoryValue<byte>(0x0069C7C8 + 1, m_Settings->ServerMaxLevel);// Mastery limit


        PatchMe(0x006AA4C3 + 1, m_Settings->MaxMasteryLevel);//
        PatchMe(0x006A51BC + 1, m_Settings->MaxMasteryLevel);//

        WriteMemoryValue<byte>(0x0073AFAE + 1, m_Settings->ServerMaxLevel);// Party Match
        WriteMemoryValue<byte>(0x0073B013 + 1, m_Settings->ServerMaxLevel);// Party Match
        WriteMemoryValue<byte>(0x0073B030 + 1, m_Settings->ServerMaxLevel);// Party Match
        WriteMemoryValue<byte>(0x0073FA4C + 1, m_Settings->ServerMaxLevel);// Party Match
        WriteMemoryValue<byte>(0x0073FAAF + 1, m_Settings->ServerMaxLevel);// Party Match
        WriteMemoryValue<byte>(0x0073FACC + 1, m_Settings->ServerMaxLevel);// Party Match

        byte DamageText;
        *msg >> DamageText;
        if (DamageText == 1) {
            vftableHook(0x00de54ec, 9, addr_from_this(&CIDamageText::Func_9_IMPL));
        }

        *msg >> m_Settings->EnableAutoStrInt;

        *msg >> m_Settings->EnablePickSoxEffect;

        byte permanentalchemy;
        *msg >> permanentalchemy;
        if (permanentalchemy == 1) {
            PatchAlchemyPerm();

            JMPFunction(0x0059C95D, 0x0059CA04);
            RenderNop((void *) 0x0059C962, 1);
            RenderNop((void *) 0x0059C963, 1);
            RenderNop((void *) 0x0059C964, 1);
            RenderNop((void *) 0x0059CA0A, 1);
            RenderNop((void *) 0x0059CA0A, 1);
            RenderNop((void *) 0x0059CA24, 1);
            RenderNop((void *) 0x0059CA25, 1);
            RenderNop((void *) 0x0059CA26, 1);
            RenderNop((void *) 0x0059CA27, 1);
            RenderNop((void *) 0x0059CA28, 1);
            RenderNop((void *) 0x0059DF54, 1);
            RenderNop((void *) 0x0059DF55, 1);
            RenderNop((void *) 0x0059DF56, 1);
            RenderNop((void *) 0x0059DF57, 1);
            RenderNop((void *) 0x0059DF58, 1);
            RenderNop((void *) 0x0059DF59, 1);
            RenderNop((void *) 0x0059DF5A, 1);
            RenderNop((void *) 0x0059DF61, 1);
            RenderNop((void *) 0x0059DF62, 1);
            RenderNop((void *) 0x0059DF63, 1);
            RenderNop((void *) 0x0059DF64, 1);
            RenderNop((void *) 0x0059DF65, 1);
        }

        *msg >> m_Settings->GuildJobMode;

        *msg >> m_Settings->EnableUniqueTarget;
        if (m_Settings->EnableUniqueTarget == 1) {
            replaceOffset(0x00779aaa, addr_from_this(&CICPlayer::EffectHook));
        }
        *msg >> m_Settings->EnableMacro;
        byte secondpw;
        *msg >> secondpw >> m_Settings->NewCharInfoScreen >> m_Settings->NewIDPWScreen;

        if(!m_Settings->PSTitleIsLoaded)
        {
            if(m_Settings->NewIDPWScreen)
            {

                this->m_IRM.GetResObj(1926, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1927, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1928, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1929, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1930, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1931, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(1930, 1)->SetText(L"Location");
                this->m_IRM.GetResObj(104, 1)->SetText(L"CHANNEL");

                wnd_pos servernamelist = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->GetPos();
                wnd_size servernamelistsize = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->GetSize();
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->MoveGWnd(servernamelist.x + 22, servernamelist.y);
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10000)->SetGWndSize(servernamelistsize.width - 22, servernamelistsize.height - 14);

                wnd_pos servernamelistx = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->GetPos();
                wnd_size servernamelistsizex = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->GetSize();
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->MoveGWnd(servernamelistx.x + 38, servernamelistx.y);
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10001)->SetGWndSize(servernamelistsizex.width + 4, servernamelistsizex.height);


                wnd_pos servernamelistxx = this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10002)->GetPos();
                wnd_size servernamelistsizexx = this->GetSize();
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->GetGuiFromList<CIFSimpleListCtrl>(10002)->MoveGWnd(servernamelistxx.x + 60, servernamelistxx.y);



                this->m_IRM.GetResObj<CIFButton>(46, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj<CIFButton>(46, 1)->SetEnabledState(false);
                this->m_IRM.GetResObj<CIFButton>(46, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);
                this->m_IRM.GetResObj(45, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(104, 1)->ShowGWnd(true);
                //  this->m_IRM.GetResObj(15, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(43, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj(45, 1)->SetText(L"JT-Online");
                wnd_pos tt =  this->m_IRM.GetResObj(104, 1)->GetPos();
                this->m_IRM.GetResObj(104, 1)->MoveGWnd(tt.x - 22, tt.y - 2);

                wnd_pos t5 =  this->m_IRM.GetResObj(45, 1)->GetPos();
                this->m_IRM.GetResObj(45, 1)->MoveGWnd(t5.x - 17, t5.y - 2);
                this->m_IRM.GetResObj(45, 1)->SetGWndSize(198, 17);

                wnd_pos chlist =  this->m_IRM.GetResObj(46, 1)->GetPos();
                this->m_IRM.GetResObj(46, 1)->MoveGWnd(chlist.x + 94, chlist.y - 4);


                wnd_pos logo;
                logo = this->m_IRM.GetResObj(GDR_LOGO, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_LOGO, 1)->MoveGWnd(logo.x, logo.y - 60);


                wnd_pos x;
                x = this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->TB_Func_13("clientlibrary\\charselect\\newlogin.ddj", 1, 1);
                this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->SetGWndSize(400, 190);
                this->m_IRM.GetResObj(GDR_IDPWFRAME, 1)->MoveGWnd(x.x - 56, x.y - 47);




                wnd_pos connect;
                connect = this->m_IRM.GetResObj(GDR_CONNECTBTN, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_CONNECTBTN, 1)->MoveGWnd(connect.x - 10, connect.y);


                wnd_pos exit;
                exit = this->m_IRM.GetResObj(GDR_EXIT, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_EXIT, 1)->MoveGWnd(exit.x - 6, exit.y);

                wnd_pos listbtn;
                listbtn = this->m_IRM.GetResObj(GDR_LISTBTN, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_LISTBTN, 1)->MoveGWnd(listbtn.x + 94, listbtn.y - 2);


                wnd_pos id;
                id = this->m_IRM.GetResObj(GDR_ID, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_ID, 1)->SetGWndSize(138, 17);
                this->m_IRM.GetResObj(GDR_ID, 1)->MoveGWnd(id.x - 17, id.y - 2);


                wnd_pos pw;
                pw = this->m_IRM.GetResObj(GDR_PW, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_PW, 1)->SetGWndSize(198, 17);
                this->m_IRM.GetResObj(GDR_PW, 1)->MoveGWnd(pw.x - 17, pw.y - 2);


                wnd_pos sw;
                sw = this->m_IRM.GetResObj(GDR_SWNAME, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_SWNAME, 1)->SetGWndSize(198, 17);
                this->m_IRM.GetResObj(GDR_SWNAME, 1)->MoveGWnd(sw.x - 17, sw.y - 2);


                wnd_pos id1;
                id1 = this->m_IRM.GetResObj(GDR_101ID, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_101ID, 1)->SetGWndSize(53, 16);
                this->m_IRM.GetResObj(GDR_101ID, 1)->MoveGWnd(id1.x - 22, id1.y - 5);


                wnd_pos pw1;
                pw1 = this->m_IRM.GetResObj(GDR_102PW, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_102PW, 1)->SetGWndSize(53, 16);
                this->m_IRM.GetResObj(GDR_102PW, 1)->MoveGWnd(pw1.x - 22, pw1.y - 4);


                wnd_pos sw1;
                sw1 = this->m_IRM.GetResObj(GDR_103SW, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_103SW, 1)->SetGWndSize(53, 16);
                this->m_IRM.GetResObj(GDR_103SW, 1)->MoveGWnd(sw1.x - 22, sw1.y - 3);


                wnd_pos list = this->m_IRM.GetResObj(GDR_LIST, 1)->GetPos();
                this->m_IRM.GetResObj(GDR_LIST, 1)->TB_Func_13("clientlibrary\\charselect\\server_global_window.ddj", 1, 1);
                this->m_IRM.GetResObj(GDR_LIST, 1)->SetGWndSize(312, 340);
                this->m_IRM.GetResObj(GDR_LIST, 1)->MoveGWnd(list.x - 36, list.y);

                wnd_pos sliderctrl = this->m_IRM.GetResObj(54, 1)->GetPos();
                this->m_IRM.GetResObj(54, 1)->MoveGWnd(sliderctrl.x + 69, sliderctrl.y - 3);



                wnd_pos serverlist = this->m_IRM.GetResObj(51, 1)->GetPos();
                wnd_size serverlistsize = this->m_IRM.GetResObj(51, 1)->GetSize();
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->MoveGWnd(serverlist.x, serverlist.y +14);
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->SetGWndSize(serverlistsize.width+71, serverlistsize.height-14);
                this->m_IRM.GetResObj<CIFOListCtrl>(51, 1)->N00001C53 = serverlistsize.width + 71;


            }
            else
            {
                this->m_IRM.GetResObj(1926, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj(1927, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj(1928, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj(1929, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj(1930, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj(1931, 1)->ShowGWnd(false);


            }
            m_Settings->PSTitleIsLoaded = true;
        }
        *msg >> m_CustomDataManager->FacebookUrl >> m_CustomDataManager->DiscordUrl >> m_CustomDataManager->WebSiteUrl;
        *msg >> m_Settings->EnableChangeLog >> m_Settings->ShowChangeLogFirstLogin >> m_Settings->FixNewJobSuit;
        *msg >> m_Settings->EnableOldItemMall;
        *msg >> m_Settings->InsertCommaToPrices;
        *msg >> m_Settings->EnableCharacterBound;
        *msg >> m_Settings->EnableNewItemMall;
        if(m_Settings->EnableNewItemMall)
        {
            replaceOffset(0x0060cdb9, addr_from_this(&CGInterface::Wtf));
        }
        if(m_Settings->EnableOldItemMall)
        {
            replaceAddr(0xDC8A48, addr_from_this(&CIFItemMall::OnCloseWnd_IMPL));
            replaceAddr(0x00dc89c4, addr_from_this(&CIFItemMall::OnCreateIMPL));
            vftableHook(0x0D9B9CC, 12, addr_from_this(&CIFMessageBox::OnUpdateIMPL));

            HOOK_ORIGINAL_MEMBER(0x007978A0, &CGInterface::OnReservedItemBuy);
            HOOK_ORIGINAL_MEMBER(0x007D0460, &CIFItemMallZzim::OnBuyAll_BtnClick);
            HOOK_ORIGINAL_MEMBER(0x007D0350, &CIFItemMallZzim::OnBuyAllCallBack);
            HOOK_ORIGINAL_MEMBER(0x0079B580, &CGInterface::OnItemMallSectionControl);

            /* replaceOffset(0x007bfe7b, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007c0646, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007cf114, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007d0318, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007d042c, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007d80e2, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007d83da, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007db0d5, addr_from_this(&CGInterface::OnReservedItemBuy));
             replaceOffset(0x007db3a0, addr_from_this(&CGInterface::OnReservedItemBuy));

             replaceOffset(0x00652c9f, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x007879b2, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x00798774, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x007d16c8, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x0087e00e, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x008a1474, addr_from_this(&CGInterface::OnItemMallSectionControl));
             replaceOffset(0x0087e31a, addr_from_this(&CGInterface::OnItemMallSectionControl));

             replaceOffset(0x007D0350, addr_from_this(&CIFItemMallZzim::OnBuyAllCallBack));
             replaceOffset(0x007d0659, addr_from_this(&CIFItemMallZzim::OnBuyAll_BtnClick));*/


        }

        *msg >> m_Settings->ENABLE_EMOJI_SYSTEM;
        if(m_Settings->ENABLE_EMOJI_SYSTEM)
        {
            replaceOffset(0x0063570B, addr_from_this(&CGFontTexture::Render_0));
            replaceOffset(0x00639FE0, addr_from_this(&CGFontTexture::Render_0));
            replaceOffset(0x008B4EA1, addr_from_this(&CGFontTexture::Render_1));

        }
        *msg >> m_Settings->ENABLE_NEW_PT_MATCH;


        *msg >> m_Settings->ENABLE_NEW_JOB_UI;

        *msg >> m_Settings->ENABLE_NEW_ALCHEMY;

        *msg >> m_Settings->EnableItemTranslation;
        *msg >> m_Settings->ItemTranslationPayment;
        *msg >> m_Settings->ItemTranslationPrice;

        msg->m_currentReadBytes = 0;
    }
    else if (msg->msgid() == 0xA340)//notice
    {
        std::n_wstring notice;
        *msg >> notice;;

        this->ShowMessage(notice.c_str(), 0xFFFF671D);

        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x1212)
    {
        byte type;
        *msg >> type;
        byte locale;
        std::n_string UserName;
        std::n_string Password;
        unsigned __int16 wShardID;

        /**msg >> locale;
        *msg >> UserName;
        *msg >> Password;
        *msg >> wShardID;*/
        if(type == eResultType::CREATE_THE_PASSWORD)
        {
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);

            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(true);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetMode(0);

            this->ShowMessage(L"You will create a secondary passcode for further protection of your account.", 0xFFFF671D);


            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == eResultType::ENTER_THE_PASSWORD) /// enter pass
        {
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);


            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->UpdateMenuSize();
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(true);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetMode(1);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetRememberPC(false);

//            this->ShowMessage(L"Click the keypad to the right to create a passcode.", 0xFFFF671D);

            CGEffSoundBody::get()->PlaySound(L"snd_error");

        }
        else if(type == eResultType::CREATED_SUCCESS_ENTER_THE_PASSWORD)
        {
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);

            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->UpdateMenuSize();
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(true);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetMode(1);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetRememberPC(false);

            this->ShowMessage(L"Password created succesfully. Click the keypad to the right to create a passcode.", 0xFFFF671D);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ClearAll();

            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == eResultType::WRONG_PASSWORD)
        {

            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);

            this->ShowMessage(L"Invalid passcode. If you enter the wrong passcode more than six times, you will be locked out for 60 minutes.", 0xFFFF671D);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == eResultType::PW_TRUE)
        {
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(true);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(true);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("interface\\outer\\button.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("interface\\outer\\button.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(true);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("interface\\outer\\list_button.ddj",1,1);


            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(false);
            this->ShowMessage(L"Secondary passcode has been entered succesfully.", 0xFFFF671D);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == eResultType::PASSWORD_CHANGED_ENTER_THE_PASSWORD)
        {
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->UpdateMenuSize();
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(true);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetMode(1);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetRememberPC(false);

            this->ShowMessage(L"Password changed succesfully. Enter the new passcode.", 0xFFFF671D);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ClearAll();

            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == eResultType::WRONG_PASSWORD)
        {
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(GDR_CONNECTBTN, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);
            this->m_IRM.GetResObj<CIFButton>(12, 1)->TB_Func_13("clientlibrary\\common\\button_disable.ddj", 1, 1);

            this->m_IRM.GetResObj<CIFButton>(44, 1)->SetEnabledState(false);
            this->m_IRM.GetResObj<CIFButton>(44, 1)->TB_Func_13("clientlibrary\\common\\list_button_disable.ddj",1,1);


            this->ShowMessage(L"Invalid passcode. If you enter the wrong passcode more than six times, you will be locked out for 60 minutes.", 0xFFFF671D);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
       /* else if(type == 2)
        {
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->UpdateMenuSize();
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->ShowGWnd(true);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetMode(type);

            this->m_IRM.GetResObj<CIFSecondaryPassword>(5000, 1)->SetRememberPC(false);


            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_CREATE_NUMBER"), 0xFFFFF0);


            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == 3)
        {

            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->SetMode(1);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->SetRememberPC(true);
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_ENTERED_SUCCESS"), 0xFFFFF0);


            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == 5)
        {
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_ENTER_ERROR"), 0xFFFFF0);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == 6)
        {
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_ENTERED_SUCCESS"), 0xFFFFF0);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->ClearAll();
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->ShowGWnd(false);
        }
        else if(type == 7)
        {
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->SetMode(1);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->ClearAll();
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_CREATE_SUCCESS"), 0xFFFFF0);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == 8)
        {
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->SetMode(0);
            this->m_IRM.GetResObj<CIFSecondaryPassword>(300, 1)->ClearAll();
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_SECOND_PASSWORD_REMOVED_SUCCESS"), 0xFFFFF0);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }
        else if(type == 9)
        {
            this->ShowMessage(TSM_GETTEXTPTR(L"VFILTER_HWID_ERROR"), 0xFFFFF0);
            CGEffSoundBody::get()->PlaySound(L"snd_error");
        }*/
        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x165A)
    {
        HWIDGenerator hwidGenerator;
        std::string hwid = hwidGenerator.GenerateHWID();

        CMsgStreamBuffer buf(0x165B);
        buf << std::n_string(hwid.c_str());
        SendMsg(buf);

        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x208F)
    {
        m_CustomDataManager->CustomItemMallItemList.clear();
        int Count;
        *msg >> Count;
        int i = 0;
        while (i < Count && Count > 0) {
            i++;
            int ID;
            std::n_string CategoryName;
            byte Type;
            int ItemID;
            int ItemCount;
            int Silk;
            int ItemIndex;
            byte ShowInNewBest;
            int NewBestItemIndex;

            *msg >> ID;
            *msg >> CategoryName;
            *msg >> Type;
            *msg >> ItemID;
            *msg >> ItemCount;
            *msg >> Silk;

            *msg >> ShowInNewBest;

            *msg >> ItemIndex;

            *msg >> NewBestItemIndex;
            CustomDataManager::CustomItemMallItemStruct Data = CustomDataManager::CustomItemMallItemStruct();
            Data.ID = ID;
            Data.Category = TO_NWSTRING(CategoryName);
            Data.Type = Type;
            Data.ItemID = ItemID;
            Data.ItemCount = ItemCount;
            Data.SilkPrice = Silk;
            Data.ShowInNewBest= ShowInNewBest;
            Data.ItemIndex = ItemIndex;
            Data.NewBestItemIndex = NewBestItemIndex;
            m_CustomDataManager->CustomItemMallItemList.insert((std::make_pair(ID, Data)));
        }
        msg->m_currentReadBytes = 0;
    }
    else if(msg->msgid() == 0x209B)
    {
        m_CustomDataManager->AvatarMallItemList.clear();
        int Count;
        *msg >> Count;
        int i = 0;
        while (i < Count && Count > 0) {
            i++;
            int ID;
            byte Type;
            int ItemID;
            int Silk;
            int ItemIndex;
            int PetObjID;
            *msg >> ID;
            *msg >> Type;
            *msg >> ItemID;
            *msg >> Silk;
            *msg >> ItemIndex;
            *msg >> PetObjID;

            CustomDataManager::AvatarMallStruct Data = CustomDataManager::AvatarMallStruct();
            Data.ID = ID;
            Data.CategoryID = Type;
            Data.ItemID = ItemID;
            Data.Silk = Silk;
            Data.ItemIndex = ItemIndex;
            Data.PetObjID = PetObjID;
            m_CustomDataManager->AvatarMallItemList.insert((std::make_pair(ID, Data)));
        }
        msg->m_currentReadBytes = 0;
    }
    return reinterpret_cast<int(__thiscall *)(CPSTitle *, CMsgStreamBuffer *)>(0x0086bfc0)(this, msg);
}
void CPSTitle::PatchAlchemyPerm() {
#pragma pack(push, 1)
    struct {
        BYTE opcode;
        BYTE puch;
    } Pushing;
#pragma pack(pop)
    Pushing.opcode = 0x6A;// push (constant)
    Pushing.puch = 1;
    CopyBytes(0x0059EB2F, &Pushing, sizeof(Pushing));
}
