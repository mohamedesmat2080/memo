#include "IFCharRankSlot.h"

GFX_IMPLEMENT_DYNCREATE(CIFCharRankSlot, CIFWnd)

CIFCharRankSlot::CIFCharRankSlot(void)
{
}
CIFCharRankSlot::~CIFCharRankSlot(void)
{
}
bool CIFCharRankSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_ranktextRect = { 10,7,38,13 };
    m_ranktext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_ranktextRect, 3, 0);

    RECT m_gradetextRect = { 66,5,17,17 };
    m_gradetext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_gradetextRect, 4, 0);

    RECT m_charnametextRect = { 110,7,126,13 };
    m_charnametext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_charnametextRect, 5, 0);

    RECT m_leveltextRect = { 239,7,42,13 };
    m_leveltext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_leveltextRect, 6, 0);

    RECT m_pointstextRect = { 320,7,22,13 };
    m_pointstext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_pointstextRect, 7, 0);

    RECT m_guildtextRect = { 363,7,110,13 };
    m_guildtext = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_guildtextRect, 8, 0);

    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);

    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFCharRankSlot::OnUpdate() {

}