#include "IFMacroMenuAutoSkill.h"
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
#include <SRIFLib/NInterfaceResource.h>
#include <SRIFLib/NIFLattice.h>
#include <ICMonster.h>
#include <algorithm>
#include <EntityManagerClient.h>
#include <PartyData.h>
#include <CharacterDependentData.h>
#include <IFPlayerMiniInfo.h>
#include <iostream>
#include <NavMesh/LocationInfo.h>
#include <NavMesh/IRegionManager.h>
#include <cmath>
#include <ctime>
#include <sstream>
#include "IFMacro.h"
#define GDR_SAVE_BUTTON 29
#define GDR_CANCEL_BUTTON 30

#define GDR_SKILL_WEAPON 153
#define GDR_BUFF_WEAPON 154

#define StartOf
GFX_IMPLEMENT_DYNCREATE(CIFMacroMenuAutoSkill, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFMacroMenuAutoSkill, CIFWnd)
                    ONG_COMMAND(300, &CIFMacroMenuAutoSkill::OnUnknownStuff)
                    ONG_COMMAND(301, &CIFMacroMenuAutoSkill::OnUnknownStuff)
                    ONG_COMMAND(302, &CIFMacroMenuAutoSkill::OnUnknownStuff)
                    ONG_COMMAND(GDR_SAVE_BUTTON, &CIFMacroMenuAutoSkill::SaveButton)
                    ONG_COMMAND(GDR_CANCEL_BUTTON, &CIFMacroMenuAutoSkill::CancelButton)
                    ONG_COMMAND(903, &CIFMacroMenuAutoSkill::AddPtMemberBtn)
                    ONG_COMMAND(904, &CIFMacroMenuAutoSkill::RemovePtMemberBtn)
                    ONG_COMMAND(99999, &CIFMacroMenuAutoSkill::LoadPartyMembers)
GFX_END_MESSAGE_MAP()

CIFMacroMenuAutoSkill::CIFMacroMenuAutoSkill(void) {
    BS_DEBUG_LOW(">" __FUNCTION__);
    PartyBuffList = std::map<std::n_wstring, std::vector<int>>();
    SelectedPartyMemberName = std::n_wstring();
    Macro_AutoSkill = false;
    AutoSkillTimerRunning = false;
    MyLastResUseTime = time(NULL);
    ActiveTabPage = -1;
    replace = -1;
}
CIFMacroMenuAutoSkill::~CIFMacroMenuAutoSkill(void) {
    if (m_pTabsSecond) {
        free(m_pTabsSecond);
        m_pTabsSecond = 0;
    }
    BS_DEBUG_LOW(">" __FUNCTION__);
}
void CIFMacroMenuAutoSkill::AddPtMemberBtn(){

    std::n_wstring charname = m_popup->m_text->GetNText();
    if(charname.empty())
    {
        g_pCGInterface->ShowMessage_Notice(L"Enter the character name.");
        return;
    }
    if(PartyBuffList.size() < 8)
    {
        if(PartyBuffList.find(charname) == PartyBuffList.end())
        {
            std::vector<int> p;
            PartyBuffList.insert(std::make_pair(charname, p));
            for (int i = 0; i < 18; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    m_slotviewer[i]->slot1[j]->slot->N595 = 2;
                }
            }
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
    for(std::map<std::n_wstring, std::vector<int>>::iterator  it = PartyBuffList.begin(); it != PartyBuffList.end(); it++)
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
void CIFMacroMenuAutoSkill::RemovePtMemberBtn(){
    if(!SelectedPartyMemberName.empty())
    {
        if(PartyBuffList.find(SelectedPartyMemberName) != PartyBuffList.end())
        {
            PartyBuffList.erase(SelectedPartyMemberName);
            ClearPartySlotDDJ();
            for (int i = 0; i < 18; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    m_slotviewer[i]->slot1[j]->slot->N595 = 2;
                }
            }
            for(int i = 0; i < 7; i++)
            {
                m_PartySlot[i]->LoadItems(L"");
            }

            int i = 0;
            for(std::map<std::n_wstring , std::vector<int>>::iterator  it = PartyBuffList.begin(); it != PartyBuffList.end(); it++)
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

void CIFMacroMenuAutoSkill::SelectedPartyBuffs()
{
    if(!SelectedPartyMemberName.empty())
    {
        if(PartyBuffList.find(SelectedPartyMemberName) != PartyBuffList.end())
        {
            for (int i = 0; i < 18; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    m_slotviewer[i]->slot1[j]->slot->N595 = 2;
                }
            }
            std::vector<int>& skillList = PartyBuffList[SelectedPartyMemberName];

            for (int i = 0; i < 18; ++i)
            {
                for(int j = 0; j < 8; ++j)
                {
                    std::vector<int>::iterator skillIt = std::find(skillList.begin(), skillList.end(), m_slotviewer[i]->slot1[j]->slot->m_SkillID);
                    if (skillIt != skillList.end()) {
                        // m_SkillID varsa sil
                        m_slotviewer[i]->slot1[j]->slot->N595 = 0;
                    }
                }
            }
        }

    }

}

bool CIFMacroMenuAutoSkill::OnCreate(long ln) {

    // Populate inherited members
    CIFWnd::OnCreate(ln);
    wnd_rect sz;
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifmacromenuautoskill.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pHandleBar->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pCloseBtn->ShowGWnd(false);
    // this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pTitleText->m_FontTexture.SetColor(D3DCOLOR_ARGB(255,239,218,164));
    this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pTitleText->MoveGWnd(this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pTitleText->GetPos().x, this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pTitleText->GetPos().y - 8);

    this->m_IRM.GetResObj<CIFMainFrame>(4, 1)->m_pHandleBar->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFMainFrame>(4, 1)->m_pCloseBtn->ShowGWnd(false);
    // this->m_IRM.GetResObj<CIFMainFrame>(3, 1)->m_pTitleText->m_FontTexture.SetColor(D3DCOLOR_ARGB(255,239,218,164));
    this->m_IRM.GetResObj<CIFMainFrame>(4, 1)->m_pTitleText->MoveGWnd(this->m_IRM.GetResObj<CIFMainFrame>(4, 1)->m_pTitleText->GetPos().x, this->m_IRM.GetResObj<CIFMainFrame>(4, 1)->m_pTitleText->GetPos().y - 8);

    m_IRM.GetResObj(30, 1)->SetText(L"Cancel");

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
                m_pTabsSecond[0]->SetText(L"Configure Attack Skills");
            }
                break;
            case 1:
            {
                m_pTabsSecond[1]->MoveGWnd(m_pTabsSecond[0]->GetPos().x + tabWidth + 10, m_pTabsSecond[0]->GetPos().y);
                m_pTabsSecond[1]->SetText(L"Configure Buff Skills");
                m_pTabsSecond[1]->SetClickable(true);
            }
                break;
            case 2:
            {
                m_pTabsSecond[2]->MoveGWnd(m_pTabsSecond[1]->GetPos().x + tabWidth + 10, m_pTabsSecond[1]->GetPos().y);
                m_pTabsSecond[2]->SetText(L"Configure Party Buff Skills");
                m_pTabsSecond[2]->SetClickable(true);

            }
        }
        m_pTabsSecond[i]->sub_64CC30(0);


    }

    sz.pos.x = 25;
    sz.pos.y = 186;
    sz.size.width = 303;
    sz.size.height = 187;
    CNIFLattice * p = (CNIFLattice*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CNIFLattice),
                                                          sz, 1000, 0);
    p->NTB_Func_13("interface\\ifcommon\\lattice_window\\com_lattice_", 0, 0);


    sz.pos.x = 353;
    sz.pos.y = 252;
    sz.size.width = 303;
    sz.size.height = 121;
    ps = (CNIFLattice*)CNIFWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CNIFLattice),
                                               sz, 1001, 0);

    ps->NTB_Func_13("interface\\ifcommon\\lattice_window\\com_lattice_", 0, 0);

    this->m_IRM.GetResObj(3, 1)->SetText(L"Acquired Skills");
    this->m_IRM.GetResObj(4, 1)->SetText(L"Skills to Use");
    this->m_IRM.GetResObj(11, 1)->SetText(L"Weapon");
    this->m_IRM.GetResObj(13, 1)->SetText(L"Shield");
    this->m_IRM.GetResObj(29, 1)->SetText(L"Save");

    m_scroll_skill_list = this->m_IRM.GetResObj<CIFScrollManager>(7, 1);
    m_scroll_skill_list->sub_008124F0(0);
    m_scroll_skill_list->sub_008124C0(36);
    m_scroll_skill_list->sub_008123F0(5);
    m_scroll_skill_list->sub_00812500(0);
    m_scroll_skill_list->sub_00812420(-8, 0);
    m_scroll_skill_list->BringToFront();

    sz.pos.x = 353;
    sz.pos.y = 348;
    sz.size.width = 180;
    sz.size.height = 24;
    m_popup = m_IRM.GetResObj<CIFPopupList2>(901, 1);


    m_popup->m_text->SetGWndSize(200, 24);
    m_popup->m_bg->SetGWndSize(200, 24);

    sz.pos.x = 0;
    sz.pos.y = 0;
    sz.size.width = 303;
    sz.size.height = 36;
    for(int i =0; i< 18;i++) {
        m_slotviewer[i] = (CIFMacroMenuAutoSkillSlotViewer*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFMacroMenuAutoSkillSlotViewer), sz, 301 + i, 0);
        m_slotviewer[i]->ShowGWnd(false);
    }

    for (int i = 0; i < 7; ++i)
    {
        m_PartySlot[i] = m_IRM.GetResObj<CIFMacroMenuAutoSkillPartySlot>(5000+i, 1);
    }

    SkillWeaponSlot = this->m_IRM.GetResObj<CIFMacroSlotWep>(GDR_SKILL_WEAPON, 1);
    SkillWeaponSlot->m_pMySlot->m_pSlot->SetSlot(153);
    SkillWeaponSlot->m_pMySlot->m_pSlot->SetType(0xC);

    BuffWeaponSlot = this->m_IRM.GetResObj<CIFMacroSlotWep>(GDR_BUFF_WEAPON, 1);
    BuffWeaponSlot->m_pMySlot->m_pSlot->SetSlot(154);
    BuffWeaponSlot->m_pMySlot->m_pSlot->SetType(0xC);

    SkillShieldSlot = this->m_IRM.GetResObj<CIFMacroSlotWep>(155, 1);
    SkillShieldSlot->m_pMySlot->m_pSlot->SetSlot(155);
    SkillShieldSlot->m_pMySlot->m_pSlot->SetType(0xC);

    BuffShieldSlot = this->m_IRM.GetResObj<CIFMacroSlotWep>(156, 1);
    BuffShieldSlot->m_pMySlot->m_pSlot->SetSlot(156);
    BuffShieldSlot->m_pMySlot->m_pSlot->SetType(0xC);


    sz.pos.x = 354;
    sz.pos.y = 253;
    sz.size.width = 32;
    sz.size.height = 32;

    int startX = sz.pos.x;
    int startY = sz.pos.y;

    int offsetX = 36;
    int offsetY = 36;

    for (int i = 0; i < 24; i++) {
        int row = i / 8;
        int col = i % 8;

        sz.pos.x = 354 + col * offsetX;
        sz.pos.y = 253 + row * offsetY;

        skillslots[i] = (CIFMacroSlotSkill*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFMacroSlotSkill), sz, 10005 + i, 0);
        skillslots[i]->m_pMySlot->m_pSlot->SetType(0xC);
        skillslots[i]->m_pMySlot->m_pSlot->SetSlot(157+i);
        if ((i + 1) % 8 == 0) {
            sz.pos.x = 354; // X koordinatını başa al
            sz.pos.y += offsetY; // Y koordinatını bir sonraki satıra taşı
        }
    }

    sz.pos.x = 354;
    sz.pos.y = 253;
    sz.size.width = 32;
    sz.size.height = 32;

    int startXx = sz.pos.x;
    int startYy = sz.pos.y;

    int offsetXx = 36;
    int offsetYy = 36;

    for (int i = 0; i < 24; i++) {
        int row = i / 8;
        int col = i % 8;

        sz.pos.x = 354 + col * offsetXx;
        sz.pos.y = 253 + row * offsetYy;


        buffslots[i] = (CIFMacroSlotSkill*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFMacroSlotSkill), sz, 10405 + i, 0);
        buffslots[i]->m_pMySlot->m_pSlot->SetType(0xC);
        buffslots[i]->m_pMySlot->m_pSlot->SetSlot(181+i);
        if ((i + 1) % 8 == 0) {
            sz.pos.x = 354; // X koordinatını başa al
            sz.pos.y += offsetYy; // Y koordinatını bir sonraki satıra taşı
        }
    }
    m_popup->m_btn->SetUniqueID(99999);

    return true;
}
void CIFMacroMenuAutoSkill::LoadPartyMembers()
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
void CIFMacroMenuAutoSkill::OnUpdate() {

}

void CIFMacroMenuAutoSkill::OnUnknownStuff() {
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
void CIFMacroMenuAutoSkill::LoadPartyBuffInfos()
{
    int i = 0;
    for(std::map<std::n_wstring, std::vector<int>>::iterator  it = PartyBuffList.begin(); it != PartyBuffList.end(); it++)
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
void CIFMacroMenuAutoSkill::ActivateTabPage(BYTE page) {
    for (int i = 0; i < numberOfTabs; i++) {
        if (i == page)
            continue;

        m_pTabsSecond[i]->sub_64CC30(0);
        m_pTabsSecond[i]->m_FontTexture.sub_8B4750(2);
    }
    ActiveTabPage = page;

    m_pTabsSecond[page]->sub_64CC30(1);
    switch (page)
    {
        case 0:
        {
            Clear();
            LoadSkills();
            for (int i = 0; i< 7; i++)
            {
                m_PartySlot[i]->ShowGWnd(false);
            }
            m_IRM.GetResObj(6, 1)->ShowGWnd(true);
            m_IRM.GetResObj(8, 1)->ShowGWnd(true);
            m_IRM.GetResObj(10, 1)->ShowGWnd(true);
            m_IRM.GetResObj(11, 1)->ShowGWnd(true);
            m_IRM.GetResObj(12, 1)->ShowGWnd(true);
            m_IRM.GetResObj(13, 1)->ShowGWnd(true);
            ps->SetGWndSize(303, 121);
            ps->ShowGWnd(true);
            m_IRM.GetResObj(901, 1)->ShowGWnd(false);

            m_IRM.GetResObj(903, 1)->ShowGWnd(false);
            m_IRM.GetResObj(904, 1)->ShowGWnd(false);
            m_IRM.GetResObj(900, 1)->ShowGWnd(false);

            SkillWeaponSlot->ShowGWnd(true);
            SkillWeaponSlot->BringToFront();
            SkillShieldSlot->ShowGWnd(true);
            SkillShieldSlot->BringToFront();

            BuffWeaponSlot->ShowGWnd(false);
            BuffShieldSlot->ShowGWnd(false);
            for (int i = 0; i < 24; i++)
            {
                buffslots[i]->ShowGWnd(false);
                skillslots[i]->ShowGWnd(true);
                skillslots[i]->BringToFront();
            }
        }
            break;
        case 1:
        {
            for (int i = 0; i < 7; i++)
            {
                m_PartySlot[i]->ShowGWnd(false);
            }
            Clear();
            LoadBuffs();
            m_IRM.GetResObj(6, 1)->ShowGWnd(true);
            m_IRM.GetResObj(8, 1)->ShowGWnd(true);
            m_IRM.GetResObj(10, 1)->ShowGWnd(true);
            m_IRM.GetResObj(11, 1)->ShowGWnd(true);
            m_IRM.GetResObj(12, 1)->ShowGWnd(true);
            m_IRM.GetResObj(13, 1)->ShowGWnd(true);
            ps->SetGWndSize(303, 121);
            ps->ShowGWnd(true);

            m_IRM.GetResObj(901, 1)->ShowGWnd(false);

            m_IRM.GetResObj(903, 1)->ShowGWnd(false);
            m_IRM.GetResObj(904, 1)->ShowGWnd(false);
            m_IRM.GetResObj(900, 1)->ShowGWnd(false);
            SkillWeaponSlot->ShowGWnd(false);
            SkillShieldSlot->ShowGWnd(false);

            BuffWeaponSlot->ShowGWnd(true);
            BuffShieldSlot->ShowGWnd(true);
            BuffShieldSlot->BringToFront();
            BuffWeaponSlot->BringToFront();

            for (int i = 0; i < 24; i++)
            {
                skillslots[i]->ShowGWnd(false);
                buffslots[i]->ShowGWnd(true);
                buffslots[i]->BringToFront();
            }


        }
            break;
        case 2:
        {
            Clear();
            LoadPartyBuffs();
            ClearPartySlotDDJ();
            LoadPartyBuffInfos();
            for (int i = 0; i< 7; i++)
            {
                m_PartySlot[i]->ShowGWnd(true);
            }
            m_IRM.GetResObj(6, 1)->ShowGWnd(false);
            m_IRM.GetResObj(8, 1)->ShowGWnd(false);
            m_IRM.GetResObj(10, 1)->ShowGWnd(false);
            m_IRM.GetResObj(11, 1)->ShowGWnd(false);
            m_IRM.GetResObj(12, 1)->ShowGWnd(false);
            m_IRM.GetResObj(13, 1)->ShowGWnd(false);
            m_IRM.GetResObj(901, 1)->ShowGWnd(true);

            m_IRM.GetResObj(903, 1)->ShowGWnd(true);
            m_IRM.GetResObj(904, 1)->ShowGWnd(true);
            m_IRM.GetResObj(900, 1)->ShowGWnd(true);
            ps->SetGWndSize(303, 121);
            ps->ShowGWnd(false);

            SkillWeaponSlot->ShowGWnd(false);
            BuffWeaponSlot->ShowGWnd(false);
            SkillShieldSlot->ShowGWnd(false);
            BuffShieldSlot->ShowGWnd(false);

            for (int i = 0; i < 24; i++)
            {
                skillslots[i]->ShowGWnd(false);
                buffslots[i]->ShowGWnd(false);
            }

        }
            break;
    }
}

int CIFMacroMenuAutoSkill::Func_4(int a2) {
    int v1 = 0;
    while (a2 != v1 + 100) {
        if (++v1 >= 17)
            return -1;
    }

    return 100;
}
void CIFMacroMenuAutoSkill::ClearPartySlotDDJ()
{
    for (int i = 0; i < 7; ++i)
    {
        m_PartySlot[i]->ClearDDJ();
    }
    SelectedPartyMemberName = L"";

}

void CIFMacroMenuAutoSkill::Clear()
{
    for (int i = 0; i < 18; ++i)
    {
        m_scroll_skill_list->DeleteItem(m_slotviewer[i]);
        for(int j = 0; j < 8; ++j)
        {
            m_scroll_skill_list->DeleteItem(m_slotviewer[i]->slot1[j]);

        }
    }
}
void CIFMacroMenuAutoSkill::AutoSave(int SlotSeq, byte SlotType, int SlotData)
{
    CMsgStreamBuffer buf(0x188C);
    buf << SlotSeq << SlotType << SlotData;
    SendMsg(buf);
}
void CIFMacroMenuAutoSkill::LoadSkills()
{
    int slotViewerIndex = 0; // slotViewer indexini tutmak için kullanılacak bir değişken
    int slotNumber = 0; // slot numarasını tutmak için kullanılacak bir değişken
    int is = 0;
    int itemCount = 0;
    for (std::map<int, sSkillData*>::iterator it = g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.begin();
         it != g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.end(); ++it)
    {
        if (it->second->m_pSkillData->Target_Required == 1 && it->second->m_pSkillData->TargetType_Animal == 1
            && it->second->m_pSkillData->TargetGroup_Self == 0 && it->second->m_pSkillData->TargetEtc_SelectDeadBody == 0
            && it->second->m_pSkillData->TargetGroup_Party == 0)
        {
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->PutSkills(it->first);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->ShowGWnd(true);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->SetDragable(true);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->N595 = 0;
            is++;

            if ((slotNumber + 1) % 8 == 0) {
                slotViewerIndex++;
                slotNumber = 0;
            } else {
                slotNumber++;
            }

            // Her 8 slot tamamlandığında itemCount’u arttır
            if (is % 8 == 0) {
                itemCount++;
            }
        }
    }

// Kalan öğeler varsa son bir AddItem çağrısı daha yap
    if (is % 8 != 0) {
        itemCount++;
    }

// itemCount değeri kadar AddItem çağrısı yap
    for (int i = 0; i < itemCount; ++i) {
        m_scroll_skill_list->AddItem(m_slotviewer[i], 1, 0);
      //  m_slotviewer[i]->ShowGWnd(true);
        m_slotviewer[i]->BringToFront();
    }

}
void CIFMacroMenuAutoSkill::LoadBuffs()
{

    int slotViewerIndex = 0; // slotViewer indexini tutmak için kullanılacak bir değişken
    int slotNumber = 0; // slot numarasını tutmak için kullanılacak bir değişken
    int is = 0;
    int itemCount = 0;
    for (std::map<int, sSkillData*>::iterator it = g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.begin();
         it != g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.end(); ++it)
    {
        if (it->second->m_pSkillData->TargetGroup_Enemy_M != 1 && it->second->m_pSkillData->TargetGroup_Enemy_P != 1
            && it->second->m_pSkillData->Basic_Activity != 0)
        {
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->PutSkills(it->first);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->ShowGWnd(true);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->SetDragable(true);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->N595 = 0;
            is++;

            if ((slotNumber + 1) % 8 == 0) {
                slotViewerIndex++;
                slotNumber = 0;
            } else {
                slotNumber++;
            }

            // Her 8 slot tamamlandığında itemCount’u arttır
            if (is % 8 == 0) {
                itemCount++;
            }
        }
    }

// Kalan öğeler varsa son bir AddItem çağrısı daha yap
    if (is % 8 != 0) {
        itemCount++;
    }

// itemCount değeri kadar AddItem çağrısı yap
    for (int i = 0; i < itemCount; ++i) {
        m_scroll_skill_list->AddItem(m_slotviewer[i], 1, 0);
       // m_slotviewer[i]->ShowGWnd(true);
        m_slotviewer[i]->BringToFront();  }

}
void CIFMacroMenuAutoSkill::LoadPartyBuffs()
{
    int slotViewerIndex = 0; // slotViewer indexini tutmak için kullanılacak bir değişken
    int slotNumber = 0; // slot numarasını tutmak için kullanılacak bir değişken
    int is = 0;
    int itemCount = 0;
    for (std::map<int, sSkillData*>::iterator it = g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.begin();
         it != g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.end(); ++it)
    {
        if (it->second->m_pSkillData->TargetGroup_Enemy_M != 1 && it->second->m_pSkillData->TargetGroup_Enemy_P != 1
            && it->second->m_pSkillData->Basic_Activity != 0 && it->second->m_pSkillData->TargetGroup_Party == 1)
        {
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->PutSkills(it->first);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->ShowGWnd(true);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->SetDragable(false);
            m_slotviewer[slotViewerIndex]->slot1[slotNumber]->slot->N595 = 2;

            is++;

            if ((slotNumber + 1) % 8 == 0) {
                slotViewerIndex++;
                slotNumber = 0;
            } else {
                slotNumber++;
            }

            // Her 8 slot tamamlandığında itemCount’u arttır
            if (is % 8 == 0) {
                itemCount++;
            }
        }
    }

// Kalan öğeler varsa son bir AddItem çağrısı daha yap
    if (is % 8 != 0) {
        itemCount++;
    }
    // itemCount değeri kadar AddItem çağrısı yap
    for (int i = 0; i < itemCount; ++i) {
        m_scroll_skill_list->AddItem(m_slotviewer[i], 1, 0);
        // m_slotviewer[i]->ShowGWnd(true);
        m_slotviewer[i]->BringToFront();
    }
}
void CIFMacroMenuAutoSkill::UpdateMenuSize()
{
    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2 - 50) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void CIFMacroMenuAutoSkill::SaveButton(){
 char buffer3[0x200];
    sprintf(buffer3, "%s\\Setting\\%ls_MacroAutoBuffSettings.txt", theApp.GetWorkingDir(), g_pMyPlayerObj->GetCharName().c_str());

// Dosyayı yazma modunda aç
    FILE *file = fopen(buffer3, "w");
    if (file != NULL) {
        std::set<int> seenIDs; // Tekrar eden ID'leri izlemek için küme oluştur

        std::map<std::n_wstring, std::vector<int>>::iterator it;
        for (it = PartyBuffList.begin(); it != PartyBuffList.end(); ++it) {
            // Anahtar ve değerleri dosyaya yaz
            std::fwprintf(file, L"%ls: ", it->first.c_str());
            std::vector<int>& values = it->second;

            std::set<int> uniqueValues; // Tekrar etmeyen değerler için küme oluştur
            for (size_t i = 0; i < values.size(); ++i) {
                if (uniqueValues.find(values[i]) == uniqueValues.end()) {
                    uniqueValues.insert(values[i]);
                    std::fprintf(file, "%d ", values[i]);
                }
            }
            std::fprintf(file, "\n");

            seenIDs.insert(uniqueValues.begin(), uniqueValues.end());
        }
        std::fclose(file); // Dosyayı kapat
    }
}
void CIFMacroMenuAutoSkill::CancelButton(){
    g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(1355, 1)->ShowGWnd(false);
}

bool CIFMacroMenuAutoSkill::CheckBuffWeaponSlot(int Type1, int Type2)
{
    if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
    {
        if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if (BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 6 &&
                (BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == Type1 ||
                 BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == Type2))
            {
                BuffWeaponSlot->UseItem(BuffWeaponSlot->m_pMySlot->m_pSlot->GetSlot());
                return true;
            }
            else
            {
                return false;
            }
        }

    }
    return false;
}
bool CIFMacroMenuAutoSkill::CheckSkillWeaponAndWear(int Type1, int Type2)
{

    if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
    {
        if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 6 &&
                (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == Type1 ||
                 SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == Type2))
            {
                SkillWeaponSlot->UseItem(SkillWeaponSlot->m_pMySlot->m_pSlot->GetSlot());
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    return false;
}


bool CIFMacroMenuAutoSkill::CheckSkillShieldCondition()
{
    if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != NULL && SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != NULL)
    {
        if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL && SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 3
               || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 2
               || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 7
               ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 10
               ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 15)
            {
                if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                {
                    if (SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                        SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                        SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                    {
                        SkillShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}
bool CIFMacroMenuAutoSkill::CheckBuffShieldSlot()
{
    if(BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
    {
        if(BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if (BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
            {
                BuffShieldSlot->UseItem(BuffShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                return true;
            }
        }
    }

    return false;
}
bool CIFMacroMenuAutoSkill::CheckBuffShieldCondition()
{
    if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != NULL)
    {
        if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 3
               || BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 2
               || BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 7
               ||  BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 10
               ||  BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 15)
            {
                if(BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                {
                    if (BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                        BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                        BuffShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                    {
                        BuffShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
        }
    }


    return false;
}
bool CIFMacroMenuAutoSkill::CheckSkillWeaponSlots()
{
    if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
    {
        if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData() == NULL)
            {
                SkillWeaponSlot->UseItem(SkillWeaponSlot->m_pMySlot->m_pSlot->GetSlot());
                if (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                    SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                    SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 6
                    && (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 3
                        || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 2
                        || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 7
                        ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 10
                        ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 15))
                {
                    if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                    {
                        if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
                        {
                            if (SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                                SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                                SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                            {
                                SkillShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                            }
                        }
                    }
                }
                return true;
            }
            else if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->RefObjectId !=  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->RefObjectId)
            {
                SkillWeaponSlot->UseItem(SkillWeaponSlot->m_pMySlot->m_pSlot->GetSlot());
                if (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                    SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                    SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 6
                    && (SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 3
                        || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 2
                        || SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 7
                        ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 10
                        ||  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID4() == 15))
                {
                    if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                    {
                        if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
                        {
                            if (SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                                SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                                SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                            {
                                SkillShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                            }
                        }
                    }
                }
                return true;
            }
            else if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->RefObjectId ==  SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->RefObjectId)
            {
                if (g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->m_typeId.getTypeID4() == 3
                    || g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->m_typeId.getTypeID4() == 2
                    || g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->m_typeId.getTypeID4() == 7
                    || g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->m_typeId.getTypeID4() == 10
                    || g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData()->m_typeId.getTypeID4() == 15)
                {
                    if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(7)->GetItemData() != NULL)
                    {
                        if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                        {
                            if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
                            {
                                if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(7)->GetItemData()->RefObjectId != SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->RefObjectId)
                                {
                                    if (SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                                        SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                                        SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                                    {
                                        SkillShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                                        return true;
                                    }
                                }

                            }
                        }
                        return true;
                    }
                    else
                    {
                        if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo != 0x0)
                        {
                            if(SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
                            {
                                if (SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3 &&
                                    SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1 &&
                                    SkillShieldSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                                {
                                    SkillShieldSlot->UseItem(SkillShieldSlot->m_pMySlot->m_pSlot->GetSlot());
                                    return true;
                                }
                            }
                        }
                        return true;
                    }

                }
                else
                {
                    return  true;
                }
            }
        }

    }
    return false;
}
D3DXVECTOR3 CIFMacroMenuAutoSkill::ConvertToGlobalCoordinates(const uregion& region, const D3DXVECTOR3& localCoords) {
    float globalX, globalY;

    if (region.single.x == 0) {  // Assuming dungeon
        globalX = localCoords.x / 10.0f;
        globalY = localCoords.y / 10.0f;
    } else {  // World map
        globalX = (region.single.x - 135) * 192 + localCoords.x / 10.0f;
        globalY = (region.single.y - 92) * 192 + localCoords.y / 10.0f;
    }

    return D3DXVECTOR3(globalX, globalY, localCoords.z);  // Assuming Z coordinate remains unchanged
}
D3DVECTOR CIFMacroMenuAutoSkill::generateRandomPositionWithinDistance(const D3DVECTOR& center, float minDistance, float maxDistance) {
    const float PI = 3.14159265358979323846f;  // Pi sayısı

    float scaledMinDistance = minDistance;
    float scaledMaxDistance = maxDistance;

    D3DVECTOR randomPosition;
    float distance;

    do {
        // Rastgele bir açı ve mesafe üret
        float theta = ((float)std::rand() / RAND_MAX) * 2 * PI;  // 0 ile 2π arasında bir açı
        float phi = ((float)std::rand() / RAND_MAX) * PI;       // 0 ile π arasında bir açı
        float radius = scaledMinDistance + ((float)std::rand() / RAND_MAX) * (scaledMaxDistance - scaledMinDistance);

        // Küresel koordinatları kartesyen koordinatlara çevir
        float randomX = radius * std::sin(phi) * std::cos(theta);
        float randomY = radius * std::sin(phi) * std::sin(theta);
        float randomZ = radius * std::cos(phi);

        randomPosition.x = center.x + randomX;
        randomPosition.y = center.y + randomY;
        randomPosition.z = center.z + randomZ;

        distance = CalculateDistance(center, randomPosition);
    } while (distance < scaledMinDistance || distance > scaledMaxDistance);

    return randomPosition;
}
float CIFMacroMenuAutoSkill::CalculateDistance(const D3DXVECTOR3& loc1, const D3DXVECTOR3& loc2) {
    D3DXVECTOR3 delta = loc1 - loc2;
    return D3DXVec3Length(&delta);
}

int CIFMacroMenuAutoSkill::FindNeededBuff()
{
    int aaa = -1;
    for (int i = 0; i < 24; i++)
    {
        if (buffslots[i]->m_pMySlot->m_pSlot->GetSlotType() == 73 && buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex() != 0)
        {
            if (g_pMyPlayerObj->GetBuffListBase((DWORD32)g_pMyPlayerObj) != 0)
            {
                if (!g_pMyPlayerObj->TargetIsBuffInUse(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex(), (DWORD32)g_pMyPlayerObj)
                && !g_pMyPlayerObj->CheckMagOverlap(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex(), (DWORD32)g_pMyPlayerObj)
                && !g_pMyPlayerObj->CheckPhyOverlap(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex(), (DWORD32)g_pMyPlayerObj)
                && !g_pMyPlayerObj->CheckSpeedOverlap(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex(), (DWORD32)g_pMyPlayerObj))
                {
                    if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->Param8 == 1919250793)
                    {
                        if(CheckBuffShieldSlot())
                        {
                            if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->TargetGroup_Self == 1)
                            {
                                if(g_pCGInterface->Get_SelectedObjectId() != g_pMyPlayerObj->GetUniqueId())
                                {
                                    g_pCGInterface->Set_SelectedObjectId(g_pMyPlayerObj->GetUniqueId());
                                }
                            }
                            int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                            if (cooldown == 0)
                            {
                                aaa = i;
                                return aaa;
                            }
                        }
                    }

                    else if(g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1 == 255)
                    {
                        if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->TargetGroup_Self == 1)
                        {
                            //g_pCGInterface->Set_SelectedObjectId(this->UniqueID());
                            if(g_pCGInterface->Get_SelectedObjectId() != g_pMyPlayerObj->GetUniqueId())
                            {
                                g_pCGInterface->Set_SelectedObjectId(g_pMyPlayerObj->GetUniqueId());
                            }
                        }
                        int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                        if (cooldown == 0)
                        {
                            aaa = i;
                            return aaa;
                        }
                    }
                    else if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1 != 255)
                        {
                            int WeaponCon1 = g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1;
                            int WeaponCon2 = g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon2;
                            if(CheckBuffWeaponSlot(WeaponCon1, WeaponCon2))
                            {
                                CheckBuffShieldCondition();
                                if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->TargetGroup_Self == 1)
                                {
                                    //g_pCGInterface->Set_SelectedObjectId(this->UniqueID());
                                    if(g_pCGInterface->Get_SelectedObjectId() != g_pMyPlayerObj->GetUniqueId())
                                    {
                                        g_pCGInterface->Set_SelectedObjectId(g_pMyPlayerObj->GetUniqueId());
                                    }
                                }
                                int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                                if (cooldown == 0)
                                {
                                    aaa = i;
                                    return aaa;
                                }
                            }
                            else if(CheckSkillWeaponAndWear(WeaponCon1, WeaponCon2))
                            {
                                CheckSkillShieldCondition();
                                if (g_CGlobalDataManager->GetSkillData(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->TargetGroup_Self == 1)
                                {
                                    //g_pCGInterface->Set_SelectedObjectId(this->UniqueID());
                                    if(g_pCGInterface->Get_SelectedObjectId() != g_pMyPlayerObj->GetUniqueId())
                                    {
                                        g_pCGInterface->Set_SelectedObjectId(g_pMyPlayerObj->GetUniqueId());
                                    }
                                }
                                int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(buffslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                                if (cooldown == 0)
                                {
                                    aaa = i;
                                    return aaa;
                                }
                            }
                        }
                    }
            }
        }
    }
    return aaa;
}
bool CIFMacroMenuAutoSkill::SkillVeItemUyumu(tid_t CastWeapon1, tid_t CastWeapon2)
{

    if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != NULL)
    {
        if(SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            TypeId tid = SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId;
            if(tid.getTypeID1() == 3 && tid.getTypeID2() == 1 && tid.getTypeID3() == 6 && (tid.getTypeID4() == CastWeapon1 || tid.getTypeID4() == CastWeapon2))
            {
                return true;
            }
        }
    }
    return false;
}
bool CIFMacroMenuAutoSkill::BuffVeItemUyumu(tid_t CastWeapon1, tid_t CastWeapon2)
{
    if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo != NULL)
    {
        if(BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
        {
            TypeId tid = BuffWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_typeId;
            if(tid.getTypeID1() == 3 && tid.getTypeID2() == 1 && tid.getTypeID3() == 6 && (tid.getTypeID4() == CastWeapon1 || tid.getTypeID4() == CastWeapon2))
            {
                return true;
            }
        }
    }
    return false;
}
int CIFMacroMenuAutoSkill::FindAttackSkillSlot()
{
    int aaa = -1;
    for (int i = 0; i < 24; i++)
    {
        if (skillslots[i]->m_pMySlot->m_pSlot->GetSlotType() == 73 && skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex() != 0)
        {
            if(g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1 == 255)
            {
                CSkillData* skilldata = g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                if(skilldata != NULL)
                {
                    int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                    if (cooldown == 0)
                    {
                        aaa = i;
                        return aaa;
                    }
                }
            }
            else if (g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1 != 255)
            {

                if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData() != NULL && SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
                {
                    if(g_pCGInterface->GetMainPopup()->GetEquipment()->GetEquipmentObjectBySlot(6)->GetItemData() != SkillWeaponSlot->m_pMySlot->m_pSlot->ItemInfo->GetItemData())
                    {
                        SkillWeaponSlot->UseItem(SkillWeaponSlot->m_pMySlot->m_pSlot->GetSlot());
                        if(SkillVeItemUyumu(g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1, g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon2))
                        {

                            CSkillData* skilldata = g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                            if(skilldata != NULL)
                            {
                                int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                                if (cooldown == 0)
                                {
                                    aaa = i;
                                    return aaa;
                                }
                            }
                        }
                    }
                    else
                    {
                        if(SkillVeItemUyumu(g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon1, g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex())->ReqCast_Weapon2))
                        {

                            CSkillData* skilldata = g_CGlobalDataManager->GetSkillData(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                            if(skilldata != NULL)
                            {
                                int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(skillslots[i]->m_pMySlot->m_pSlot->GetSkillSlotInDex());
                                if (cooldown == 0)
                                {
                                    aaa = i;
                                    return aaa;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CIFMacroMenuAutoSkill::StartAutoSkill()
{
    if (!AutoSkillTimerRunning)
    {
        AutoSkillTimerRunning = true;
        g_pCGInterface->StartTimer(START_AUTO_SKILL, 500);
    }
    if (!g_pMyPlayerObj)
        return;
    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead || g_pMyPlayerObj->CHARACTER_STATUS == SkillCast ||g_pMyPlayerObj->CHARACTER_STATUS == Stall
        || g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet == 17)
        return;

    if(AutoSkillTimerRunning)
    {
        int NeededBufSlot = FindNeededBuff();
        if(NeededBufSlot != -1)
        {
            buffslots[NeededBufSlot]->UseItem(buffslots[NeededBufSlot]->m_pMySlot->m_pSlot->GetSlot());
        }
        else
        {
            CIFMacroMenuAutoHunt * AutoHunt = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoHuntSlot;
            if(AutoHunt->Macro_AutoHunt)
            {
                if(!AutoHunt->DONT_ATTACK_MONSTERCB->GetCheckedState_MAYBE())
                {
                    FindMonsterViaAttackAndPartyBuff();
                }
                else
                {
                    if(AutoHunt->AutoResPtMembersCB->GetCheckedState_MAYBE())
                    {
                        if(AutoResurrrectPartyMember() == 0)
                        {
                            ///TODO GO PT BUFF
                            JustPartyBuff();
                        }
                    }
                    else
                    {
                        JustPartyBuff();
                    }
                }
            }
            else
            {
                JustAutoAttack();
            }
        }
    }
}
void CIFMacroMenuAutoSkill::JustAutoAttack(){
    if (!g_pMyPlayerObj)
        return;
    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead || g_pMyPlayerObj->CHARACTER_STATUS == Stall
        || g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet == 17)
        return;

    if(CheckSkillWeaponSlots())
    {
        if(g_pCGInterface->Get_SelectedObjectId() != 0)
        {
            CICharactor *pUser = GetCharacterObjectByID_MAYBE(g_pCGInterface->Get_SelectedObjectId());
            if (pUser != NULL) {
                if(pUser->CHARACTER_STATUS != Dead && pUser->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet != 0
                   &&pUser->unkhp != 0)
                {
                    if(pUser->GetCommonData()->m_typeId.getTypeID1() == 1 && pUser->GetCommonData()->m_typeId.getTypeID2() == 2
                       && pUser->GetCommonData()->m_typeId.getTypeID3() == 1)
                    {
                        int SlotID = FindAttackSkillSlot();
                        if(SlotID != -1)
                        {
                            skillslots[SlotID]->UseItem(skillslots[SlotID]->m_pMySlot->m_pSlot->GetSlot());
                        }
                    }
                }
            }
        }
    }

}
void CIFMacroMenuAutoSkill::FindMonsterViaAttackAndPartyBuff(){
    if (!g_pMyPlayerObj)
        return;
    if (g_pMyPlayerObj->CHARACTER_STATUS == Dead || g_pMyPlayerObj->CHARACTER_STATUS == Stall
        || g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet == 17)
        return;

  /*  }
    if(CheckSkillWeaponSlots())
    {*/
        CIFMacroMenuAutoHunt* Hunt = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoHuntSlot;
        if(!Hunt->DONT_ATTACK_MONSTERCB->GetCheckedState_MAYBE())
        {
            if(Hunt->AutoResPtMembersCB->GetCheckedState_MAYBE())
            {
                if(AutoResurrrectPartyMember() == 0 && JustPartyBuff() == 0)
                {
                    int SelectedObject = GetNearMonsterID();
                    if(g_pCGInterface->Get_SelectedObjectId() != SelectedObject && SelectedObject != 0)
                    {
                        CMsgStreamBuffer buf(0x7045);
                        buf << SelectedObject;
                        SendMsg(buf);

                    }
                    if(SelectedObject == 0)
                    {
                        if(g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet != 2)
                        {
                            D3DVECTOR createdpos = generateRandomPositionWithinDistance(Hunt->GetLocation(), 10*10, 30*10);

                            LocationInfo l1;
                            l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                            l1.pos = g_pMyPlayerObj->GetLocation();
                            l1.field_0 = g_pMyPlayerObj->GetNavCell();
                            l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                            LocationInfo l2;
                            l2.region.raw = Hunt->GetRegion().r;
                            l2.pos = createdpos;
                            if(!g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, g_pMyPlayerObj))
                            {
                                g_pCGInterface->m_Nav.MoveTo(g_pMyPlayerObj->GetRegion(), createdpos);
                            }
                        }
                    }
                    if(g_pCGInterface->Get_SelectedObjectId() == SelectedObject)
                    {
                        JustAutoAttack();
                    }
                }
            }
            else
            {
                if(JustPartyBuff() == 0)
                {
                    int SelectedObject = GetNearMonsterID();
                    if(g_pCGInterface->Get_SelectedObjectId() != SelectedObject && SelectedObject != 0)
                    {
                        CMsgStreamBuffer buf(0x7045);
                        buf << SelectedObject;
                        SendMsg(buf);
                    }
                    if(SelectedObject == 0)
                    {
                        if(g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet != 2)
                        {
                            D3DVECTOR createdpos = generateRandomPositionWithinDistance(Hunt->GetLocation(), 10*10, 30*10);

                            LocationInfo l1;
                            l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                            l1.pos = g_pMyPlayerObj->GetLocation();
                            l1.field_0 = g_pMyPlayerObj->GetNavCell();
                            l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                            LocationInfo l2;
                            l2.region.raw = Hunt->GetRegion().r;
                            l2.pos = createdpos;
                            if(!g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, g_pMyPlayerObj))
                            {
                                g_pCGInterface->m_Nav.MoveTo(g_pMyPlayerObj->GetRegion(), createdpos);
                            }
                        }
                    }
                    if(g_pCGInterface->Get_SelectedObjectId() == SelectedObject)
                    {
                        JustAutoAttack();
                    }
                }

            }

        }

}
int CIFMacroMenuAutoSkill::JustPartyBuff()
{
    if(!PartyBuffList.empty())
    {
        CICharactor * SelectedPartyMember = GetNearPartyMemberID();
        if(SelectedPartyMember != 0 && g_pCGInterface->Get_SelectedObjectId() != SelectedPartyMember->GetUniqueId())
        {
            CMsgStreamBuffer buf(0x7045);
            buf << SelectedPartyMember;
            SendMsg(buf);
        }
        if(SelectedPartyMember != 0)
        {
            if(PartyBuffList.find(SelectedPartyMember->GetName().c_str()) != PartyBuffList.end())
            {
                int UnusedSkillID = 0;
                std::vector<int>& skillList = PartyBuffList[SelectedPartyMember->GetName()];
                for (std::vector<int>::iterator skillIt2 = skillList.begin(); skillIt2 != skillList.end(); ++skillIt2) {
                    if(!TargetIsBuffInUse(*skillIt2, (DWORD32)SelectedPartyMember)
                       && !CheckMagOverlap(*skillIt2, (DWORD32)SelectedPartyMember)
                       && !CheckPhyOverlap(*skillIt2, (DWORD32)SelectedPartyMember)
                       && !CheckSameBuffOverlap(*skillIt2, (DWORD32)SelectedPartyMember))
                    {
                        UnusedSkillID = *skillIt2;
                        break;
                    }
                }

                if(UnusedSkillID != 0)
                {
                    int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(UnusedSkillID);
                    if (cooldown == 0 && SelectedPartyMember->CHARACTER_STATUS != Dead)
                    {
                        int WeaponCon1 = g_CGlobalDataManager->GetSkillData(UnusedSkillID)->ReqCast_Weapon1;
                        int WeaponCon2 = g_CGlobalDataManager->GetSkillData(UnusedSkillID)->ReqCast_Weapon2;
                        if(BuffVeItemUyumu(WeaponCon1, WeaponCon2) && CheckBuffWeaponSlot(WeaponCon1, WeaponCon2))
                        {
                            CheckBuffShieldCondition();
                            CMsgStreamBuffer buf(0x7074);
                            buf << (byte)1;
                            buf << (byte)4;
                            buf << (unsigned int)UnusedSkillID;
                            buf << (byte)1;
                            buf << (unsigned int)SelectedPartyMember->GetUniqueId();
                            SendMsg(buf);
                            return 1;
                        }

                    }

                }

            }
        }
    }
    return 0;
}
DWORD32 CIFMacroMenuAutoSkill::GetBuffListBase(DWORD32 Addr) {
    if(g_pMyPlayerObj)
        return *(DWORD32*)(Addr + 0x288);
    else
        return 0;
}
boolean CIFMacroMenuAutoSkill::TargetIsBuffInUse(DWORD32 SkillID, DWORD32 Target) {
    DWORD32 dwListBase = GetBuffListBase(Target);
    if(dwListBase != 0)
    {
        DWORD32 Base = dwListBase;
        while (1)
        {
            Base = *(DWORD32*)(Base);
            if (Base == 0)
                return false;
            if (Base == dwListBase)
                return false;
            if (SkillID == *(DWORD32*)(*(DWORD32*)(Base + 0x8) + 0x5C)) {
                return true;
            }
        }
    }
    return false;
}
CICharactor* CIFMacroMenuAutoSkill::GetNearPartyMemberID() {
    CIFMacroMenuAutoHunt * AutoHuntSlot = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoHuntSlot;
    D3DXVECTOR3 location2 = g_pMyPlayerObj->GetLocation();
    float radius = AutoHuntSlot->AutoHuntSetting[RADUIS_SETTING];

        D3DXVECTOR3 locationtarget = AutoHuntSlot->GetLocation();
        D3DXVECTOR3 locationmine = g_pMyPlayerObj->GetLocation();

// Convert local coordinates to global coordinates
        D3DXVECTOR3 globalTarget = ConvertToGlobalCoordinates(AutoHuntSlot->GetRegion(), locationtarget);
        D3DXVECTOR3 globalMine = ConvertToGlobalCoordinates(g_pMyPlayerObj->GetRegion(), locationmine);


        float distance = CalculateDistance(globalTarget, globalMine);
        if(distance / 10 < radius + 10)
        {
            if(g_pMyPlayerObj->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet != 2)
            {
                LocationInfo l1;
                l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                l1.pos = g_pMyPlayerObj->GetLocation();
                l1.field_0 = g_pMyPlayerObj->GetNavCell();
                l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                LocationInfo l2;
                l2.region.raw = AutoHuntSlot->GetRegion().r;
                l2.pos = AutoHuntSlot->GetLocation();

                if(!g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, g_pMyPlayerObj))
                {
                    g_pCGInterface->m_Nav.MoveTo(AutoHuntSlot->GetRegion(), AutoHuntSlot->GetLocation());
                }
            }
        }

        unsigned int FoundedUserUniqueID = 0;
        std::n_wstring FoundedCharName;
        std::map<int, CIObject*> entities = g_pGfxEttManager->entities;
        for (std::map<int, CIObject*>::iterator it = entities.begin(); it != entities.end(); ++it) {
            if (!strcmp(it->second->GetRuntimeClass()->m_lpszClassName, "CICUser")) {
                CICUser* nearmember = (CICUser*)it->second;
                if (nearmember)
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
        bool isinparty = false;

        if (FoundedUserUniqueID != 0) {

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
            if(isinparty)
            {
                CICharactor* nearpartyuser = GetCharacterObjectByID_MAYBE(FoundedUserUniqueID);
                if(nearpartyuser->CHARACTER_STATUS != Dead)
                {
                    if(nearpartyuser->IsChinese() || nearpartyuser->IsEurope())
                    {
                        if(PartyBuffList.find(nearpartyuser->GetName().c_str()) != PartyBuffList.end())
                        {
                            int UnusedSkillID = 0;
                            std::vector<int>& skillList = PartyBuffList[nearpartyuser->GetName()];
                            for (std::vector<int>::iterator skillIt2 = skillList.begin(); skillIt2 != skillList.end(); ++skillIt2) {
                                if(!TargetIsBuffInUse(*skillIt2, (DWORD32)nearpartyuser)
                                        && !CheckMagOverlap(*skillIt2, (DWORD32)nearpartyuser)
                                        && !CheckPhyOverlap(*skillIt2, (DWORD32)nearpartyuser)
                                        && !CheckSameBuffOverlap(*skillIt2, (DWORD32)nearpartyuser))
                                {
                                    UnusedSkillID = *skillIt2;
                                    break;
                                }
                            }
                            if(UnusedSkillID != 0)
                            {
                                CICharactor* pUser = GetCharacterObjectByID_MAYBE(nearpartyuser->GetUniqueId());
                                // Get local coordinates from your objects
                                D3DXVECTOR3 locationtarget = pUser->GetLocation();
                                D3DXVECTOR3 locationmine = g_pMyPlayerObj->GetLocation();

// Convert local coordinates to global coordinates
                                D3DXVECTOR3 globalTarget = ConvertToGlobalCoordinates(pUser->GetRegion(), locationtarget);
                                D3DXVECTOR3 globalMine = ConvertToGlobalCoordinates(g_pMyPlayerObj->GetRegion(), locationmine);


                                float distance = CalculateDistance(globalTarget, globalMine);
                                if (distance / 10 <= radius + 10)
                                {
                                    if (pUser && pUser->CHARACTER_STATUS != Dead) {
                                        LocationInfo l1;
                                        l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                                        l1.pos = g_pMyPlayerObj->GetLocation();
                                        l1.field_0 = g_pMyPlayerObj->GetNavCell();
                                        l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                                        LocationInfo l2;
                                        l2.region.raw = nearpartyuser->GetRegion().r;
                                        l2.pos = nearpartyuser->GetLocation();
                                        l2.field_0 = nearpartyuser->GetNavCell();
                                        l2.field_1 = nearpartyuser->m_object_under_foot;
                                        if(!g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, nearpartyuser))
                                            return pUser;
                                    }
                                }
                            }

                        }
                    }
                }
            }
        }
    return 0;
}

int CIFMacroMenuAutoSkill::GetNearMonsterID() {

    CIFMacroMenuAutoHunt* Hunt = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoHuntSlot;
    float radius = Hunt->AutoHuntSetting[RADUIS_SETTING];

    D3DXVECTOR3 locationmine = g_pMyPlayerObj->GetLocation();

// Convert local coordinates to global coordinates
    D3DXVECTOR3 StartLocation = ConvertToGlobalCoordinates(Hunt->GetRegion(), Hunt->GetLocation());
    D3DXVECTOR3 CurrentLocationForBack   = ConvertToGlobalCoordinates(g_pMyPlayerObj->GetRegion(), locationmine);

    std::map<int, CIObject*> entities = g_pGfxEttManager->entities;
    for (std::map<int, CIObject*>::iterator it = entities.begin(); it != entities.end(); ++it)
        {
            if (it->second->IsSame(GFX_RUNTIME_CLASS(CICMonster)))
            {
                CICMonster* nearmonster = (CICMonster*)it->second;

                if (!nearmonster)
                {
                    continue;
                }
                if (nearmonster->CHARACTER_STATUS == Dead && nearmonster->Dead0Stay1Walking2Sit0SkillCast0emotion33Stall12817isridingpet == 0)
                {
                    continue;
                }


                D3DXVECTOR3 TargetMobLocation = ConvertToGlobalCoordinates(it->second->GetRegion(), it->second->GetLocation());
                D3DXVECTOR3 CurrentLocation = ConvertToGlobalCoordinates(Hunt->GetRegion(), Hunt->GetLocation());

                float distanceForMob = CalculateDistance(TargetMobLocation, CurrentLocation);
                // Canavarın bölge ve konum bilgilerini al
                if ((distanceForMob / 10) > (radius + 15))
                {
                    continue;
                }
                LocationInfo l1;
                l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                l1.pos = g_pMyPlayerObj->GetLocation();
                l1.field_0 = g_pMyPlayerObj->GetNavCell();
                l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                LocationInfo l2;
                l2.region.raw = nearmonster->GetRegion().r;
                l2.pos = nearmonster->GetLocation();
                l2.field_0 = nearmonster->GetNavCell();
                l2.field_1 = nearmonster->m_object_under_foot;

                if(g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, g_pMyPlayerObj))
                {
                   continue;
                }
                short Level = *(short*)((DWORD32)nearmonster + 0x7AC);//0 Normal 1 = champion 3 rare 4 Giant
                    if(Hunt->AutoHuntSetting[ZERK_SETTING] == eZerkSetting::All && Level == 0)
                    {
                        // berserk
                        CIFPlayerMiniInfo *miniinfo = (CIFPlayerMiniInfo*)g_pCGInterface->m_IRM.GetResObj(11, 1);
                        if(miniinfo != NULL)
                        {
                            if(miniinfo->GetZerkButtonState())
                            {
                                if(miniinfo->GetZerkButtonState())
                                {
                                    CMsgStreamBuffer buf(0x70A7);
                                    buf << (byte)1;
                                    SendMsg(buf);
                                }
                            }
                        }
                    }
                    else if(Hunt->AutoHuntSetting[ZERK_SETTING] == eZerkSetting::Giant && Level == 4)
                    {
                        CIFPlayerMiniInfo *miniinfo = (CIFPlayerMiniInfo*)g_pCGInterface->m_IRM.GetResObj(11, 1);
                        if(miniinfo != NULL)
                        {
                            if(miniinfo->GetZerkButtonState())
                            {
                                if(miniinfo->GetZerkButtonState())
                                {
                                    CMsgStreamBuffer buf(0x70A7);
                                    buf << (byte)1;
                                    SendMsg(buf);
                                }
                            }
                        }
                    }


                return nearmonster->GetUniqueId();
                }
            }

    return 0;
}      
int CIFMacroMenuAutoSkill::IHaveResurrectSkill(byte Level)
{
    for (std::map<int, sSkillData*>::iterator it = g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.begin();
         it != g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.end(); ++it)
    {
        if(g_CGlobalDataManager->GetSkillData(it->second->m_skillId) != NULL)
        {
            if(g_CGlobalDataManager->GetSkillData(it->second->m_skillId)->TargetEtc_SelectDeadBody == 1 &&
               g_CGlobalDataManager->GetSkillData(it->second->m_skillId)->Param2 != 1835229552
               && (g_CGlobalDataManager->GetSkillData(it->second->m_skillId)->Param10 >= Level ||
                   g_CGlobalDataManager->GetSkillData(it->second->m_skillId)->Param8 >= Level))
            {
                return it->first;
            }
        }
    }

    return 0;
}

int CIFMacroMenuAutoSkill::AutoResurrrectPartyMember()
{
    time_t now = time(NULL);  // Güncel zamanı al
    double elapsed = difftime(now, MyLastResUseTime);

    if(elapsed <  30)
    return 0;// 30 saniye geçmiş mi kontrol et
    CICharactor * SelectedPartyMember = GetNearPartyDeadMemberID();
    if(SelectedPartyMember != 0 && g_pCGInterface->Get_SelectedObjectId() != SelectedPartyMember->GetUniqueId())
    {
        CMsgStreamBuffer buf(0x7045);
        buf << SelectedPartyMember;
        SendMsg(buf);
    }
    if(SelectedPartyMember != 0)
    {
        byte TargetLevel = 0;
        const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
        if (partyData.NumberOfMembers > 0)
        {
            for (int i = 0; i < partyData.NumberOfMembers; ++i) {
                const SPartyMemberData& memberData = g_CCharacterDependentData.GetPartyMemberData(i);
                if(memberData.m_charactername == SelectedPartyMember->GetName())
                {
                    TargetLevel = memberData.currentLevel;
                    break;
                }
            }
        }
        if(TargetLevel != 0)
        {
            int ResSkillID = IHaveResurrectSkill(TargetLevel);
            if(ResSkillID!= 0)
            {
                int cooldown = g_pCGInterface->GetSkillCoolTimeManager()->FUN_009bba90(ResSkillID);
                if(cooldown == 0)
                {
                    if (SelectedPartyMember->CHARACTER_STATUS == Dead)
                    {
                        if (g_CGlobalDataManager->GetSkillData(ResSkillID)->Param8 == 1919250793)
                        {
                            if(CheckBuffShieldCondition())
                            {
                                CMsgStreamBuffer buf(0x7074);
                                buf << (byte)1;
                                buf << (byte)4;
                                buf << (unsigned int)ResSkillID;
                                buf << (byte)1;
                                buf << (unsigned int)SelectedPartyMember->GetUniqueId();
                                SendMsg(buf);
                                MyLastResUseTime = time(NULL);
                                return 1;
                            }
                        }
                        else if(g_CGlobalDataManager->GetSkillData(ResSkillID)->ReqCast_Weapon1 == 255)
                        {
                            CMsgStreamBuffer buf(0x7074);
                            buf << (byte)1;
                            buf << (byte)4;
                            buf << (unsigned int)ResSkillID;
                            buf << (byte)1;
                            buf << (unsigned int)SelectedPartyMember->GetUniqueId();
                            SendMsg(buf);
                            MyLastResUseTime = time(NULL);
                            return 1;
                        }
                        else if (g_CGlobalDataManager->GetSkillData(ResSkillID)->ReqCast_Weapon1 != 255)
                        {

                            int WeaponCon1 = g_CGlobalDataManager->GetSkillData(ResSkillID)->ReqCast_Weapon1;
                            int WeaponCon2 = g_CGlobalDataManager->GetSkillData(ResSkillID)->ReqCast_Weapon2;
                            if(CheckBuffWeaponSlot(WeaponCon1, WeaponCon2))
                            {
                                CheckBuffShieldCondition();

                                CMsgStreamBuffer buf(0x7074);
                                buf << (byte)1;
                                buf << (byte)4;
                                buf << (unsigned int)ResSkillID;
                                buf << (byte)1;
                                buf << (unsigned int)SelectedPartyMember->GetUniqueId();
                                SendMsg(buf);
                                MyLastResUseTime = time(NULL);
                                return 1;
                            }
                            else if(CheckSkillWeaponAndWear(WeaponCon1, WeaponCon2))
                            {
                                CheckSkillShieldCondition();
                                CMsgStreamBuffer buf(0x7074);
                                buf << (byte)1;
                                buf << (byte)4;
                                buf << (unsigned int)ResSkillID;
                                buf << (byte)1;
                                buf << (unsigned int)SelectedPartyMember->GetUniqueId();
                                SendMsg(buf);
                                MyLastResUseTime = time(NULL);
                                return 1;
                            }

                        }
                    }
                }
            }
        }
    }
    return 0;
}
CICharactor* CIFMacroMenuAutoSkill::GetNearPartyDeadMemberID() {

    CIFMacroMenuAutoHunt* Hunt = g_pCGInterface->m_IRM.GetResObj<CIFMacroMenu>(MacroMenuID, 1)->AutoHuntSlot;
    float radius = Hunt->AutoHuntSetting[RADUIS_SETTING];

        unsigned int FoundedUserUniqueID = 0;
        std::n_wstring FoundedCharName;
        std::map<int, CIObject*> entities = g_pGfxEttManager->entities;
        for (std::map<int, CIObject*>::iterator it = entities.begin(); it != entities.end(); ++it) {
            if (!strcmp(it->second->GetRuntimeClass()->m_lpszClassName, "CICUser")) {
                CICUser* nearmember = (CICUser*)it->second;
                if (nearmember)
                {
                    if (nearmember->CHARACTER_STATUS == Dead)
                    {
                        FoundedUserUniqueID = nearmember->GetUniqueId();
                        FoundedCharName = nearmember->GetName();
                        break;
                    }
                }
            }
        }
        bool isinparty = false;

        if (FoundedUserUniqueID != 0) {

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

            if(isinparty)
            {
                CICharactor* nearpartyuser = GetCharacterObjectByID_MAYBE(FoundedUserUniqueID);
                if(nearpartyuser->CHARACTER_STATUS == Dead)
                {
                    if(nearpartyuser->IsChinese() || nearpartyuser->IsEurope())
                    {
                        CICharactor* pUser = GetCharacterObjectByID_MAYBE(nearpartyuser->GetUniqueId());

                        D3DXVECTOR3 globalTarget = ConvertToGlobalCoordinates(pUser->GetRegion(), pUser->GetLocation());
                        D3DXVECTOR3 globalMine = ConvertToGlobalCoordinates(g_pMyPlayerObj->GetRegion(), g_pMyPlayerObj->GetLocation());

                        float distance = CalculateDistance(globalTarget, globalMine);
                        if (distance /10 <= radius)
                        {
                            if (pUser && pUser->CHARACTER_STATUS == Dead) {
                                LocationInfo l1;
                                l1.region.raw = g_pMyPlayerObj->GetRegion().r;
                                l1.pos = g_pMyPlayerObj->GetLocation();
                                l1.field_0 = g_pMyPlayerObj->GetNavCell();
                                l1.field_1 = g_pMyPlayerObj->m_object_under_foot;

                                LocationInfo l2;
                                l2.region.raw = nearpartyuser->GetRegion().r;
                                l2.pos = nearpartyuser->GetLocation();
                                l2.field_0 = nearpartyuser->GetNavCell();
                                l2.field_1 = nearpartyuser->m_object_under_foot;
                                if(!g_CRegionManagerBody.SomethingWithMapCollision_MAYBE(1, 1, &l1, &l2, 0, nearpartyuser))
                                    return pUser;
                            }
                        }
                    }

                }
            }
        }
    return 0;
}

boolean CIFMacroMenuAutoSkill::CheckSameBuffOverlap(DWORD32 SkillID, DWORD32 Target)
{
    DWORD32 dwListBase = GetBuffListBase(Target);
    if (dwListBase != 0)
    {
        DWORD32 Base = dwListBase;
        while (1)
        {
            Base = *(DWORD32*)(Base);
            if (Base == 0 || Base == dwListBase)
            {
                break;
            }
            DWORD32 currentSkillID = *(DWORD32*)(*(DWORD32*)(Base + 0x8) + 0x5C);
            if (g_CGlobalDataManager->GetSkillData(currentSkillID) != NULL && g_CGlobalDataManager->GetSkillData(SkillID) != NULL)
            {
                int currentSkillOverlap = g_CGlobalDataManager->GetSkillData(currentSkillID)->Action_Overlap;
                int targetSkillOverlap = g_CGlobalDataManager->GetSkillData(SkillID)->Action_Overlap;
                // Mag overlap kontrolü
                if (targetSkillOverlap == currentSkillOverlap)
                {
                    return true;
                }
            }
        }
    }
    return false;
}
boolean CIFMacroMenuAutoSkill::CheckMagOverlap(DWORD32 SkillID, DWORD32 Target)
{
    DWORD32 dwListBase = GetBuffListBase(Target);
    if (dwListBase != 0)
    {
        DWORD32 Base = dwListBase;
        while (1)
        {
            Base = *(DWORD32*)(Base);
            if (Base == 0 || Base == dwListBase)
            {
                break;
            }
            DWORD32 currentSkillID = *(DWORD32*)(*(DWORD32*)(Base + 0x8) + 0x5C);
            if (g_CGlobalDataManager->GetSkillData(currentSkillID) != NULL && g_CGlobalDataManager->GetSkillData(SkillID) != NULL)
            {
                int currentSkillOverlap = g_CGlobalDataManager->GetSkillData(currentSkillID)->Action_Overlap;
                int targetSkillOverlap = g_CGlobalDataManager->GetSkillData(SkillID)->Action_Overlap;

                int ChFireMagDef = 3;
                int EuDeityMagDef = 1744830467;

                // Mag overlap kontrolü
                if ((targetSkillOverlap == ChFireMagDef && currentSkillOverlap == EuDeityMagDef) ||
                    (targetSkillOverlap == EuDeityMagDef && currentSkillOverlap == ChFireMagDef))
                {

                    return true;
                }
            }
        }
    }
    return false;
}

boolean CIFMacroMenuAutoSkill::CheckPhyOverlap(DWORD32 SkillID, DWORD32 Target)
{
    DWORD32 dwListBase = GetBuffListBase(Target);
    if (dwListBase != 0)
    {
        DWORD32 Base = dwListBase;
        while (1)
        {
            Base = *(DWORD32*)(Base);
            if (Base == 0 || Base == dwListBase)
            {
                break;
            }
            DWORD32 currentSkillID = *(DWORD32*)(*(DWORD32*)(Base + 0x8) + 0x5C);
            if (g_CGlobalDataManager->GetSkillData(currentSkillID) != NULL && g_CGlobalDataManager->GetSkillData(SkillID) != NULL)
            {
                int currentSkillOverlap = g_CGlobalDataManager->GetSkillData(currentSkillID)->Action_Overlap;
                int targetSkillOverlap = g_CGlobalDataManager->GetSkillData(SkillID)->Action_Overlap;

                int ChColdPhyDef = 2;
                int EuDeityPhyDef = 1728053250;


                // Phy overlap kontrolü
                if ((targetSkillOverlap == ChColdPhyDef && currentSkillOverlap == EuDeityPhyDef) ||
                    (targetSkillOverlap == EuDeityPhyDef && currentSkillOverlap == ChColdPhyDef))
                {

                    return true;
                }
            }
        }
    }
    return false;
}