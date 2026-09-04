#include "IFUniqueLog.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFUniqueLog, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFUniqueLog, CIFMainFrame)
GFX_END_MESSAGE_MAP()
CIFUniqueLog::CIFUniqueLog(void)
{
}
CIFUniqueLog::~CIFUniqueLog(void)
{
}
bool CIFUniqueLog::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,504,296 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 33,89,503,268 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 24, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = { 11,57,546,312 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 3, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_stretchRect = {22,202,528,154};
    m_stretch = (CIFStretchWnd*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStretchWnd), m_stretchRect, 5, 0);
    m_stretch->SetTexture("interface\\ifcommon\\com_blacksquare_");
    m_stretch->sub_652DF0(0xFF000000);

    RECT m_headRect = {23,80,160,24};
    m_head= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_headRect, 17, 0);
    m_head->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head->SetText(L"Unique Name");

    RECT m_head2Rect = {183,80,140,24};
    m_head2= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 18, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head2->SetText(L"Killer");

    RECT m_head3Rect = {323,80,76,24};
    m_head3= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 19, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"State");

    RECT m_head4Rect = {400,80,150,24};
    m_head4= (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head4Rect, 20, 0);
    m_head4->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head4->SetText(L"Date");

    RECT m_spinRect = {245,336,50,16};
    m_spin = (CIFSpinButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSpinButton), m_spinRect, 23, 0);
    void* aggs = union_cast<void*, bool(CIFUniqueLog::*)(int type)>(&CIFUniqueLog::OnPageChange);
    m_spin->SetOnChangeFun(this, aggs);

    m_slots = new CIFUniqueLogSlot* [10];
    for(int i=0; i<10; i++)
    {
        RECT m_slotsRect = { 23,102,520,24 };
        m_slotsRect.top += 22 * i;
        m_slots[i] = (CIFUniqueLogSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFUniqueLogSlot), m_slotsRect, 6+i, 0);
    }
    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    SetText(L"Unique Logs (F5)");
    return true;
}
void CIFUniqueLog::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
bool CIFUniqueLog::OnPageChange(int type)
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
    CMsgStreamBuffer buf(0x180D);
    buf << m_spin->index;
    SendMsg(buf);
    return true;
}
void CIFUniqueLog::OnUpdate() {

}
void CIFUniqueLog::ClearRecords()
{
    for(int i=0; i<10; i++)
    {
        m_slots[i]->m_uniquetext->SetText(L"");
        m_slots[i]->m_killertext->SetText(L"");
        m_slots[i]->m_statetext->SetText(L"");
        m_slots[i]->m_datetext->SetText(L"");
    }
}