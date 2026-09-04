#include "IFTitleManagerSlot.h"

GFX_IMPLEMENT_DYNCREATE(CIFTitleManagerSlot, CIFWnd)

CIFTitleManagerSlot::CIFTitleManagerSlot(void)
{
}
CIFTitleManagerSlot::~CIFTitleManagerSlot(void)
{
}
bool CIFTitleManagerSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);

    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);

    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFTitleManagerSlot::OnUpdate() {

}
void CIFTitleManagerSlot::SetEnableState(bool state)
{
    if(!state)
    {
        TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
        r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);
        l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);
    }
    else
    {
        TB_Func_13("interface\\ifcommon\\com_bar01select_mid.ddj",0,0);
        r_edge->TB_Func_13("interface\\ifcommon\\com_bar01select_right.ddj",0,0);
        l_edge->TB_Func_13("interface\\ifcommon\\com_bar01select_left.ddj",0,0);
    }
}