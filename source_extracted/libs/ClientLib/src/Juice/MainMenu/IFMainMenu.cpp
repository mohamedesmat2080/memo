#include "IFMainMenu.h"
#include <BSLib/multibyte.h>
#include <GInterface.h>
#include <GameDataExt.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>
#include <ICPlayer.h>
#include <IFChatViewer.h>
#include <Juice/CharRank/IFCharRank.h>
#include <Juice/CustomsMarket/IFCustomsMarket.h>
#include <Juice/DailyReward/IFDailyReward.h>
#include <Juice/EventSchedule/IFEventSchedule.h>
#include <Juice/TitleManager/IFTitleManager.h>
#include <Juice/UniqueLog/IFUniqueLog.h>
#include <sstream>

GFX_IMPLEMENT_DYNCREATE(CIFMainMenu, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFMainMenu, CIFWnd)
            ONG_COMMAND(1, &CIFMainMenu::OnToggle)
            ONG_COMMAND(7, &CIFMainMenu::OnRank)
            ONG_COMMAND(8, &CIFMainMenu::OnUnique)
            ONG_COMMAND(9, &CIFMainMenu::OnEvent)
            ONG_COMMAND(10, &CIFMainMenu::OnTitle)
            ONG_COMMAND(11, &CIFMainMenu::OnMarket)
            ONG_COMMAND(12, &CIFMainMenu::OnDaily)
GFX_END_MESSAGE_MAP()

CIFMainMenu::CIFMainMenu(void)
{
    //printf("> " __FUNCTION__ "\n");
}


CIFMainMenu::~CIFMainMenu(void)
{
    //printf("> " __FUNCTION__ "\n");
}
bool CIFMainMenu::OnCreate(long ln)
{
    CIFWnd::OnCreate(ln);

    RECT toggleBtnRect = { 0,0,12,96 };
    toggleBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), toggleBtnRect, 1, 0);
    toggleBtn->TB_Func_13("interface\\ifcommon\\com_side02_button.ddj", 0, 0);

    int menuPosY = -(243/2);
    int menuPosX = -231;
    RECT mainMenuRect = { menuPosX,menuPosY,243,395 };
    mainMenu = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), mainMenuRect, 2, 0);
    mainMenu->TB_Func_13("interface\\mall\\mall_pre_characterview.ddj", 0, 0);
    mainMenu->ShowGWnd(false);
    getControls.push_back(mainMenu);

    RECT circlePicRect = { menuPosX+17,menuPosY+8,73,73 };
    circlePic = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), circlePicRect, 3, 0);
    circlePic->ShowGWnd(false);
    getControls.push_back(circlePic);

    RECT charTypeRect = { menuPosX+68,menuPosY+56,24,24 };
    charType = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), charTypeRect, 4, 0);
    charType->ShowGWnd(false);
    getControls.push_back(charType);

    RECT charNameRect = { menuPosX+105,menuPosY+19,123,21 };
    charName = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), charNameRect, 5, 0);
    charName->TB_Func_13("juicer\\extra\\custom_text_bg.ddj", 0, 0);
    charName->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    charName->ShowGWnd(false);
    getControls.push_back(charName);

    RECT charGuildRect = { menuPosX+105,menuPosY+52,123,21 };
    charGuild = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), charGuildRect, 6, 0);
    charGuild->TB_Func_13("juicer\\extra\\custom_text_bg.ddj", 0, 0);
    charGuild->SetText(L"<Egyptians>");
    charGuild->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    charGuild->ShowGWnd(false);
    getControls.push_back(charGuild);

    RECT rankBtnRect = { menuPosX+2,menuPosY+105,238,40 };
    rankBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), rankBtnRect, 7, 0);
    rankBtn->TB_Func_13("juicer\\menu\\button_ranking.ddj", 0, 0);
    rankBtn->SetText(L"     Character Ranking");
    rankBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    rankBtn->ShowGWnd(false);
    getControls.push_back(rankBtn);

    RECT uniqueBtnRect = { menuPosX+2,menuPosY+152,238,40 };
    uniqueBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), uniqueBtnRect, 8, 0);
    uniqueBtn->TB_Func_13("juicer\\menu\\button_unique.ddj", 0, 0);
    uniqueBtn->SetText(L"     Unique Logs");
    uniqueBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    uniqueBtn->ShowGWnd(false);
    getControls.push_back(uniqueBtn);

    RECT scheduleBtnRect = { menuPosX+2,menuPosY+198,238,40 };
    scheduleBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), scheduleBtnRect, 9, 0);
    scheduleBtn->TB_Func_13("juicer\\menu\\button_schedule.ddj", 0, 0);
    scheduleBtn->SetText(L"     Event Scheduling");
    scheduleBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    scheduleBtn->ShowGWnd(false);
    getControls.push_back(scheduleBtn);

    RECT titleBtnRect = { menuPosX+2,menuPosY+244,238,40 };
    titleBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), titleBtnRect, 10, 0);
    titleBtn->TB_Func_13("juicer\\menu\\button_title_storage.ddj", 0, 0);
    titleBtn->SetText(L"     Title Manager");
    titleBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    titleBtn->ShowGWnd(false);
    getControls.push_back(titleBtn);

    RECT marketBtnRect = { menuPosX+2,menuPosY+290,238,40 };
    marketBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), marketBtnRect, 11, 0);
    marketBtn->TB_Func_13("juicer\\menu\\button_title.ddj", 0, 0);
    marketBtn->SetText(L"     Title Market");
    marketBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    marketBtn->ShowGWnd(false);
    getControls.push_back(marketBtn);

    RECT dailyBtnRect = { menuPosX+2,menuPosY+336,238,40 };
    dailyBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), dailyBtnRect, 12, 0);
    dailyBtn->TB_Func_13("juicer\\menu\\button_daily.ddj", 0, 0);
    dailyBtn->SetText(L"     Daily Reward");
    dailyBtn->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 165, 0, 255));
    dailyBtn->ShowGWnd(false);
    getControls.push_back(dailyBtn);


    return true;
}
void CIFMainMenu::OnToggle() {
    int togglePosX = this->toggleBtn->GetPos().x;
    if(togglePosX == CLIENT_SCREEN_WIDTH-12)
        Toggle(true);
    else
        Toggle(false);
}
void CIFMainMenu::Toggle(bool show) {
    this->toggleBtn->MoveGWnd(show ? this->GetPos().x-243 : this->GetPos().x,this->GetPos().y);
    this->toggleBtn->TB_Func_13(show ? "interface\\ifcommon\\com_side_button.ddj" : "interface\\ifcommon\\com_side02_button.ddj", 0, 0);
    for(std::list<CIFWnd*>::iterator it = getControls.begin(); it != getControls.end();it++)
        (*it)->ShowGWnd(show);
    if(show)
    {
        int imgID = 0;
        std::string imgStr;

        DWORD32 refObjID = *(DWORD32 *) ((DWORD32) g_pCICPlayer + 0x21C);
        if(refObjID>= 1907 && refObjID <= 1919)
        {
            imgStr = "juicer\\character\\char_ch_man";
            imgID = 1920 - refObjID;
            charType->TB_Func_13("interface\\ifcommon\\com_kindred_china.ddj", 0, 0);
        }
        else if (refObjID >= 1920 && refObjID <=1932)
        {
            imgStr = "juicer\\character\\char_ch_woman";
            imgID = 1933 - refObjID;
            charType->TB_Func_13("interface\\ifcommon\\com_kindred_china.ddj", 0, 0);
        }
        else if(refObjID >= 14876 && refObjID <= 14888)
        {
            imgStr = "juicer\\character\\char_eu_man";
            imgID = 14889 - refObjID;
            charType->TB_Func_13("interface\\ifcommon\\com_kindred_europe.ddj", 0, 0);
        }
        else
        {
            imgStr = "juicer\\character\\char_eu_woman";
            charType->TB_Func_13("interface\\ifcommon\\com_kindred_europe.ddj", 0, 0);
            imgID = 14901 - refObjID;
        }

        std::ostringstream temp;
        temp << imgID;
        imgStr = imgStr + temp.str() + ".ddj";
        circlePic->TB_Func_13(imgStr.c_str(), 0, 0);

        wchar_t *NewMsg = new wchar_t[1024];
        wsprintfW(NewMsg, L"%s  Lv.%d", g_pCICPlayer->GetName().c_str(),g_pCICPlayer->GetCurrentLevel());
        charName->SetText(NewMsg);

        charGuild->SetText(g_pCICPlayer->GetGuildName().c_str());
    }
}
void CIFMainMenu::OnUpdate()
{

}
void CIFMainMenu::OnRank()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }
    CIFCharRank* rankWnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(13392);

    rankWnd->m_spin->index = 1;
    rankWnd->m_spin->numStatic->SetText(L"1");
    rankWnd->totalCount = 1;

    if(rankWnd->RankInfo.size() > 0)
    {
        rankWnd->m_popup->m_text->SetText(rankWnd->RankInfo.begin()->first.c_str());
        CMsgStreamBuffer buf(0x180C);
        buf << rankWnd->RankInfo.begin()->second << 0x01;
        SendMsg(buf);
    }
    rankWnd->BringToFront();
    rankWnd->ShowGWnd(true);

}
void CIFMainMenu::OnUnique()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }

    CIFUniqueLog* uniqueWnd = (CIFUniqueLog *) g_pCGInterface->GetWindowByUniqueID(13393);
    uniqueWnd->ShowGWnd(true);
    uniqueWnd->m_spin->index = 1;
    uniqueWnd->m_spin->numStatic->SetText(L"1");
    uniqueWnd->BringToFront();

    CMsgStreamBuffer buf(0x180D);
    buf << BYTE(0x01);
    SendMsg(buf);
}
void CIFMainMenu::OnEvent()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }

    CIFEventSchedule* eventWnd = (CIFEventSchedule *) g_pCGInterface->GetWindowByUniqueID(13394);
    eventWnd->ShowGWnd(true);
    eventWnd->m_spin->index = 1;
    eventWnd->m_spin->numStatic->SetText(L"1");
    eventWnd->BringToFront();

    CMsgStreamBuffer buf(0x180E);
    buf << BYTE(0x01);
    SendMsg(buf);
}
void CIFMainMenu::OnTitle()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }

    CIFTitleManager* titleWnd = (CIFTitleManager *) g_pCGInterface->GetWindowByUniqueID(13396);
    titleWnd->MoveToCenter();
    titleWnd->ShowTitleWnd();

    titleWnd->m_Tabs[0]->sub_64CC30(1);
    titleWnd->m_Tabs[1]->sub_64CC30(0);

    titleWnd->ShowGWnd(true);
    titleWnd->BringToFront();

}
void CIFMainMenu::OnMarket()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }

    CIFCustomsMarket* marketWnd = (CIFCustomsMarket *) g_pCGInterface->GetWindowByUniqueID(13397);
    marketWnd->MoveToCenter();

    marketWnd->m_buy1->SetEnabledState(true);
    marketWnd->m_buy2->SetEnabledState(true);
    marketWnd->m_buy3->SetEnabledState(true);

    marketWnd->ShowGWnd(true);
    marketWnd->BringToFront();
}
void CIFMainMenu::OnDaily()
{
    for(int i = 13392;i<= 13398;i++)
    {
        CGWnd* Wnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(i);
        if(Wnd)
            Wnd->ShowGWnd(false);
    }

    CIFDailyReward* dailyWnd = (CIFDailyReward *) g_pCGInterface->GetWindowByUniqueID(13398);
    dailyWnd->m_spin->index = 1;
    dailyWnd->m_spin->numStatic->SetText(L"1");
    dailyWnd->BringToFront();

    CMsgStreamBuffer buf(0x181D);
    buf << BYTE(0x00) << 0x01;
    SendMsg(buf);
    dailyWnd->ShowGWnd(true);
}