#include "IFAchievements.h"
#include <Game.h>
#include <ICPlayer.h>
#include <BSLib/multibyte.h>
#include "GInterface.h"
#include <CharacterDependentData.h>
#include <TextStringManager.h>
#include <GlobalDataManager.h>
#include <CustomData/CustomDataManager.h>
#include <CustomData/CustomCICPlayer.h>



#define GDR_BUTTON_USE 23
#define GDR_BUTTON_ALL 112
#define GDR_BUTTON_GENERAL 113
#define GDR_BUTTON_QUEST 114
#define GDR_BUTTON_UNIQUE 115
#define GDR_BUTTON_MATCH 116
#define GDR_BUTTON_EVENT 117
#define GDR_BUTTON_REMOVE 24
GFX_IMPLEMENT_DYNCREATE(CIFAchievements, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFAchievements, CIFMainFrame)
                    ONG_COMMAND(100, &CIFAchievements::OnUnknownStuff)
                    ONG_COMMAND(GDR_BUTTON_USE, &On_BtnClick)
                    ONG_COMMAND(GDR_BUTTON_ALL, &On_BtnClickAll)
                    ONG_COMMAND(GDR_BUTTON_GENERAL, &On_BtnClickGeneral)
                    ONG_COMMAND(GDR_BUTTON_QUEST, &On_BtnClickQuest)
                    ONG_COMMAND(GDR_BUTTON_UNIQUE, &On_BtnClickUnique)
                    ONG_COMMAND(GDR_BUTTON_MATCH, &On_BtnClickMatch)
                    ONG_COMMAND(GDR_BUTTON_EVENT, &On_BtnClickEvent)
                    ONG_COMMAND(GDR_BUTTON_REMOVE, &On_BtnClickRemove)

GFX_END_MESSAGE_MAP()

CIFAchievements::CIFAchievements(void){
    vscroll = 0;
    m_scroll = 0;
    SelectedItemID = 0;
    ActiveTabNumber = -1;
    LastActiveCategory = -1;

}
CIFAchievements::~CIFAchievements(void){
    if (m_pTabs) {
        free(m_pTabs);
        m_pTabs = 0;
    }
}

int CIFAchievements::Func_4(int a2) {
    int v1 = 0;
    while (a2 != v1 + 100) {
        if (++v1 >= 5)
            return -1;
    }

    return 100;
}

int CIFAchievements::Func_36(int a1, short action, int a3, int a4) {
    if (action <= 0) {
        if (action < 0) {
            vscroll->sub_65A5C0(0);
        }
    } else {
        vscroll->sub_65A5A0(0);
    }

    return 1;
}

bool CIFAchievements::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);
    wnd_rect sz;

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifachievements.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    m_pTabs = new CIFSelectableArea *[numberOfTabs];

    for (int i = 0; i < numberOfTabs; i++) {


        RECT mrect;
        mrect.top = 5;
        mrect.left = 6;
        mrect.right = 0;
        mrect.bottom = 4;

        RECT selectable_area_size;
        selectable_area_size.top = 39;
        selectable_area_size.left = tabMarginLeft + tabWidth * i;
        selectable_area_size.right = tabWidth - 4;
        selectable_area_size.bottom = tabHeight;

        m_pTabs[i] = (CIFSelectableArea *) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSelectableArea),
                                                                 selectable_area_size, tabFirstId + i, 0);

        m_pTabs[i]->SetSomeRect(mrect);
        m_pTabs[i]->SetFont(this->N00009C2F);

        m_pTabs[i]->sub_64CE30("clientlibrary\\menu\\com_long_tab_on.ddj",
                               "clientlibrary\\menu\\com_long_tab_off.ddj", "clientlibrary\\menu\\com_long_tab_disable.ddj");

        if (i) {
            switch (i) {
                case 1:
                    std::n_wstring msg = L"Achievements";
                    std::n_wstring labelmsg = KisaYazi(msg, 6);
                    m_pTabs[1]->SetText(labelmsg.c_str());
                    m_pTabs[1]->SetTooltip(msg);
                    m_pTabs[1]->SetStyleThingy(TOOLTIP);
                    break;

            }
            m_pTabs[i]->sub_64CC30(0);
        } else {
            m_pTabs[0]->SetText(L"Titles");


            m_pTabs[i]->sub_64CC30(1);
        }
    }
    std::n_wstring msg = L"Achievements";
    std::n_wstring labelmsg = KisaYazi(msg, 6);
    m_pTabs[1]->SetText(labelmsg.c_str());
    m_pTabs[1]->SetTooltip(msg);
    m_pTabs[1]->SetStyleThingy(TOOLTIP);


    std::n_wstring category = L"Category";
    std::n_wstring categoryshort = category.substr(0, 2) + L"..";
    m_IRM.GetResObj(9, 1)->SetText(categoryshort.c_str());
    m_IRM.GetResObj(9, 1)->SetTooltip(category);
    m_IRM.GetResObj(9, 1)->SetStyleThingy(TOOLTIP);



    m_IRM.GetResObj<CIFStatic>(10, 1)->SetText(L"Title Name");
    m_IRM.GetResObj<CIFStatic>(3, 1)->SetText(L"Current Title :");


    m_scroll = this->m_IRM.GetResObj<CIFScrollManager>(11, 1);
    m_scroll->sub_008124F0(0);
    m_scroll->sub_008124C0(33);
    m_scroll->sub_008123F0(6);
    m_scroll->sub_00812500(0);
    m_scroll->sub_00812420(-8, 0);

    sz.size.width = 192;
    sz.size.height = 32;
    for (int i = 0; i < 20; ++i)
    {
        m_slots[i] = (CIFAchievementsSlot *) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFAchievementsSlot), sz, 302+i, 0);
    }


    descbox = m_IRM.GetResObj<CIFTextBox>(25, 1);
    descbox->JustifyHorizontal(JUSTIFY_LEFT);
    descbox->JustifyVertical(JUSTIFY_MIDDLE);
    descbox->N00000608 = this;

    vscroll = this->m_IRM.GetResObj<CIFVerticalScroll>(22, 1);


    descbox->m_HeightInLines = 10;
    descbox->SetLineHeight(20);
    descbox->sub_638B50(0);
    descbox->sub_64E380(0);
    descbox->sub_638C70(14);
    descbox->sub_638D50(1);
    descbox->sub_638D40(1);
    descbox->LinkScrollbar(vscroll);
    descbox->m_LinesOfHistory = 500;

   descbox->N00000608 = this;
   descbox->m_HeightInLines = 16;
   descbox->SetLineHeight(16);
   descbox->sub_638B50(0);
   descbox->sub_64E380(0);
   descbox->sub_638C70(16);
   //descbox->SetHightlineLine(true);
   descbox->sub_638D50(1);
 //  descbox->SetHighlightColor(D3DCOLOR_RGBA(255, 255, 255, 102));
   descbox->sub_638D40(1);
   descbox->ShowGWnd(true);
   descbox->m_FontTexture.sub_8B4400(0, 0);
   descbox->m_FontTexture.sub_8B4750(0);


    m_IRM.GetResObj<CIFStatic>(800, 1)->BringToFront();
    m_IRM.GetResObj<CIFStatic>(801, 1)->BringToFront();
    m_IRM.GetResObj<CIFStatic>(802, 1)->BringToFront();
    m_IRM.GetResObj<CIFStatic>(803, 1)->BringToFront();
    m_IRM.GetResObj<CIFStatic>(804, 1)->BringToFront();
    m_IRM.GetResObj<CIFStatic>(805, 1)->BringToFront();

    std::n_wstring msgall = L"All";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_ALL, 1)->SetTooltip(msgall);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_ALL, 1)->SetStyleThingy(TOOLTIP);

    std::n_wstring msggeneral = L"General";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_GENERAL, 1)->SetTooltip(msggeneral);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_GENERAL, 1)->SetStyleThingy(TOOLTIP);

    std::n_wstring msgquest = L"Quest";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_QUEST, 1)->SetTooltip(msgquest);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_QUEST, 1)->SetStyleThingy(TOOLTIP);

    std::n_wstring msgunique = L"Unique";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_UNIQUE, 1)->SetTooltip(msgunique);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_UNIQUE, 1)->SetStyleThingy(TOOLTIP);



    std::n_wstring msgmatch = L"Matches";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_MATCH, 1)->SetTooltip(msgmatch);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_MATCH, 1)->SetStyleThingy(TOOLTIP);

    std::n_wstring msgevent = L"Event";
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_EVENT, 1)->SetTooltip(msgevent);
    m_IRM.GetResObj<CIFButton>(GDR_BUTTON_EVENT, 1)->SetStyleThingy(TOOLTIP);

    m_IRM.GetResObj<CIFButton>(24, 1)->SetText(L"Remove Title");
    m_IRM.GetResObj<CIFButton>(23, 1)->SetText(L"Use Title");
    m_IRM.GetResObj(19, 1)->SetText(L"Descriptions");
    m_IRM.GetResObj(10, 1)->SetText(L"Title Name");
    UpdateMenuSize();
    this->ShowGWnd(false);
    return true;
}
void CIFAchievements::OnUnknownStuff() {
    int id = GetCurrentEventMsgCtrlId();
    int i = 0;

    for (int i = 0; i < numberOfTabs; ++i) {
        if (id == m_pTabs[i]->UniqueID()) {
            ActivateTabPage(i);
            return;
        }
    }
}
void CIFAchievements::ActivateTabPage(BYTE page) {
    for (int i = 0; i < numberOfTabs; i++) {
        if (i == page)
            continue;

        m_pTabs[i]->sub_64CC30(0);
        m_pTabs[i]->m_FontTexture.sub_8B4750(2);
    }


    m_pTabs[page]->sub_64CC30(1);

    if(page == 0)
    {
        ActiveTabNumber = 0;
        m_IRM.GetResObj<CIFStatic>(10, 1)->SetText(L"Title Name");


        m_IRM.GetResObj<CIFButton>(23, 1)->ShowGWnd(true);
        m_IRM.GetResObj<CIFButton>(24, 1)->ShowGWnd(true);
        m_IRM.GetResObj<CIFStatic>(3, 1)->SetText(L"Current Title : ");

        m_IRM.GetResObj(4, 1)->SetGWndSize(290, 22);
        m_IRM.GetResObj(5, 1)->SetGWndSize(286, 18);
        m_IRM.GetResObj(7, 1)->ShowGWnd(true);
        m_IRM.GetResObj(118, 1)->ShowGWnd(false);
        m_IRM.GetResObj(119, 1)->ShowGWnd(false);
        m_IRM.GetResObj<CIFButton>(23, 1)->SetEnabledState(false);
        Clear();
        ClearDDJ();
        SetDescBoxText(L" ");
        On_BtnClickAll();
    }
    else if(page == 1)
    {
        ActiveTabNumber = 1;
        m_IRM.GetResObj<CIFStatic>(10, 1)->SetText(L"Name");

        m_IRM.GetResObj<CIFButton>(23, 1)->ShowGWnd(false);
        m_IRM.GetResObj<CIFButton>(24, 1)->ShowGWnd(false);

        std::n_wstring category = L"Achievements :";
        m_IRM.GetResObj(3, 1)->SetText(category.c_str());

        m_IRM.GetResObj(4, 1)->SetGWndSize(370, 22);
        m_IRM.GetResObj(5, 1)->SetGWndSize(366, 18);
        m_IRM.GetResObj(7, 1)->ShowGWnd(false);

        int completesize = 0;
        int achievemetsize = m_CustomDataManager->m_RefAchievement.size();
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(it->second != 0)
            {
                completesize++;
            }
        }
        if(completesize > 0 && achievemetsize > 0)
        {
            int barwidht = (369 * completesize) / achievemetsize;
            m_IRM.GetResObj(118, 1)->SetGWndSize(barwidht, 18);
            m_IRM.GetResObj(118, 1)->ShowGWnd(true);
            wchar_t buffer[1024];
            int yuzde = (completesize*100)/achievemetsize;
            swprintf(buffer, L"%d%%", yuzde);
            m_IRM.GetResObj(119, 1)->SetText(buffer);

            m_IRM.GetResObj(119, 1)->ShowGWnd(true);
        }
        else
        {
            int barwidht = 0;
            m_IRM.GetResObj(118, 1)->SetGWndSize(barwidht, 18);
            m_IRM.GetResObj(118, 1)->ShowGWnd(true);
            wchar_t buffer[1024];
            swprintf(buffer, L"%d%%", 0);
            m_IRM.GetResObj(119, 1)->SetText(buffer);
            m_IRM.GetResObj(119, 1)->ShowGWnd(true);
        }

        Clear();
        ClearDDJ();
        On_BtnClickAll();
        SetDescBoxText(L" ");
    }
}
void CIFAchievements::UpdateMenuSize()
{

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void CIFAchievements::SetDescBoxText(std::n_wstring string)
{
    //  vscroll->sub_65A5A0(999);
    descbox->SetText(string.c_str());
    vscroll->sub_65A5A0(999);

}

void CIFAchievements::SetUseButtonState(bool s)
{
    m_IRM.GetResObj<CIFButton>(23, 1)->SetEnabledState(s);
}

void CIFAchievements::OnUpdate()
{
        std::n_wstring Test;
        g_pMyPlayerObj->fonttexture_playername.GetText(&Test);
        this->m_IRM.GetResObj<CIFStatic>(7, 1)->SetText(Test.c_str());
}
void CIFAchievements::Clear()
{
    for (int i = 0; i < 20; ++i)
    {
        m_scroll->DeleteItem(m_slots[i]);
    }
}
void CIFAchievements::ClearDDJ()
{
    for (int i = 0; i < 20; ++i)
    {
        m_slots[i]->ClearDDJ();
    }
    g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->SetUseButtonState(false);
}
void CIFAchievements::On_BtnClick() {

    if(SelectedItemID > 0)
    {
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x8) << SelectedItemID;
        SendMsg(buf);
        UseButtonDelay(7500);
    }
}
void CIFAchievements::OnListUpdated(){
    int i = 0;
    SetDescBoxText(L" ");
    Clear();

    if(LastActiveCategory == -1)
    {
        if(ActiveTabNumber == 0)
        {
            for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
            {
                if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
                {
                    if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0)
                    {
                        i++;
                        m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                              m_CustomDataManager->m_RefAchievement[it->first].Category,
                                              m_CustomDataManager->m_RefAchievement[it->first].Name);
                        m_scroll->AddItem(m_slots[i], 1, 0);
                    }
                }

            }
        }
        else if(ActiveTabNumber == 1)
        {
            for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
            {

                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
    else
    {
        if(ActiveTabNumber == 0)
        {
            for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
            {
                if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
                {
                    if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                       && m_CustomDataManager->m_RefAchievement[it->first].Category == LastActiveCategory)
                    {
                        i++;
                        m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                              m_CustomDataManager->m_RefAchievement[it->first].Category,
                                              m_CustomDataManager->m_RefAchievement[it->first].Name);
                        m_scroll->AddItem(m_slots[i], 1, 0);
                    }
                }

            }
        }
        else if(ActiveTabNumber == 1)
        {
            for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
            {
                if(it->second.Category == LastActiveCategory)
                {
                    i++;
                    m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }
        }
    }

}
void CIFAchievements::On_BtnClickAll(){
    int i = 0;
    LastActiveCategory = -1;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                          m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

        }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {

                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
        }
    }
}
void CIFAchievements::On_BtnClickGeneral()
{
    LastActiveCategory = 0;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    int i = 0;
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                   && m_CustomDataManager->m_RefAchievement[it->first].Category == 0)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                          m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

        }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {
            if(it->second.Category == 0)
            {
                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
}
void CIFAchievements::On_BtnClickQuest(){
    LastActiveCategory = 1;
    int i = 0;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                   && m_CustomDataManager->m_RefAchievement[it->first].Category == 1)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                          m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

        }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {
            if(it->second.Category == 1)
            {
                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
}
void CIFAchievements::On_BtnClickUnique(){
    int i = 0;
    LastActiveCategory = 2;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                   && m_CustomDataManager->m_RefAchievement[it->first].Category == 2)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                            m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

       }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {
            if(it->second.Category == 2)
            {
                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
}
void CIFAchievements::On_BtnClickMatch(){
    LastActiveCategory = 3;
    int i = 0;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                   && m_CustomDataManager->m_RefAchievement[it->first].Category == 3)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                          m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

        }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {
            if(it->second.Category == 3)
            {
                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
}
void CIFAchievements::On_BtnClickEvent(){
    LastActiveCategory = 4;
    int i = 0;
    Clear();
    ClearDDJ();
    SetDescBoxText(L" ");
    if(ActiveTabNumber == 0)
    {
        for (std::map<int, byte>::iterator it = m_Player->m_Achievements.begin(); it != m_Player->m_Achievements.end(); ++it)
        {
            if(m_CustomDataManager->m_RefAchievement.find(it->first) != m_CustomDataManager->m_RefAchievement.end())
            {
                if(it->second != 0 && m_CustomDataManager->m_RefAchievement[it->first].RewardType == 0
                   && m_CustomDataManager->m_RefAchievement[it->first].Category == 4)
                {
                    i++;
                    m_slots[i]->WriteLine(m_CustomDataManager->m_RefAchievement[it->first].ID,
                                          m_CustomDataManager->m_RefAchievement[it->first].Category,
                                          m_CustomDataManager->m_RefAchievement[it->first].Name);
                    m_scroll->AddItem(m_slots[i], 1, 0);
                }
            }

        }
    }
    else if(ActiveTabNumber == 1)
    {
        for (std::map<int, CustomDataManager::Achievements>::iterator it = m_CustomDataManager->m_RefAchievement.begin(); it != m_CustomDataManager->m_RefAchievement.end(); ++it)
        {
            if(it->second.Category == 4)
            {
                i++;
                m_slots[i]->WriteLine(it->second.ID, it->second.Category,it->second.Name);
                m_scroll->AddItem(m_slots[i], 1, 0);
            }
        }
    }
}

void CIFAchievements::On_BtnClickRemove(){
    CMsgStreamBuffer buf(0x169A);
    buf << BYTE(6);
    SendMsg(buf);
    RemoveButtonDelay(7500);
}
#define GDR_BUTTON_REMOVETIMER 13513
#define GDR_BUTTON_USETIMER 13514
void CIFAchievements::RemoveButtonDelay(int timeoutSeconds) {
    this->m_IRM.GetResObj<CIFButton>(GDR_BUTTON_REMOVE, 1)->SetEnabledState(0);
    this->StartTimer(GDR_BUTTON_REMOVETIMER, timeoutSeconds);
}
void CIFAchievements::UseButtonDelay(int timeoutSeconds) {
    this->m_IRM.GetResObj<CIFButton>(GDR_BUTTON_USE, 1)->SetEnabledState(0);
    this->StartTimer(GDR_BUTTON_USETIMER, timeoutSeconds);
}
void CIFAchievements::OnTimer(int timerId) {
    if (timerId == GDR_BUTTON_REMOVETIMER)
    {
        this->KillTimer(GDR_BUTTON_REMOVETIMER);
        this->m_IRM.GetResObj<CIFButton>(GDR_BUTTON_REMOVE, 1)->SetEnabledState(1);
    }
    if(timerId == GDR_BUTTON_USETIMER)
    {
        this->KillTimer(GDR_BUTTON_USETIMER);
        this->m_IRM.GetResObj<CIFButton>(GDR_BUTTON_USE, 1)->SetEnabledState(1);
    }
}