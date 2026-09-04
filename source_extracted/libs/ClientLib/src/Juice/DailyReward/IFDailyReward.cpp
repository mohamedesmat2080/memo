#include "IFDailyReward.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFDailyReward, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFDailyReward, CIFMainFrame)
GFX_END_MESSAGE_MAP()
CIFDailyReward::CIFDailyReward(void)
{
}
CIFDailyReward::~CIFDailyReward(void)
{
}
bool CIFDailyReward::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,746,532 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    m_slot = new CIFDailyRewardSlot*[6];
    RECT m_slotRect = {53, 57, 229, 243};
    for(int i =0; i<6;i++) {
        m_slot[i] = (CIFDailyRewardSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFDailyRewardSlot), m_slotRect, 3 + i, 0);
        m_slotRect.left += 240;
        if((i+1)%3 == 0)
        {
            m_slotRect.top = 317;
            m_slotRect.left = 53;
        }
        m_slot[i]->ShowGWnd(false);
    }

    RECT m_spinRect = {380,575,50,16};
    m_spin = (CIFSpinButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSpinButton), m_spinRect, 23, 0);
    void* aggs = union_cast<void*, bool(CIFDailyReward::*)(int type)>(&CIFDailyReward::OnPageChange);
    m_spin->SetOnChangeFun(this, aggs);

    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    SetText(L"Daily Reward");
    return true;
}
void CIFDailyReward::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
void CIFDailyReward::OnUpdate() {

}
bool CIFDailyReward::OnPageChange(int type)
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
            if((double)totalCount/6 > m_spin->index)
                m_spin->index++;
            else
                return false;
            break;
    }

    CMsgStreamBuffer buf(0x181D);
    buf << BYTE(0x00) << m_spin->index;
    SendMsg(buf);
}
void CIFDailyReward::ClearRecords()
{
    for(int i=0;i<6;i++)
    {
        m_slot[i]->ShowGWnd(false);
    }
}
