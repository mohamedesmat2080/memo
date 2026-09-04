#include "IFItemChest.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFItemChest, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFItemChest, CIFMainFrame)
            ONG_COMMAND(24, &CIFItemChest::OnTakeAll)

GFX_END_MESSAGE_MAP()
CIFItemChest::CIFItemChest(void)
{
}
CIFItemChest::~CIFItemChest(void)
{
}
bool CIFItemChest::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,645,356 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 33,89,646,328 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 25, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = { 11,57,682,372 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 3, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_stretchRect = {22,202,664,214};
    m_stretch = (CIFStretchWnd*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStretchWnd), m_stretchRect, 5, 0);
    m_stretch->SetTexture("interface\\ifcommon\\com_blacksquare_");
    m_stretch->sub_652DF0(0xFF000000);

    RECT m_headRect = {23,80,220,24};
    m_head= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_headRect, 17, 0);
    m_head->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head->SetText(L"Item Info");

    RECT m_head2Rect = {243,80,140,24};
    m_head2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 18, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head2->SetText(L"From");

    RECT m_head3Rect = {383,80,76,24};
    m_head3= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 19, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"Count");

    RECT m_head4Rect = {461,80,150,24};
    m_head4= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head4Rect, 20, 0);
    m_head4->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head4->SetText(L"Date");

    RECT m_head5Rect = {611,80,76,24};
    m_head5= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head5Rect, 21, 0);
    m_head5->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head5->SetText(L"Take");

    RECT m_invTextRect = {50,397,113,24};
    m_invText = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_invTextRect, 22, 0);
    m_invText->SetText(L"Remaining inventory Slots: 0");

    RECT m_spinRect = {303,396,50,16};
    m_spin = (CIFSpinButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSpinButton), m_spinRect, 23, 0);
    void* aggs = union_cast<void*, bool(CIFItemChest::*)(int type)>(&CIFItemChest::OnPageChange);
    m_spin->SetOnChangeFun(this, aggs);

    RECT m_takeallBtnRect = {537,394,137,24};
    m_takeallBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_takeallBtnRect, 24, 0);
    m_takeallBtn->SetText(L"Take All");
    m_takeallBtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);

    m_slots = new CIFItemChestSlot* [10];
    for(int i=0; i<10; i++)
    {
        RECT m_slotsRect = { 23,102,659 ,30 };
        m_slotsRect.top += 28 * i;
        m_slots[i] = (CIFItemChestSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFItemChestSlot), m_slotsRect, 6+i, 0);
    }
    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    SetText(L"Item Chest (F7)");
    return true;
}
void CIFItemChest::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
bool CIFItemChest::OnPageChange(int type)
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
            if((double)totalCount/10 > m_spin->index)
                m_spin->index++;
            else
                return false;
            break;
    }
    CMsgStreamBuffer buf(0x180F);
    buf << byte(0x00) << m_spin->index;
    SendMsg(buf);
    return true;
}
void CIFItemChest::OnUpdate() {
    wchar_t* str = new wchar_t[200];
    int freeSlots = getEmptySlotsCount();
    wsprintfW(str, L"Remaining Inventory Slots: (%d)",freeSlots);
    this->m_invText->SetText(str);
}
void CIFItemChest::ClearRecords()
{
    for(int i=0; i<10; i++)
    {
        m_slots[i]->m_itemText->SetText(L"");
        m_slots[i]->m_dateText->SetText(L"");
        m_slots[i]->m_fromText->SetText(L"");
        m_slots[i]->m_countText->SetText(L"");
        m_slots[i]->m_takeBtn->ShowGWnd(false);
        m_slots[i]->m_itemInfo->ShowGWnd(false);
    }
}
void CIFItemChest::OnTakeAll()
{
    //make sure that atleast he has 1 item
    if(m_slots[0]->m_takeBtn->IsVisible())
    {
        CMsgStreamBuffer buf(0x180F);
        buf << byte(0x02) <<m_spin->index;
        SendMsg(buf);
    }
}