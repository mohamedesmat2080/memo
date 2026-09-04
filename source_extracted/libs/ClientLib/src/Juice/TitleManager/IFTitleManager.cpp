#include "IFTitleManager.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GameDataExt.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>
#include <ICPlayer.h>

GFX_IMPLEMENT_DYNCREATE(CIFTitleManager, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFTitleManager, CIFMainFrame)
            ONG_BSSig_u38(2, 0x806D, 48, &CIFTitleManager::OnSelect)
            ONG_COMMAND(46, &CIFTitleManager::OnSet)
            ONG_COMMAND(47, &CIFTitleManager::OnRemove)
            ONG_COMMAND(19, &CIFTitleManager::OnTab)
            ONG_COMMAND(20, &CIFTitleManager::OnTab)

GFX_END_MESSAGE_MAP()
CIFTitleManager::CIFTitleManager(void)
{
}
CIFTitleManager::~CIFTitleManager(void)
{
}
bool CIFTitleManager::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,507,405 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 20,57,529,117 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 52, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = { 11,57,549,120 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 32, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd03_", 0, 0);

    RECT m_tile3Rect = { 20,184,270,267 };
    m_tile3 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile3Rect, 35, 0);
    m_tile3->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frame2Rect = { 11,184,290,270 };
    m_frame2 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame2Rect, 33, 0);
    m_frame2->TB_Func_13("interface\\frame\\mall_sub_wnd03_", 0, 0);

    RECT m_tile4Rect = { 308,184,250,267 };
    m_tile4 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile4Rect, 36, 0);
    m_tile4->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frame3Rect = { 308,184,250,270 };
    m_frame3 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame3Rect, 34, 0);
    m_frame3->TB_Func_13("interface\\frame\\mall_sub_wnd03_", 0, 0);

    RECT m_headRect = {23,192,246,24};
    m_head= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_headRect, 17, 0);
    m_head->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head->SetText(L"Title Name");

    RECT m_head2Rect = {268,192,18,24};
    m_head2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 18, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);

    RECT m_text1Rect = {200,83,150,24};
    m_text1= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_text1Rect, 30, 0);
    m_text1->SetText(L"Current custom title name");

    RECT m_text2Rect = {228,120,75,24};
    m_text2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_text2Rect, 31, 0);
    m_text2->m_FontTexture.SetColor(0xffff00);

    RECT m_buynewBtnRect = {441,142,100,24};
    m_buynewBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_buynewBtnRect, 40, 0);
    m_buynewBtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_buynewBtn->SetText(L"Buy New");

    RECT m_verticalRect = {270,230,14,179};
    m_vericalSroll = (CIFVerticalScroll*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFVerticalScroll), m_verticalRect, 41, 0);

    CIFButton* up = m_vericalSroll->m_IRM.GetResObj<CIFButton>(0,1);
    if(up) up->TB_Func_13("interface\\mall\\mall_scroll_up.ddj",0,0);

    CIFButton* down = m_vericalSroll->m_IRM.GetResObj<CIFButton>(1,1);
    if(down) down->TB_Func_13("interface\\mall\\mall_scroll_down.ddj",0,0);

    CIFButton* bar = m_vericalSroll->m_IRM.GetResObj<CIFButton>(2,1);
    if(bar){bar->TB_Func_13("interface\\mall\\mall_scroll_bar.ddj",0,0);bar->SetGWndSize(bar->GetSize().width-1,bar->GetSize().height);}

    RECT m_head3Rect = {320,192,206,24};
    m_head3= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 43, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"Description");

    RECT m_head4Rect = {525,192,18,24};
    m_head4= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head4Rect, 44, 0);
    m_head4->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);


    RECT m_vertical2Rect = {528,230,14,179};
    m_vericalSroll2 = (CIFVerticalScroll*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFVerticalScroll), m_vertical2Rect, 45, 0);

    up = m_vericalSroll2->m_IRM.GetResObj<CIFButton>(0,1);
    if(up) up->TB_Func_13("interface\\mall\\mall_scroll_up.ddj",0,0);

    down = m_vericalSroll2->m_IRM.GetResObj<CIFButton>(1,1);
    if(down) down->TB_Func_13("interface\\mall\\mall_scroll_down.ddj",0,0);

    bar = m_vericalSroll2->m_IRM.GetResObj<CIFButton>(2,1);
    if(bar){bar->TB_Func_13("interface\\mall\\mall_scroll_bar.ddj",0,0);bar->SetGWndSize(bar->GetSize().width-1,bar->GetSize().height);}

    RECT m_setBtnRect = {335,419,175,24};
    m_setBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_setBtnRect, 46, 0);
    m_setBtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_setBtn->SetText(L"Set");
    m_setBtn->SetEnabledState(false);


    RECT m_removeBtnRect = {27,142,100,24};
    m_removeBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_removeBtnRect, 47, 0);
    m_removeBtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_removeBtn->SetText(L"Remove");
    m_removeBtn->SetEnabledState(false);

    m_slots = new CIFTitleManagerSlot* [10];
    for(int i=0; i<10; i++)
    {
        RECT m_slotsRect = { 23,214,240,24 };
        m_slotsRect.top += 22 * i;
        m_slots[i] = (CIFTitleManagerSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFTitleManagerSlot), m_slotsRect, 6+i, 0);
    }

    m_Tabs = new CIFSelectableArea * [2];
    for (int i = 0; i < 2; i++) {
        RECT m_tabsRect = { 23,36,106,24 };
        m_tabsRect.left += 109*i;
        m_Tabs[i] = (CIFSelectableArea*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSelectableArea),m_tabsRect, 19 + i, 0);
        m_Tabs[i]->SetFont(this->N00009C2F);
        m_Tabs[i]->sub_64CE30("interface\\mall\\mall_sub_tab_on.ddj",
                               "interface\\mall\\mall_sub_tab_off.ddj", "interface\\mall\\mall_sub_tab_off.ddj");
        m_Tabs[i]->sub_64CC30(0);
    }
    m_Tabs[0]->sub_64CC30(1);
    m_Tabs[0]->SetText(L"Titles");
    m_Tabs[1]->SetText(L"Achievements");

    RECT m_textboxRect = {30,218,238,240};
    m_textbox = (CIFTextBox*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFTextBox), m_textboxRect, 48, 0);
    m_textbox->N00000608 = this;
    m_textbox->m_HeightInLines = 10;
    m_textbox->SetLineHeight(22);
    m_textbox->sub_638B50(0);
    m_textbox->sub_64E380(0);
    m_textbox->sub_638C70(1);
    m_textbox->SetHightlineLine(true);
    m_textbox->sub_638D50(1);
    m_textbox->SetHighlightColor(D3DCOLOR_RGBA(62, 50, 16, 255));
    m_textbox->LinkScrollbar(m_vericalSroll);
    m_textbox->m_FontTexture.sub_8B4400(0, 0);
    m_textbox->m_FontTexture.sub_8B4750(0);
    m_textbox->TB_Func_5(0);
    m_textbox->TB_Func_6(0);


    RECT m_textbox2Rect = {327,218,200,200};
    m_textbox2 = (CIFTextBox*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFTextBox), m_textbox2Rect, 49, 0);
    m_textbox2->N00000608 = this;
    m_textbox2->m_HeightInLines = 14;
    m_textbox2->SetLineHeight(14);
    m_textbox2->sub_638B50(0);
    m_textbox2->sub_64E380(0);
    m_textbox2->sub_638C70(1);
    m_textbox2->SetHightlineLine(true);
    m_textbox2->sub_638D50(1);
    m_textbox2->SetHighlightColor(D3DCOLOR_RGBA(62, 50, 16, 255));
    m_textbox2->LinkScrollbar(m_vericalSroll2);
    m_textbox2->m_FontTexture.sub_8B4400(0, 0);
    m_textbox2->m_FontTexture.sub_8B4750(0);
    m_textbox2->TB_Func_5(0);
    m_textbox2->TB_Func_6(0);

    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    SetText(L"Title Manager");
    return true;
}
void CIFTitleManager::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}

void CIFTitleManager::OnUpdate() {

}

void CIFTitleManager::OnSelect(int a1 , int a2)
{

    CIFListCtrl* pList;
    pList = m_textbox;

    CIFListCtrl::SLineOfText* line = pList->sub_63A940();

    std::n_wstring str;
    line->m_font->GetText(&str);

    std::n_wstring a;
    selectedTitle = str;

    if(m_buynewBtn->IsVisible())
    {
        wchar_t* Dest = new wchar_t[100];
        wsprintfW(Dest, L"Selected title:\n          %s", str.c_str());
        a = std::n_wstring(Dest);
    }
    else
        a = achievements[str].Descr;

    int numtoDel = m_textbox2->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox2->RemoveLine(0);
    }

    m_textbox2->sub_64F8A0(a, 0, 0xffffff, 0xffffff, -1, 0, 0);
    if(!m_buynewBtn->IsVisible())
       m_setBtn->SetEnabledState(achievements[str].isEnabled);
    else
        m_setBtn->SetEnabledState(true);

    ClearEnabledStates();
    int firstAppearIndex = *(int*)((DWORD32)pList + 0x380);
    int lastClickedIndex = *(int *)((DWORD32)pList + 0x374);
    m_slots[lastClickedIndex-firstAppearIndex]->SetEnableState(true);
}
void CIFTitleManager::ClearEnabledStates()
{
    for(int i=0; i<10; i++)
    {
        m_slots[i]->SetEnableState(false);
    }
}
void CIFTitleManager::OnSet()
{
    if(m_buynewBtn->IsVisible())
    {
        CMsgStreamBuffer buf(0x181A);
        buf << BYTE(0x01) << TO_NSTRING(selectedTitle);
        SendMsg(buf);
    }
    else
    {
        CMsgStreamBuffer buf(0x181B);
        buf << BYTE(0x01) << TO_NSTRING(selectedTitle);
        SendMsg(buf);
    }

    m_text2->SetText(selectedTitle.c_str());
    m_removeBtn->SetEnabledState(true);

    ClearEnabledStates();

    int numtoDel = m_textbox2->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox2->RemoveLine(0);
    }

    m_setBtn->SetEnabledState(false);
}
void CIFTitleManager::OnRemove()
{
    if(m_buynewBtn->IsVisible())
    {
        CMsgStreamBuffer buf(0x181A);
        buf << BYTE(0x02);
        SendMsg(buf);
    }
    else
    {
        CMsgStreamBuffer buf(0x181B);
        buf << BYTE(0x02);
        SendMsg(buf);
    }
    m_text2->SetText(L"<No Title>");

    m_removeBtn->SetEnabledState(false);
}
void CIFTitleManager::OnTab() {
    int tabID = GetIDOfInterfaceUnderCursor();
    if (tabID == m_Tabs[0]->UniqueID())
    {
        ShowTitleWnd();
        m_Tabs[0]->sub_64CC30(1);
        m_Tabs[1]->sub_64CC30(0);

    }
    else if(tabID == m_Tabs[1]->UniqueID())
    {
        ShowAchievementWnd();
        m_Tabs[1]->sub_64CC30(1);
        m_Tabs[0]->sub_64CC30(0);

    }
}
void CIFTitleManager::ShowTitleWnd()
{
    ClearEnabledStates();
    m_setBtn->SetEnabledState(false);
    m_buynewBtn->ShowGWnd(true);
    m_text1->SetText(L"Current custom title name");

    int numtoDel = m_textbox2->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox2->RemoveLine(0);
    }

    numtoDel = m_textbox->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox->RemoveLine(0);
    }

    std::map<std::n_wstring, std::n_wstring>::iterator findCustomTitle = m_CustomTitle.find(g_pCICPlayer->GetName().c_str());

    if(findCustomTitle != m_CustomTitle.end())
    {
        wchar_t buffer[255];
        std::n_wstring mymsg;
        swprintf_s(buffer, sizeof(buffer), L"%s", findCustomTitle->second.c_str());
        mymsg.assign(buffer);
        m_text2->SetText(mymsg.c_str());
        m_removeBtn->SetEnabledState(true);
    }
    else
    {
        m_removeBtn->SetEnabledState(false);
        m_text2->SetText(L"<No Title>");
    }
    std::map<std::n_wstring, int>::iterator findCustomTitleColor = m_CustomTitleColor.find(g_pCICPlayer->GetName().c_str());
    if(findCustomTitleColor != m_CustomTitleColor.end())
    {
        m_text2->m_FontTexture.SetColor(findCustomTitleColor->second);
    }
    else
        m_text2->m_FontTexture.SetColor(0xffff00);

    CMsgStreamBuffer buf(0x181A);
    buf << BYTE(0x00);
    SendMsg(buf);
}
void CIFTitleManager::ShowAchievementWnd()
{
    ClearEnabledStates();
    m_setBtn->SetEnabledState(false);
    m_buynewBtn->ShowGWnd(false);
    m_text1->SetText(L"Current custom rank name");

    int numtoDel = m_textbox2->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox2->RemoveLine(0);
    }

    numtoDel = m_textbox->GetNumberOfItems() +1;
    for(int i =0;i<=numtoDel;i++) {

        m_textbox->RemoveLine(0);
    }

    std::map<std::n_wstring, std::n_wstring>::iterator findCharName = m_CharName.find(g_pCICPlayer->GetName().c_str());

    if(findCharName != m_CharName.end())
    {
        wchar_t buffer[255];
        std::n_wstring mymsg;
        swprintf_s(buffer, sizeof(buffer), L"%s", findCharName->second.c_str());
        mymsg.assign(buffer);
        m_text2->SetText(mymsg.c_str());
        m_removeBtn->SetEnabledState(true);
    }
    else
    {
        m_text2->SetText(L"<No Rank>");
        m_removeBtn->SetEnabledState(false);
    }

    std::map<std::n_wstring, int>::iterator findCharNameColor =  m_CharNameColor.find(g_pCICPlayer->GetName().c_str());
    if(findCharNameColor != m_CharNameColor.end())
    {
        m_text2->m_FontTexture.SetColor(findCharNameColor->second);
    }
    else
        m_text2->m_FontTexture.SetColor(0xffffff);

    CMsgStreamBuffer buf(0x181B);
    buf << BYTE(0x00);
    SendMsg(buf);
}