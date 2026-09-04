#include "IFEventScheduleSlot.h"

GFX_IMPLEMENT_DYNCREATE(CIFEventScheduleSlot, CIFWnd)

CIFEventScheduleSlot::CIFEventScheduleSlot(void)
{
}
CIFEventScheduleSlot::~CIFEventScheduleSlot(void)
{
}
bool CIFEventScheduleSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_uniquetextRect = { 0,7,180,13 };
    m_eventtext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_uniquetextRect, 3, 0);

    RECT m_datetextRect = { 180,5,200,17 };
    m_datetext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_datetextRect, 4, 0);


    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);

    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFEventScheduleSlot::OnUpdate() {

}