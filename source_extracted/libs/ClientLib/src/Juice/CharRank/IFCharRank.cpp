#include "IFCharRank.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFCharRank, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFCharRank, CIFMainFrame)
            ONG_COMMAND(27, &CIFCharRank::OnRankChange)
GFX_END_MESSAGE_MAP()
CIFCharRank::CIFCharRank(void)
{
}
CIFCharRank::~CIFCharRank(void)
{
}
bool CIFCharRank::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,471,442 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 33,89,470,414 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 29, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = { 11,57,513,458 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 3, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_stretchRect = {22,202,495,300};
    m_stretch = (CIFStretchWnd*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStretchWnd), m_stretchRect, 5, 0);
    m_stretch->SetTexture("interface\\ifcommon\\com_blacksquare_");
    m_stretch->sub_652DF0(0xFF000000);

    RECT m_headRect = {27,205,52,24};
    m_head= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_headRect, 17, 0);
    m_head->TB_Func_13("interface\\mall\\mall_subj_01.ddj", 0, 0);
    m_head->SetText(L"#");

    RECT m_head2Rect = {77,205,52,24};
    m_head2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 18, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_02.ddj", 0, 0);
    m_head2->SetText(L"Grade");

    RECT m_head3Rect = {130,205,128,24};
    m_head3= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 19, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"Charname");

    RECT m_head4Rect = {259,205,76,24};
    m_head4= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head4Rect, 20, 0);
    m_head4->TB_Func_13("interface\\mall\\mall_subj_01.ddj", 0, 0);
    m_head4->SetText(L"Level");

    RECT m_head5Rect = {333,205,56,24};
    m_head5= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head5Rect, 21, 0);
    m_head5->TB_Func_13("interface\\mall\\mall_subj_02.ddj", 0, 0);
    m_head5->SetText(L"Points");

    RECT m_head6Rect = {390,205,116,24};
    m_head6= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head6Rect, 22, 0);
    m_head6->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head6->SetText(L"Guild");

    RECT m_spinRect = {228,483,50,16};
    m_spin = (CIFSpinButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSpinButton), m_spinRect, 23, 0);
    void* aggs = union_cast<void*, bool(CIFCharRank::*)(int type)>(&CIFCharRank::OnPageChange);
    m_spin->SetOnChangeFun(this, aggs);

    RECT m_infobgRect = {25,66,479,116};
    m_infobg= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_infobgRect, 24, 0);
    m_infobg->TB_Func_13("interface\\mall\\mall_sub_box.ddj",0,0);

    RECT m_infotextRect = { 38,88,445,15 };
    m_infotext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_infotextRect, 25, 0);
    m_infotext->SetText(L"You can check the rank of the players, Set your rank and click at the button.");

    RECT m_searchtextRect = { 100,125,100,19 };
    m_searchtext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_searchtextRect, 26, 0);
    m_searchtext->SetText(L"Set Inquiry Rank");
    m_searchtext->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 249, 192, 45));

    RECT m_searchbtnRect = { 337,121,100,24 };
    m_searchbtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_searchbtnRect, 27, 0);
    m_searchbtn->TB_Func_13("interface\\mall\\mall_button.ddj",0,0);
    m_searchbtn->SetText(L"Load Rank");

    RECT m_stateRect = { 10,483,200,20 };
    m_state = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_stateRect, 28, 0);

    RECT m_popupRect = { 204,125,123,20 };
    m_popup = (CIFPopupList*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFPopupList), m_popupRect, 4, 0);


    m_slots = new CIFCharRankSlot* [11];
    for(int i=0; i<11; i++)
    {
        RECT m_slotsRect = { 27,227,474,24 };
        m_slotsRect.top += 22 * i;
        m_slots[i] = (CIFCharRankSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFCharRankSlot), m_slotsRect, 6+i, 0);
    }

    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    SetText(L"Character Ranking");
    return true;
}
void CIFCharRank::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
bool CIFCharRank::OnPageChange(int type)
{

    switch (type)
    {
        case 1:
            if (m_spin->index > 1)
                m_spin->index--;
            else
                return false;
            break;
        case 2:
            if((double)totalCount/11 > m_spin->index)
                m_spin->index++;
            else
                return false;
            break;
    }
    m_state->SetText(L"Loading ranks , please wait...");
    if(RankInfo.size() > 0)
    {
        CMsgStreamBuffer buf(0x180C);
        buf << RankInfo[m_popup->m_text->GetNText()] << m_spin->index;
        SendMsg(buf);
    }
    return true;
}
void CIFCharRank::OnUpdate() {

}
void CIFCharRank::ClearRecords()
{
    for(int i=0; i<11; i++)
    {
        m_slots[i]->m_ranktext->SetText(L"");
        m_slots[i]->m_charnametext->SetText(L"");
        m_slots[i]->m_gradetext->TB_Func_13("",0,0);
        m_slots[i]->m_leveltext->SetText(L"");
        m_slots[i]->m_pointstext->SetText(L"");
        m_slots[i]->m_guildtext->SetText(L"");
    }
}
void CIFCharRank::OnRankChange() {
    m_state->SetText(L"Loading ranks , please wait...");

    m_spin->numStatic->SetText(L"1");
    m_spin->index = 1;

    if(RankInfo.size() > 0)
    {
        CMsgStreamBuffer buf(0x180C);
        buf << RankInfo[m_popup->m_text->GetNText()] << 0x01;
        SendMsg(buf);
    }
}