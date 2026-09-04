#include "IFPartyInfo.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFPartyInfo, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFPartyInfo, CIFMainFrame)
GFX_END_MESSAGE_MAP()
CIFPartyInfo::CIFPartyInfo(void)
{
}
CIFPartyInfo::~CIFPartyInfo(void)
{
}
bool CIFPartyInfo::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 31,44,645,267 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 32,89,645,239 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 25, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = { 11,43,680,283 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 3, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_stretchRect = {22,182,664,125};
    m_stretch = (CIFStretchWnd*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStretchWnd), m_stretchRect, 5, 0);
    m_stretch->SetTexture("interface\\ifcommon\\com_blacksquare_");
    m_stretch->sub_652DF0(0xFF000000);

    RECT m_headRect = {23,60,220,24};
    m_head= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_headRect, 17, 0);
    m_head->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head->SetText(L"Name");

    RECT m_head2Rect = {243,60,140,24};
    m_head2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 18, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head2->SetText(L"Guild");

    RECT m_head3Rect = {383,60,76,24};
    m_head3= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 19, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"Status");

    RECT m_head4Rect = {461,60,150,24};
    m_head4= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head4Rect, 20, 0);
    m_head4->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head4->SetText(L"Region");

    RECT m_head5Rect = {611,60,76,24};
    m_head5= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head5Rect, 21, 0);
    m_head5->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head5->SetText(L"Move");

    m_slots = new CIFPartyInfoSlot* [8];
    for(int i=0; i<8; i++)
    {
        RECT m_slotsRect = { 23,82,659 ,30 };
        m_slotsRect.top += 28 * i;
        m_slots[i] = (CIFPartyInfoSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFPartyInfoSlot), m_slotsRect, 6+i, 0);
    }
    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    MoveToCenter();
    ShowGWnd(false);
    SetText(L"Move to party member");
    return true;
}
void CIFPartyInfo::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}

void CIFPartyInfo::OnUpdate() {
}
void CIFPartyInfo::ClearRecords()
{
    for(int i=0; i<8; i++)
    {
        m_slots[i]->m_playername->SetText(L"");
        m_slots[i]->m_guildname->SetText(L"");
        m_slots[i]->m_status->SetText(L"");
        m_slots[i]->m_region->SetText(L"");
        m_slots[i]->m_movebtn->ShowGWnd(false);
    }
}
