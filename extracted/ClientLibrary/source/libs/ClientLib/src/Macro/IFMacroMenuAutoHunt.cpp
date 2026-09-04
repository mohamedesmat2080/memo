#include "IFMacroMenuAutoHunt.h"
#include "Game.h"
#include "IFMacroMenu.h"
#include <BSLib/Debug.h>
#include <GInterface.h>
#include <IFSliderCtrl.h>
#include <ICPlayer.h>
#include <TextStringManager.h>
#include <CustomData/CustomCICPlayer.h>
#include <GlobalDataManager.h>
#include <CustomData/CustomSettingManager.h>
#include <CustomInterface/IFPopupList.h>
#include <EntityManagerClient.h>
#include <ICMonster.h>
#include <cmath>
#include <IFPlayerMiniInfo.h>
#include <CharacterDependentData.h>
#include <sstream>
#include <BSLib/multibyte.h>
#include <fstream>
#include <iostream>



#include "IFMacro.h"


GFX_IMPLEMENT_DYNCREATE(CIFMacroMenuAutoHunt, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFMacroMenuAutoHunt, CIFWnd)
                    ONG_COMMAND(300, &CIFMacroMenuAutoHunt::OnUnknownStuff)
                    ONG_COMMAND(301, &CIFMacroMenuAutoHunt::OnUnknownStuff)
                    ONG_COMMAND(29, &CIFMacroMenuAutoHunt::SaveButton)
                    ONG_COMMAND(30, &CIFMacroMenuAutoHunt::CancelBtn)

                    ONG_COMMAND(71, &CIFMacroMenuAutoHunt::AddPtMemberBtn)
                    ONG_COMMAND(72, &CIFMacroMenuAutoHunt::RemovePtMemberBtn)
                    ONG_COMMAND(99999, &CIFMacroMenuAutoHunt::LoadPartyMembers)

                    ONG_COMMAND(84, &CIFMacroMenuAutoHunt::PartyExpItemSetting)
                    ONG_COMMAND(87, &CIFMacroMenuAutoHunt::PartyExpItemSetting)
                    ONG_COMMAND(90, &CIFMacroMenuAutoHunt::PartyExpItemSetting)
                    ONG_COMMAND(93, &CIFMacroMenuAutoHunt::PartyExpItemSetting)



GFX_END_MESSAGE_MAP()

void CIFMacroMenuAutoHunt::LoadPartyMembers()
{

    int numtoDel =  m_popup->m_list->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_popup->m_list->Removeline(0);
    }
    m_popup->m_list->m_CurrentLines = 0;

    const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();

    if (partyData.NumberOfMembers > 0)
    {
        if (partyData.NumberOfMembers > 0) {

            for (int i = 0; i < partyData.NumberOfMembers; ++i) {

                const SPartyMemberData& memberData = g_CCharacterDependentData.GetPartyMemberData(i);
                if(g_pMyPlayerObj->GetCharName() != memberData.m_charactername)
                {
                    std::n_wstring ptmember = memberData.m_charactername.c_str();
                    m_popup->m_list->sub_64F8A0(ptmember, 0, 0xffffff, 0xffffff, -1, 0, 0);
                }
            }
        }
    }

    m_popup->m_listbg->ShowGWnd(!m_popup->m_listbg->IsVisible());
    m_popup->m_list->ShowGWnd(!m_popup->m_list->IsVisible());

    m_popup->m_listbg->SetGWndSize(m_popup->m_listbg->GetSize().width, m_popup->m_list->m_CurrentLines * 14 + 10);
    m_popup->m_list->SetGWndSize(m_popup->m_listbg->GetSize().width, m_popup->m_list->m_CurrentLines * 14 + 10);

    m_popup->m_listbg->BringToFront();
    m_popup->m_list->BringToFront();
}
void CIFMacroMenuAutoHunt::AddPtMemberBtn(){

    std::n_wstring charname = m_popup->m_text->GetNText();
    if(charname.empty())
    {
        g_pCGInterface->ShowMessage_Notice(L"Enter the character name.");
        return;
    }
    if(AutoPartyMemberList.size() < 8)
    {
        if(AutoPartyMemberList.find(charname) == AutoPartyMemberList.end())
        {
            AutoPartyMemberList.insert(std::make_pair(charname, charname));
            ClearPartySlotDDJ();
        }
        else
        {
            g_pCGInterface->ShowMessage_Notice(L"This party member is already added to list.");
        }
    }
    else
    {
        g_pCGInterface->ShowMessage_Notice(L"Maximum character member can be add to list.");
    }
    int i = 0;
    for(std::map<std::n_wstring, std::n_wstring>::iterator  it = AutoPartyMemberList.begin(); it != AutoPartyMemberList.end(); it++)
    {
        i++;
        if(i < 8)
        {
            m_PartySlot[i-1]->LoadItems(it->first);
        }
        else
        {
            break;
        }

    }
    m_popup->m_text->SetText(L"");
}
void CIFMacroMenuAutoHunt::RemovePtMemberBtn(){
    if(!SelectedPartyMemberName.empty())
    {
        if(AutoPartyMemberList.find(SelectedPartyMemberName) != AutoPartyMemberList.end())
        {
            AutoPartyMemberList.erase(SelectedPartyMemberName);
            ClearPartySlotDDJ();

            for(int i = 0; i < 7; i++)
            {
                m_PartySlot[i]->LoadItems(L"");
            }

            int i = 0;
            for(std::map<std::n_wstring, std::n_wstring>::iterator  it = AutoPartyMemberList.begin(); it != AutoPartyMemberList.end(); it++)
            {
                i++;
                if(i < 8)
                {
                    m_PartySlot[i-1]->LoadItems(it->first);
                }
                else
                {
                    break;
                }

            }

        }

    }
}

int CIFMacroMenuAutoHunt::Func_4(int a2) {
    int v1 = 0;
    while (a2 != v1 + 100) {
        if (++v1 >= 17)
            return -1;
    }

    return 100;
}


CIFMacroMenuAutoHunt::CIFMacroMenuAutoHunt(void) {
    BS_DEBUG_LOW(">" __FUNCTION__);
    AutoPartyMemberList = std::map<std::n_wstring, std::n_wstring>();
    SelectedPartyMemberName = std::n_wstring();
    Macro_AutoHunt = false;
    AutoHuntSetting = std::map<eAutoHuntSetting, int>();


    MacroAutoTownTimerRunning = false;
    AutoHuntTimerRunning = false;
    MacroAutoInviteRunning = false;
}
CIFMacroMenuAutoHunt::~CIFMacroMenuAutoHunt(void) {
    if (m_pTabsSecond) {
        free(m_pTabsSecond);
        m_pTabsSecond = 0;
    }
    BS_DEBUG_LOW(">" __FUNCTION__);
}

bool CIFMacroMenuAutoHunt::OnCreate(long ln) {

    // Populate inherited members
    CIFWnd::OnCreate(ln);

    wnd_rect sz;
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifmacromenuautohunt.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    sz.pos.x= 45;
    sz.pos.y = 42;
    m_pTabsSecond = new CIFSelectableArea *[numberOfTabs];

    for (int i = 0; i < numberOfTabs; i++) {

        RECT selectable_area_size;
        selectable_area_size.top = 121;
        selectable_area_size.left = 24;
        // selectable_area_size.left = tabMarginLeft + tabWidth * i;
        selectable_area_size.right = tabWidth + 1;
        selectable_area_size.bottom = tabHeight;

        m_pTabsSecond[i] = (CIFSelectableArea*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSelectableArea),
                                                                     selectable_area_size, tabFirstId + i, 0);
        m_pTabsSecond[i]->SetFont(this->N00009C2F);


        m_pTabsSecond[i]->sub_64CE30("interface\\option\\opt_video_tab_long01_on.ddj",
                                     "interface\\option\\opt_video_tab_long01_off.ddj", "interface\\option\\opt_video_tab_long01_off.ddj");

        switch (i) {
            case 0:
            {
                m_pTabsSecond[0]->sub_64CC30(1);
                m_pTabsSecond[0]->SetText(L"General");
            }
                break;
            case 1:
            {
                m_pTabsSecond[1]->MoveGWnd(m_pTabsSecond[0]->GetPos().x + tabWidth + 10, m_pTabsSecond[0]->GetPos().y);
                m_pTabsSecond[1]->SetText(L"Party");
                m_pTabsSecond[1]->SetClickable(true);
            }
                break;
        }
        m_pTabsSecond[i]->sub_64CC30(0);

    }
    m_IRM.GetResObj<CIFVerticalScroll>(7, 1)->m_scrollButton->SetClickable(false);
    m_IRM.GetResObj<CIFVerticalScroll>(7, 1)->m_arrowDownButton->SetClickable(false);
    m_IRM.GetResObj<CIFVerticalScroll>(7, 1)->m_arrowUpButton->SetClickable(false);
    m_IRM.GetResObj<CIFVerticalScroll>(7, 1)->SetClickable(false);

    m_IRM.GetResObj(43, 1)->SetText(L"Dont Attack the Monsters");
    m_IRM.GetResObj(45, 1)->SetText(L"Configure Hunting Range");
    m_IRM.GetResObj(47, 1)->SetText(L"Configure Berserk Mode");
    m_IRM.GetResObj(49, 1)->SetText(L"Configure Auto-Resurrect");
    m_IRM.GetResObj(51, 1)->SetText(L"Configure Auto-Return");
    m_IRM.GetResObj(53, 1)->SetText(L"Configure Auto-Repair");
    m_IRM.GetResObj(55, 1)->SetText(L"Auto - Return when HP/MP potion < 10");
    m_IRM.GetResObj(57, 1)->SetText(L"Use Devil/Angel Spirit's");
    m_IRM.GetResObj(59, 1)->SetText(L"Use Devil/Angel Spirit's Ignore Skill");
    m_IRM.GetResObj(29, 1)->SetText(L"Save");

    m_IRM.GetResObj(107, 1)->SetText(L"Resurrect Party Members");
    m_IRM.GetResObj(105, 1)->SetText(L"Auto reform party");
    m_IRM.GetResObj(95, 1)->SetText(L"Accept join party requests");
    m_IRM.GetResObj(92, 1)->SetText(L"Exp Free / Item Free");
    m_IRM.GetResObj(89, 1)->SetText(L"Exp Share / Item Share");
    m_IRM.GetResObj(86, 1)->SetText(L"Exp Free / Item Share");
    m_IRM.GetResObj(83, 1)->SetText(L"Exp Share / Item Free");
    m_IRM.GetResObj(80, 1)->SetText(L"Only from list");
    m_IRM.GetResObj(77, 1)->SetText(L"Accept party invite");
    m_IRM.GetResObj(74, 1)->SetText(L"Invite to Party");
    m_IRM.GetResObj(72, 1)->SetText(L"Remove");
    m_IRM.GetResObj(71, 1)->SetText(L"Add");
    m_IRM.GetResObj(30, 1)->SetText(L"Cancel");

    AutoReturnHPMPLessCheckbox = m_IRM.GetResObj<CIFCheckBox>(56, 1);
    AutoUseNasrun = m_IRM.GetResObj<CIFCheckBox>(58, 1);
    AutoUseNasrunIgnore = m_IRM.GetResObj<CIFCheckBox>(60, 1);


    std::n_wstring area1 = L"30 m";
    std::n_wstring area2 = L"50 m";
    std::n_wstring area3 = L"70 m";
    std::n_wstring area4 = L"100 m";
    std::n_wstring area5 = L"150 m";
    std::n_wstring area6 = L"200 m";
    std::n_wstring area7 = L"300 m";

    RadiusList = m_IRM.GetResObj<CIFPopupList>(46, 1);
    RadiusList->m_list->sub_64F8A0(area1, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RadiusList->m_list->sub_64F8A0(area2, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RadiusList->m_list->sub_64F8A0(area3, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RadiusList->m_list->sub_64F8A0(area4, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RadiusList->m_list->sub_64F8A0(area5, 0, 0xffffff, 0xffffff, -1, 0, 0);
   // RadiusList->m_list->sub_64F8A0(area6, 0, 0xffffff, 0xffffff, -1, 0, 0);
   // RadiusList->m_list->sub_64F8A0(area7, 0, 0xffffff, 0xffffff, -1, 0, 0);

    RadiusList->m_text->SetText(area1.c_str());
    AutoHuntSetting.insert(std::make_pair(RADUIS_SETTING, 30));


    std::n_wstring hwan1 = L"All";
    std::n_wstring hwan2 = L"Giant";
    std::n_wstring hwan3 = L"Giant (Party)";


    ZerkList = m_IRM.GetResObj<CIFPopupList>(48, 1);
    ZerkList->m_list->sub_64F8A0(hwan1, 0, 0xffffff, 0xffffff, -1, 0, 0);
    ZerkList->m_list->sub_64F8A0(hwan2, 0, 0xffffff, 0xffffff, -1, 0, 0);
    //m_IRM.GetResObj<CIFPopupList>(48, 1)->m_list->sub_64F8A0(hwan3, 0, 0xffffff, 0xffffff, -1, 0, 0);

    ZerkList->m_text->SetText(hwan1.c_str());

    AutoHuntSetting.insert(std::make_pair(ZERK_SETTING, eZerkSetting::All));


    std::n_wstring sec1 = L"Use Res. Scroll";
    std::n_wstring sec2 = L"Resurrect in town";
    std::n_wstring sec3 = L"Wait for resurrection";


    TownList = m_IRM.GetResObj<CIFPopupList>(50, 1);
    TownList->m_list->sub_64F8A0(sec1, 0, 0xffffff, 0xffffff, -1, 0, 0);
    TownList->m_list->sub_64F8A0(sec2, 0, 0xffffff, 0xffffff, -1, 0, 0);
    TownList->m_list->sub_64F8A0(sec3, 0, 0xffffff, 0xffffff, -1, 0, 0);
    TownList->m_text->SetText(sec1.c_str());

    AutoHuntSetting.insert(std::make_pair(RETURN_TOWN_SETTING, eTownSetting::RES_SCROLL));



    std::n_wstring ret1 = L"In 1 hour";
    std::n_wstring ret2 = L"In 3 hours";
    std::n_wstring ret3 = L"In 5 hours";
    std::n_wstring ret4 = L"In 10 hours";
    std::n_wstring ret5 = L"In 24 hours";


    HourList = m_IRM.GetResObj<CIFPopupList>(52, 1);
    HourList->m_list->sub_64F8A0(ret1, 0, 0xffffff, 0xffffff, -1, 0, 0);
    HourList->m_list->sub_64F8A0(ret2, 0, 0xffffff, 0xffffff, -1, 0, 0);
    HourList->m_list->sub_64F8A0(ret3, 0, 0xffffff, 0xffffff, -1, 0, 0);
    HourList->m_list->sub_64F8A0(ret4, 0, 0xffffff, 0xffffff, -1, 0, 0);
    HourList->m_list->sub_64F8A0(ret5, 0, 0xffffff, 0xffffff, -1, 0, 0);

    HourList->m_text->SetText(ret1.c_str());


    AutoHuntSetting.insert(std::make_pair(BACK_HOUR_SETTING, eBackTownHour::HOUR_1));


    std::n_wstring rep1 = L"Use Repair Hammer";
    std::n_wstring rep2 = L"Return to town";


    RepairList = m_IRM.GetResObj<CIFPopupList>(54, 1);
    RepairList->m_list->sub_64F8A0(rep1, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RepairList->m_list->sub_64F8A0(rep2, 0, 0xffffff, 0xffffff, -1, 0, 0);
    RepairList->m_text->SetText(rep1.c_str());
    AutoHuntSetting.insert(std::make_pair(REPAIR_SETTING, eRepairSetting::USE_REPAIR_HAMMER));


    m_IRM.GetResObj<CIFPopupList>(54, 1)->BringToFront();
    m_IRM.GetResObj<CIFPopupList>(52, 1)->BringToFront();
    m_IRM.GetResObj<CIFPopupList>(50, 1)->BringToFront();
    m_IRM.GetResObj<CIFPopupList>(48, 1)->BringToFront();
    m_IRM.GetResObj<CIFPopupList>(46, 1)->BringToFront();
    //m_IRM.GetResObj<CIFPopupList>(44, 1)->BringToFront();


    m_IRM.GetResObj<CIFCheckBox>(56, 1)->SetCheckBoxState(true);
    m_IRM.GetResObj<CIFCheckBox>(58, 1)->SetCheckBoxState(false);
    m_IRM.GetResObj<CIFCheckBox>(60, 1)->SetCheckBoxState(false);
    DONT_ATTACK_MONSTERCB = m_IRM.GetResObj<CIFCheckBox>(44, 1);
    DONT_ATTACK_MONSTERCB->SetCheckBoxState(false);




    GDR_AUTO_REFORM_TITLE = m_IRM.GetResObj<CIFEdit>(98, 1);


    GDR_AUTO_REFORM_TITLE->SetMaxLength(50);
    wnd_size tt =  GDR_AUTO_REFORM_TITLE->GetSize();

    GDR_AUTO_REFORM_TITLE->SetText(L"For open hunting on the Silkroad!");
    GDR_AUTO_REFORM_TITLE->SetTextmode(tt.width);

    GDR_AUTO_REFORM_PT_LVL_MIN = m_IRM.GetResObj<CIFEdit>(100, 1);


    GDR_AUTO_REFORM_PT_LVL_MIN->SetMaxLength(3);
    wnd_size ttx =  GDR_AUTO_REFORM_PT_LVL_MIN->GetSize();

    GDR_AUTO_REFORM_PT_LVL_MIN->SetText(L"1");
    GDR_AUTO_REFORM_PT_LVL_MIN->SetTextmode(ttx.width);
    GDR_AUTO_REFORM_PT_LVL_MIN->AddValue_404(4);
    GDR_AUTO_REFORM_PT_LVL_MIN->SetValue_404(1);


    GDR_AUTO_REFORM_PT_LVL_MAX = m_IRM.GetResObj<CIFEdit>(102, 1);



    GDR_AUTO_REFORM_PT_LVL_MAX->SetMaxLength(3);
    wnd_size ttxx =  GDR_AUTO_REFORM_PT_LVL_MAX->GetSize();

    std::wstringstream ss;
    ss << m_Settings->ServerMaxLevel;
    std::wstring serverMaxLevelStr = ss.str();

    GDR_AUTO_REFORM_PT_LVL_MAX->SetText(serverMaxLevelStr.c_str());
    GDR_AUTO_REFORM_PT_LVL_MAX->SetTextmode(ttxx.width);
    GDR_AUTO_REFORM_PT_LVL_MAX->AddValue_404(4);
    GDR_AUTO_REFORM_PT_LVL_MAX->SetValue_404(1);

    m_IRM.GetResObj(103, 1)->SetText(L"~");
    sz.pos.x = 27;
    sz.pos.y = 167;
    sz.size.width = 238;
    sz.size.height = 24;



    AutoPartyInviteCheckBox = m_IRM.GetResObj<CIFCheckBox>(75, 1);
    AutoPartyInviteCheckBox->SetCheckBoxState(false);

    AutoPartyAcceptCheckBox =  m_IRM.GetResObj<CIFCheckBox>(78, 1);
    AutoPartyAcceptCheckBox->SetCheckBoxState(false);


    AutoPartyOnlyFromList = m_IRM.GetResObj<CIFCheckBox>(81, 1);
    AutoPartyOnlyFromList->SetCheckBoxState(true);

    GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX = m_IRM.GetResObj<CIFCheckBox>(87, 1);
    GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
    GDR_PT_EXP_AUTO_ITEM_NOCheckBox = m_IRM.GetResObj<CIFCheckBox>(84, 1);

    GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox = m_IRM.GetResObj<CIFCheckBox>(90, 1);
    GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);


    GDR_PT_EXP_FREE_ITEM_FREECheckBox = m_IRM.GetResObj<CIFCheckBox>(93, 1);
    GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);


    GDR_ACCEPT_JOIN_PT_REQ = m_IRM.GetResObj<CIFCheckBox>(96, 1);
    GDR_ACCEPT_JOIN_PT_REQ->SetCheckBoxState(false);

    GDR_AUTO_REFORM_PARTY = m_IRM.GetResObj<CIFCheckBox>(106, 1);
    GDR_AUTO_REFORM_PARTY->SetCheckBoxState(false);

    AutoResPtMembersCB = m_IRM.GetResObj<CIFCheckBox>(108, 1);
    AutoResPtMembersCB->SetCheckBoxState(false);


    sz.pos.x = 353;
    sz.pos.y = 348;
    sz.size.width = 180;
    sz.size.height = 24;
    m_popup = m_IRM.GetResObj<CIFPopupList2>(70, 1);


    m_popup->m_text->SetGWndSize(230, 24);
    m_popup->m_bg->SetGWndSize(230, 24);


    m_popup->m_btn->SetUniqueID(99999);

    for (int i = 0; i < 7; ++i)
    {
        m_PartySlot[i] = m_IRM.GetResObj<CIFMacroMenuAutoHuntPartySlot>(62+i, 1);
        m_PartySlot[i]->SetBarSize(238, 24);
    }

    //LoadSetting();
    return true;
}

void CIFMacroMenuAutoHunt::OnUpdate() {

    /* std::n_wstring gettext = m_IRM.GetResObj<CIFTextBox>(100, 1)->GetNText();


     std::string str(gettext.begin(), gettext.end());

     // std::string'i int'e dönüştür
     std::istringstream iss(str);
     int result;
     if((iss >> result))
     {
         if(result > m_Settings->ServerMaxLevel)
         {
             std::wstringstream ss;
             ss << m_Settings->ServerMaxLevel;
             std::wstring serverMaxLevelStr = ss.str();

            m_IRM.GetResObj<CIFTextBox>(100, 1)->SetText(serverMaxLevelStr.c_str());
         }
         if(result == 0)
         {
             m_IRM.GetResObj<CIFTextBox>(100, 1)->SetText(L"1");
         }
     }
 */
    std::n_wstring gettext2 = GDR_AUTO_REFORM_PT_LVL_MAX->GetNText();


    std::string str2(gettext2.begin(), gettext2.end());

    // std::string'i int'e dönüştür
    std::istringstream isss(str2);
    int result2;
    if((isss >> result2))
    {
        if(result2 > m_Settings->ServerMaxLevel)
        {
            std::wstringstream ss;
            ss << m_Settings->ServerMaxLevel;
            std::wstring serverMaxLevelStr = ss.str();

            GDR_AUTO_REFORM_PT_LVL_MAX->SetText(serverMaxLevelStr.c_str());
        }
        if(result2 == 0)
        {
            GDR_AUTO_REFORM_PT_LVL_MAX->SetText(L"1");
        }
    }
}

void CIFMacroMenuAutoHunt::OnUnknownStuff() {
    int id = GetCurrentEventMsgCtrlId();
    int i = 0;
    //printf("%id %d\n", id);
    for (int i = 0; i < numberOfTabs; ++i) {
        if (id == m_pTabsSecond[i]->UniqueID()) {
            ActivateTabPage(i);
            return;
        }
    }
}
void CIFMacroMenuAutoHunt::ActivateTabPage(BYTE page) {
    for (int i = 0; i < numberOfTabs; i++) {
        if (i == page)
            continue;

        m_pTabsSecond[i]->sub_64CC30(0);
        m_pTabsSecond[i]->m_FontTexture.sub_8B4750(2);
    }


    m_pTabsSecond[page]->sub_64CC30(1);
    switch (page)
    {
        case 0://title
        {
            m_IRM.GetResObj(107, 1)->ShowGWnd(true);
            AutoResPtMembersCB->ShowGWnd(true);

            for(int is = 31; is < 61; is++)
            {
                this->m_IRM.GetResObj(is, 1)->ShowGWnd(true);
            }

            for(int is = 61; is < 107; is++)
            {
                this->m_IRM.GetResObj(is, 1)->ShowGWnd(false);
            }
            for (int i = 0; i < 7; ++i)
            {
                //m_PartySlot[i]->ShowGWnd(false);
            }
            ClearPartySlotDDJ();
            this->m_IRM.GetResObj(69, 1)->ShowGWnd(false);
        }
            break;
        case 1://name tag
        {
            m_IRM.GetResObj(107, 1)->ShowGWnd(false);
            AutoResPtMembersCB->ShowGWnd(false);

            for(int is = 31; is < 61; is++)
            {
                m_IRM.GetResObj(is, 1)->ShowGWnd(false);
            }

            for(int is = 61; is < 107; is++)
            {
                this->m_IRM.GetResObj(is, 1)->ShowGWnd(true);
            }
            for (int i = 0; i < 7; ++i)
            {
                //m_PartySlot[i]->ShowGWnd(true);
            }
            ClearPartySlotDDJ();
            this->m_IRM.GetResObj(69, 1)->ShowGWnd(false);
        }
            break;
    }
}
void CIFMacroMenuAutoHunt::ClearPartySlotDDJ()
{
    for (int i = 0; i < 7; ++i)
    {
        m_PartySlot[i]->ClearDDJ();
    }
    SelectedPartyMemberName = L"";
}

void CIFMacroMenuAutoHunt::UpdateMenuSize()
{
    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2 - 50) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void CIFMacroMenuAutoHunt::BackTown()
{
    if (!g_pMyPlayerObj)
        return;

    if(MacroAutoInviteRunning)
    {
        MacroAutoInviteRunning = false;
        g_pCGInterface->KillTimer(STARTED_INVITE_PLAYER_PARTY);
    }
    if(MacroAutoTownTimerRunning)
    {
        MacroAutoTownTimerRunning = false;
        g_pCGInterface->KillTimer(START_BACK_TOWN);

        if(g_pMyPlayerObj->GetCurrentHp() == 0)
        {
            CMsgStreamBuffer buf(0x3053);
            buf << (byte)1;
            SendMsg(buf);
        }
        else
        {
            for(std::n_vector<CIFSlotWithHelp*>::iterator it = g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.begin(); it != g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.end(); it++)
            {
                if((*it)->ItemInfo != NULL)
                {

                    if((*it)->ItemInfo->GetItemData()->IsReturnScroll())
                    {
                        if((*it)->ItemInfo->GetItemData()->Country == 3)
                        {

                            (*it)->UseItem();
                            break;
                        }

                    }
                }
            }
        }
    }
}
void CIFMacroMenuAutoHunt::AutoReformParty()
{
    if (!g_pMyPlayerObj)
        return;
    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead ||g_pMyPlayerObj->CHARACTER_STATUS == Stall)
        return;
    if(g_CCharacterDependentData.GetRegisteredPartyID() == 0)
    {
        const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
        if (partyData.NumberOfMembers <= 8)
        {

            byte Action = 0;
            if(GDR_PT_EXP_FREE_ITEM_FREECheckBox->GetCheckedState_MAYBE())
            {
                Action = 0x4; // can ivite true // cant invite 0
            }
            else  if(GDR_PT_EXP_AUTO_ITEM_NOCheckBox->GetCheckedState_MAYBE())
            {
                Action = 0x5; // can invite // cant invite 1
            }
            else  if(GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->GetCheckedState_MAYBE())
            {
                Action = 0x6; // can invite cant invite // 2
            }
            else  if(GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->GetCheckedState_MAYBE())
            {
                Action = 0x7; // can invite cant invite // 3
            }
            if(Action != 0)
            {
                CMsgStreamBuffer buf(0x7069);
                buf << (unsigned int)0;
                buf << (unsigned int)0;
                buf << Action;
                byte Purpose = 0;

                if(g_pMyPlayerObj->GetJobType() == 4)
                {
                    Purpose = 0;
                }
                else if(g_pMyPlayerObj->GetJobType() == 3 || g_pMyPlayerObj->GetJobType() == 1)// hunter trader
                {
                    Purpose = 2;
                }
                else if(g_pMyPlayerObj->GetJobType() == 2) // thief
                {
                    Purpose = 3;
                }
                buf << Purpose;



                std::wstring test1 = GDR_AUTO_REFORM_PT_LVL_MIN->GetNText().c_str();
                std::string test = TO_STRING(test1).c_str();

                byte ConvertedInteger = atoi(test.c_str());

                buf << ConvertedInteger;

                std::wstring test12 = GDR_AUTO_REFORM_PT_LVL_MAX->GetNText().c_str();
                std::string test2 = TO_STRING(test12).c_str();

                byte MaxLevel = atoi(test2.c_str());

                buf << MaxLevel;

                std::n_wstring Title = GDR_AUTO_REFORM_TITLE->GetNText();

                std::string x = TO_STRING(Title);

                buf << x;

                SendMsg(buf);

            }
        }
    }
}
void CIFMacroMenuAutoHunt::InviteNearPartyMembers()
{
    if (!g_pMyPlayerObj)
        return;
    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead ||g_pMyPlayerObj->CHARACTER_STATUS == Stall)
        return;
    unsigned int FoundedUserUniqueID = 0;
    std::n_wstring FoundedCharName;
    std::map<int, CIObject*> entities = g_pGfxEttManager->entities;
    for (std::map<int, CIObject*>::iterator it = entities.begin(); it != entities.end(); ++it) {
        if (!strcmp(it->second->GetRuntimeClass()->m_lpszClassName, "CICUser")) {
            CICUser* nearmember = (CICUser*)it->second;
            if (nearmember) {
                if(nearmember->GetJobType() == g_pMyPlayerObj->GetJobType())
                {
                    if(AutoPartyOnlyFromList->GetCheckedState_MAYBE())
                    {
                        if(AutoPartyMemberList.find(nearmember->GetName()) != AutoPartyMemberList.end())
                        {
                            FoundedUserUniqueID = nearmember->GetUniqueId();
                            FoundedCharName = nearmember->GetName();
                            break;
                        }
                    }
                    else
                    {
                        if (nearmember->CHARACTER_STATUS != Dead)
                        {
                            FoundedUserUniqueID = nearmember->GetUniqueId();
                            FoundedCharName = nearmember->GetName();
                            break;
                        }
                    }
                }
            }
        }
    }
    bool isinparty = false;
    const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
    if (partyData.NumberOfMembers > 0)
    {
        for (int i = 0; i < partyData.NumberOfMembers; ++i) {
            const SPartyMemberData& memberData = g_CCharacterDependentData.GetPartyMemberData(i);
            if(memberData.m_charactername == FoundedCharName)
            {
                isinparty = true;
                break;
            }
        }
    }

    if(FoundedUserUniqueID != 0 && !isinparty)
    {

        byte Action = 0;
        if(GDR_PT_EXP_FREE_ITEM_FREECheckBox->GetCheckedState_MAYBE())
        {
            Action = 0x4;
        }
        else  if(GDR_PT_EXP_AUTO_ITEM_NOCheckBox->GetCheckedState_MAYBE())
        {
            Action = 0x5;
        }
        else  if(GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->GetCheckedState_MAYBE())
        {
            Action = 0x6;
        }
        else  if(GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->GetCheckedState_MAYBE())
        {
            Action = 0x7;
        }
        if(Action != 0)
        {
            CMsgStreamBuffer buf(0x7060);
            buf << FoundedUserUniqueID;
            buf << Action;
            SendMsg(buf);
        }
        // disable exp and item 0x5 == disable item active exp

    }
}
void CIFMacroMenuAutoHunt::CheckNasrun()
{
    if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT != NULL)
    {
        if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT->ItemInfo != NULL)
        {
            if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT->ItemInfo->GetItemData() != NULL)
            {
                if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd != NULL)
                {
                    if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1 != NULL)
                    {
                        if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1->m_pSlot != NULL)
                        {
                            if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1->m_pSlot->GetSkillSlotInDex() != 0)
                            {
                                int GetSkillID = g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1->m_pSlot->GetSkillSlotInDex();
                                if(GetSkillID != 0)
                                {
                                    float delay = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(GetSkillID);
                                    // printf("%f delay\n", delay);
                                    if (delay > 0.0)
                                        return;
                                    else
                                    {

//                                               g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1->m_pSlot->UseItemSlot();
                                        CMsgStreamBuffer buf(0x7074);
                                        byte i = 4;
                                        buf << (byte)1;
                                        buf << i;
                                        buf << GetSkillID;
                                        buf << (byte)0;
                                        SendMsg(buf);

                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }
}
void CIFMacroMenuAutoHunt::CheckNasrunIgnore()
{
    if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT != NULL)
    {
        if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT->ItemInfo != NULL)
        {
            if(g_pCGInterface->GetMainPopup()->GetEquipment()->DEVILSLOT->ItemInfo->GetItemData() != NULL)
            {
                if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd != NULL)
                {
                    if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot2 != NULL)
                    {
                        if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot2->m_pSlot != NULL)
                        {
                            if(g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot2->m_pSlot->GetSkillSlotInDex() != 0)
                            {
                                int GetSkillID = g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot2->m_pSlot->GetSkillSlotInDex();
                                if(GetSkillID != 0)
                                {
                                    float delay = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(GetSkillID);
                                    // printf("%f delay\n", delay);
                                    if (delay > 0.0)
                                        return;
                                    else
                                    {

//                                               g_pCGInterface->GetMainPopup()->GetEquipment()->pSkillSlotWnd->CIFSkillSlot1->m_pSlot->UseItemSlot();
                                        CMsgStreamBuffer buf(0x7074);
                                        byte i = 4;
                                        buf << (byte)1;
                                        buf << i;
                                        buf << GetSkillID;
                                        buf << (byte)0;
                                        SendMsg(buf);

                                    }
                                }
                            }
                        }
                    }
                }

            }
        }
    }
}
void CIFMacroMenuAutoHunt::StartAutoHunt()
{
    if(!AutoHuntTimerRunning)
    {
        AutoHuntTimerRunning = true;
        g_pCGInterface->StartTimer(START_AUTO_HUNT, 500);
    }
    if(!MacroAutoTownTimerRunning)
    {
        MacroAutoTownTimerRunning = true;
         g_pCGInterface->StartTimer(START_BACK_TOWN, 3600000 * AutoHuntSetting[BACK_HOUR_SETTING]);
    }
    if (!g_pMyPlayerObj)
        return;
    if(g_pMyPlayerObj->GetCurrentHp() == 0)
    {
       if(AutoHuntSetting[RETURN_TOWN_SETTING] == eTownSetting::RES_SCROLL) // Use Res Scroll
       {

           for(std::n_vector<CIFSlotWithHelp*>::iterator it = g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.begin(); it != g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.end(); it++)
           {
               if((*it)->ItemInfo != NULL)
               {
                   if((*it)->ItemInfo->GetItemData()->IsResurrectScroll())
                   {
                       (*it)->UseItem();
                       break;
                   }
               }
           }
       }
       else if(AutoHuntSetting[RETURN_TOWN_SETTING] == eTownSetting::GO_TOWN) // go town
       {
           CMsgStreamBuffer buf(0x3053);
           buf << (byte)1;
           SendMsg(buf);
       }
       else if(AutoHuntSetting[RETURN_TOWN_SETTING] == eTownSetting::WAIT_FOR_RES) // wait for ress
       {

       }
    }

    if(AutoPartyInviteCheckBox->GetCheckedState_MAYBE())
    {
        if(!MacroAutoInviteRunning)
        {
            MacroAutoInviteRunning = true;
            g_pCGInterface->StartTimer(STARTED_INVITE_PLAYER_PARTY, 5000);;
        }
    }

    if(GDR_AUTO_REFORM_PARTY->GetCheckedState_MAYBE())
    {
        AutoReformParty();
    }

    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead ||g_pMyPlayerObj->CHARACTER_STATUS == Stall
        || g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet == 17)
        return;


        if(AutoReturnHPMPLessCheckbox->GetCheckedState_MAYBE())
            {
                int HpItemID = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoPotionSlot->GetHPItemID();
                if(HpItemID != 0)
                {
                    int TotalHPPotion = 0;
                    for(std::n_vector<CIFSlotWithHelp*>::iterator it = g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.begin(); it != g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.end(); it++)
                    {
                        if((*it)->ItemInfo != NULL)
                        {
                            if((*it)->ItemInfo->GetItemData() != NULL)
                            {
                                if((*it)->ItemInfo->GetItemData()->RefObjectId == HpItemID)
                                {
                                    TotalHPPotion += (*it)->ItemInfo->GetQuantity();
                                    if(TotalHPPotion < 10)
                                    {
                                        BackTown();
                                        break;
                                    }
                                }
                            }
                        }
                    }

                }
                else
                {
                    BackTown();
                }
                int MpItemID = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoPotionSlot->GetMPItemID();
                if(MpItemID != 0)
                {
                    int TotalMPPotion = 0;
                    for(std::n_vector<CIFSlotWithHelp*>::iterator it = g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.begin(); it != g_pCGInterface->GetMainPopup()->GetInventory()->pSlots.end(); it++)
                    {
                        if((*it)->ItemInfo != NULL)
                        {
                            if((*it)->ItemInfo->GetItemData() != NULL)
                            {
                                if((*it)->ItemInfo->GetItemData()->RefObjectId == MpItemID)
                                {
                                    TotalMPPotion += (*it)->ItemInfo->GetQuantity();
                                    if(TotalMPPotion < 10)
                                    {
                                        BackTown();
                                        break;
                                    }
                                }
                            }

                        }
                    }

                }
                else
                {
                    BackTown();
                }
            }

        if(AutoHuntSetting[REPAIR_SETTING] == eRepairSetting::USE_REPAIR_HAMMER)
        {
            bool itemfound = false;
            for(int i = 0; i < 7; i++)
            {

                CSOItem * p = g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(i);
                if(p)
                {
                    if(p->GetItemData() != NULL)
                    {
                        if(p->m_CurrDurability < 5)
                        {
                            itemfound = true;
                            break;
                        }
                    }
                }
            }
            CSOItem * p = g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(7);
            if(p != NULL)
            {
                if(p->GetItemData() != NULL)
                {
                    if(p->GetItemData()->m_typeId.getTypeID1() == 3 && p->GetItemData()->m_typeId.getTypeID2() == 1 && p->GetItemData()->m_typeId.getTypeID3() == 4)
                    {
                        if(p->m_CurrDurability < 5)
                        {
                            itemfound = true;
                        }
                    }
                }
            }
            if(itemfound)
            {
                CIFInventory *Inventory = g_pCGInterface->GetMainPopup()->GetInventory();
                int slotcnt = Inventory->InventorySlotCount();
                for(int i = 0; i <= slotcnt; i++)
                {
                    if(Inventory->pSlots[i]->ItemInfo != NULL)
                    {
                        if(Inventory->pSlots[i]->ItemInfo->GetItemData() != NULL)
                        {
                            if(Inventory->pSlots[i]->ItemInfo->GetItemData()->IsRepairHammer())
                            {
                                CMsgStreamBuffer buf(0x704C);
                                UINT16 type = Inventory->pSlots[i]->ItemInfo->GetItemData()->m_typeId.m_type_id_value;
                                byte slot = i + 13;
                                buf << slot;
                                buf << type;
                                //printf("%d \n", type);
                                SendMsg(buf);
                                break;
                            }
                        }
                    }
                }

            }
        }
        else if(AutoHuntSetting[REPAIR_SETTING] == eRepairSetting::RETURN_TOWN)
        {
            for(int i = 0; i < 7; i++)
            {
                CSOItem * p = g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(i);
                if(p  != NULL)
                {
                    if(p->GetItemData() != NULL)
                    {
                        if(p->m_CurrDurability < 5)
                        {
                            BackTown();
                            break;
                        }
                    }
                }
            }
            CSOItem * p = g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(7);
            if(p  != NULL)
            {
                if(p->GetItemData() != NULL)
                {
                    if(p->GetItemData()->m_typeId.getTypeID1() == 3 && p->GetItemData()->m_typeId.getTypeID2() == 1 && p->GetItemData()->m_typeId.getTypeID3() == 4)
                    {
                        if(p->m_CurrDurability < 5)
                        {
                            BackTown();
                        }
                    }
                }

            }


        }



        if(AutoUseNasrun->GetCheckedState_MAYBE())
        {
            CheckNasrun();
        }


        if(AutoUseNasrunIgnore->GetCheckedState_MAYBE())
            {
                CheckNasrunIgnore();
            }


}
uregion CIFMacroMenuAutoHunt::GetRegion() const {
    return StartRegion;
}

D3DVECTOR CIFMacroMenuAutoHunt::GetLocation() const {
    return StartPosition;
}

void CIFMacroMenuAutoHunt::SaveButton(){

    std::n_wstring Range = m_IRM.GetResObj<CIFPopupList>(46, 1)->m_text->GetNText().c_str();
    int selectedrange = 0;
    if(Range.length() > 0)
    {
        std::n_wstring area1 = L"30 m";
        std::n_wstring area2 = L"50 m";
        std::n_wstring area3 = L"70 m";
        std::n_wstring area4 = L"100 m";
        std::n_wstring area5 = L"150 m";
        //std::n_wstring area6 = L"200 m";
        //std::n_wstring area7 = L"300 m";

        if(Range == area1)
        {
            selectedrange = 30;
        }
        else if(Range == area2)
        {
            selectedrange = 50;
        }
        else if( Range == area3)
        {
            selectedrange = 70;
        }
        else if( Range == area4)
        {
            selectedrange = 100;
        }
        else if( Range == area5)
        {
            selectedrange = 150;
        }
        AutoHuntSetting[RADUIS_SETTING] = selectedrange;
    }

    std::n_wstring hwan = m_IRM.GetResObj<CIFPopupList>(48, 1)->m_text->GetNText().c_str();
    int selectedhwan = 0;
    if(hwan.length() > 0)
    {

        std::n_wstring hwan1 = L"All";
        std::n_wstring hwan2 = L"Giant";

        if(hwan == hwan1)
        {
            selectedhwan = 1;
        }
        else if(hwan == hwan2)
        {
            selectedhwan = 2;
        }
        /* else if(hwan == hwan3)
         {
             selectedhwan = 3;
         }*/
        AutoHuntSetting[ZERK_SETTING] = selectedhwan;
    }

    std::n_wstring res = m_IRM.GetResObj<CIFPopupList>(50, 1)->m_text->GetNText().c_str();
    int selectedres = 0;
    if(res.length() > 0)
    {
        std::n_wstring sec1 = L"Use Res. Scroll";
        std::n_wstring sec2 = L"Resurrect in town";
        std::n_wstring sec3 = L"Wait for resurrection";
        if(res == sec1)
        {
            selectedres = 1;
        }
        else if(res == sec2)
        {
            selectedres = 2;
        }
        else if(res == sec3)
        {
            selectedres = 3;
        }
        AutoHuntSetting[RETURN_TOWN_SETTING] = selectedres;
    }

    std::n_wstring returne = m_IRM.GetResObj<CIFPopupList>(52, 1)->m_text->GetNText().c_str();
    int selectedreturn = 0;
    if(returne.length() > 0)
    {
        std::n_wstring ret1 = L"In 1 hour";
        std::n_wstring ret2 = L"In 3 hours";
        std::n_wstring ret3 = L"In 5 hours";
        std::n_wstring ret4 = L"In 10 hours";
        std::n_wstring ret5 = L"In 24 hours";
        if(returne == ret1)
        {
            selectedreturn = 1;
        }
        else if(returne == ret2)
        {
            selectedreturn = 2;
        }
        else if(returne == ret3)
        {
            selectedreturn = 3;
        }
        else if(returne == ret4)
        {
            selectedreturn = 4;
        }
        AutoHuntSetting[BACK_HOUR_SETTING] = selectedreturn;
    }

    std::n_wstring repair = m_IRM.GetResObj<CIFPopupList>(54, 1)->m_text->GetNText().c_str();
    int selectedrepair = 0;
    if(repair.length() > 0)
    {

        std::n_wstring rep1 = L"Use Repair Hammer";
        std::n_wstring rep2 = L"Return to town";

        if(repair == rep1)
        {
            selectedrepair = 1;
        }
        else if(repair == rep2)
        {
            selectedrepair = 2;
        }
        AutoHuntSetting[REPAIR_SETTING] = selectedrepair;
    }

    char buffer3[0x200];
    sprintf(buffer3, "%s\\Setting\\%ls_Macro.txt", theApp.GetWorkingDir(), g_pMyPlayerObj->GetCharName().c_str());

// Dosyayı yazma modunda aç
    FILE *file3 = fopen(buffer3, "w");
    if (file3 != NULL) {
        // Veri kontrolü ve dosyaya yazma

        fprintf(file3, "Dont attack the monsters: %d\n", DONT_ATTACK_MONSTERCB->GetCheckedState_MAYBE());
        fprintf(file3, "Radius: %d\n", AutoHuntSetting[RADUIS_SETTING]);
        fprintf(file3, "Hwan setting: %d\n", AutoHuntSetting[ZERK_SETTING]);
        fprintf(file3, "Return town setting: %d\n", AutoHuntSetting[RETURN_TOWN_SETTING]);
        fprintf(file3, "Back town setting: %d\n", AutoHuntSetting[BACK_HOUR_SETTING]);
        fprintf(file3, "Repair setting: %d\n", AutoHuntSetting[REPAIR_SETTING]);
        fprintf(file3, "Back town when potion less: %d\n", AutoReturnHPMPLessCheckbox->GetCheckedState_MAYBE());
        fprintf(file3, "Nasrun setting: %d\n", AutoUseNasrun->GetCheckedState_MAYBE());
        fprintf(file3, "Ignore skill setting: %d\n", AutoUseNasrunIgnore->GetCheckedState_MAYBE());

        fprintf(file3, "Invite to Party: %d\n", AutoPartyInviteCheckBox->GetCheckedState_MAYBE());
        fprintf(file3, "Accept party invite: %d\n", AutoPartyAcceptCheckBox->GetCheckedState_MAYBE());
        fprintf(file3, "Only from list: %d\n", AutoPartyOnlyFromList->GetCheckedState_MAYBE());
        fprintf(file3, "Accept join party requests: %d\n", GDR_ACCEPT_JOIN_PT_REQ->GetCheckedState_MAYBE());
        fprintf(file3, "Auto reform party: %d\n",GDR_AUTO_REFORM_PARTY->GetCheckedState_MAYBE());

        fprintf(file3, "Resurrect party members: %d\n", AutoResPtMembersCB->GetCheckedState_MAYBE());


        std::n_wstring Title = GDR_AUTO_REFORM_TITLE->GetNText();
        fprintf(file3, "Auto reform party title: %ls\n", Title.c_str());


        fprintf(file3, "Auto reform min level: %ls\n",GDR_AUTO_REFORM_PT_LVL_MIN->GetNText().c_str());
        fprintf(file3, "Auto reform max level: %ls\n", GDR_AUTO_REFORM_PT_LVL_MAX->GetNText().c_str());

        if(GDR_PT_EXP_AUTO_ITEM_NOCheckBox->GetCheckedState_MAYBE())
        {
            fprintf(file3, "Auto party setting: %d\n", 84);
        }
        else if(GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->GetCheckedState_MAYBE())
        {
            fprintf(file3, "Auto party setting: %d\n", 87);
        }
        else if(GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->GetCheckedState_MAYBE())
        {
            fprintf(file3, "Auto party setting: %d\n", 90);
        }
        else if(GDR_PT_EXP_FREE_ITEM_FREECheckBox->GetCheckedState_MAYBE())
        {
            fprintf(file3, "Auto party setting: %d\n", 93);
        }
        int i = 0;
        if(AutoPartyMemberList.size() > 0)
        {
            for(std::map<std::n_wstring, std::n_wstring>::iterator it = AutoPartyMemberList.begin();
                it != AutoPartyMemberList.end(); it++)
            {
                i++;
                fprintf(file3, "Auto party member %d: %ls\n",i, it->first.c_str());
            }
        }
        fclose(file3); // Dosyayı kapat
        //printf("Veri dosyaya başarıyla yazıldı: %s", buffer3);
    }

}
undefined1 CIFMacroMenuAutoHunt::OnCloseWnd(){
    return CIFWnd::OnCloseWnd();
}

void CIFMacroMenuAutoHunt::CancelBtn(){
    g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(1355, 1)->ShowGWnd(false);
}
void CIFMacroMenuAutoHunt::LoadSetting(){

}

void CIFMacroMenuAutoHunt::PartyExpItemSetting()
{
    int id = GetCurrentEventMsgCtrlId();
    if(id == 84)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(true);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);

    }
    else if(id == 87)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(true);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);
    }
    else if(id == 90)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(true);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);
    }
    else if(id == 93)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(true);
    }
}
void CIFMacroMenuAutoHunt::LoadPartyExpItemSetting(int id)
{
    if(id == 84)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(true);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);

    }
    else if(id == 87)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(true);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);
    }
    else if(id == 90)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(true);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(false);
    }
    else if(id == 93)
    {
        GDR_PT_EXP_AUTO_ITEM_NOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX->SetCheckBoxState(false);
        GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox->SetCheckBoxState(false);
        GDR_PT_EXP_FREE_ITEM_FREECheckBox->SetCheckBoxState(true);
    }
}