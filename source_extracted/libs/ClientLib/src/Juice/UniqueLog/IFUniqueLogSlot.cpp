#include "IFUniqueLogSlot.h"

GFX_IMPLEMENT_DYNCREATE(CIFUniqueLogSlot, CIFWnd)

CIFUniqueLogSlot::CIFUniqueLogSlot(void)
{
}
CIFUniqueLogSlot::~CIFUniqueLogSlot(void)
{
}
bool CIFUniqueLogSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_uniquetextRect = { 0,7,160,13 };
    m_uniquetext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_uniquetextRect, 3, 0);

    RECT m_killertextRect = { 160,5,140,17 };
    m_killertext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_killertextRect, 4, 0);

    RECT m_statetextRect = { 300,7,76,13 };
    m_statetext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_statetextRect, 5, 0);

    RECT m_datetextRect = { 377,7,150,13 };
    m_datetext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_datetextRect, 6, 0);

    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);

    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFUniqueLogSlot::OnUpdate() {

}
