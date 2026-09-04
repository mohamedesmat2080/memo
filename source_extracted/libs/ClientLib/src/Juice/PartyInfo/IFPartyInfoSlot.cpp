#include "IFPartyInfoSlot.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include "IFPartyInfo.h"
#include <BSLib/multibyte.h>
#include <GInterface.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFPartyInfoSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFPartyInfoSlot, CIFWnd)
            ONG_COMMAND(7, &CIFPartyInfoSlot::OnMove)
GFX_END_MESSAGE_MAP()
CIFPartyInfoSlot::CIFPartyInfoSlot(void)
{
}
CIFPartyInfoSlot::~CIFPartyInfoSlot(void)
{
}
bool CIFPartyInfoSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_itemTextRect = { 0,9,220,13 };
    m_playername = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_itemTextRect, 3, 0);

    RECT m_fromTextRect = { 220,9,140,17 };
    m_guildname = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_fromTextRect, 4, 0);

    RECT m_countTextRect = { 360,9,76,13 };
    m_status = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_countTextRect, 5, 0);

    RECT m_dateTextRect = { 437,9,150,13 };
    m_region = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_dateTextRect, 6, 0);

    RECT m_takeBtnRect = { 593,5,65,20 };
    m_movebtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_takeBtnRect, 7, 0);
    m_movebtn->SetText(L"Move");
    m_movebtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_movebtn->ShowGWnd(false);


    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);
    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFPartyInfoSlot::OnUpdate() {
}

void CIFPartyInfoSlot::OnMove()
{
    CIFPartyInfo* partyWnd = (CIFPartyInfo*) g_pCGInterface->GetWindowByUniqueID(13401);

    CMsgStreamBuffer buf(0x182A);
    buf << BYTE(partyWnd->invSlot);
    buf << std::n_string(TO_NSTRING(playername));
    SendMsg(buf);

    partyWnd->ShowGWnd(false);
}