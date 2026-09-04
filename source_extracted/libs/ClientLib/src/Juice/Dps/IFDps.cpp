#include "IFDps.h"
#include <Game.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFDps, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFDps, CIFWnd)
            ONG_COMMAND(11, &CIFDps::OnToggle)
GFX_END_MESSAGE_MAP()

CIFDps::CIFDps(void)
{
    //printf("> " __FUNCTION__ "\n");
}


CIFDps::~CIFDps(void)
{
    // printf("> " __FUNCTION__ "\n");
}

bool CIFDps::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);

    RECT m_wndRect = {0,0,300,240};
    m_wnd = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_wndRect, 0, 0);
    m_wnd->TB_Func_13("juicer\\dps\\dps_wnd.ddj",0,0);
    getControls.push_back(m_wnd);
    m_slots = new CIFDpsSlot*[6];

    RECT m_dpsSlotRect = {4,48,296,24};
    for(int i=0;i<6;i++)
    {
        m_dpsSlotRect.top =48 + (29+i)*i;
        if(i==4)
            m_dpsSlotRect.top -=2;
        if(i==5)
        {
            m_dpsSlotRect.top-=7;
            m_dpsSlotRect.left = 3;
        }

        m_slots[i] = (CIFDpsSlot*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFDpsSlot), m_dpsSlotRect, 2+i, 0);
        getControls.push_back(m_slots[i]);
    }

    RECT m_iconRect = {9,11,24,24};
    m_icon = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_iconRect, 9, 0);
    m_icon->TB_Func_13("interface\\targetwindow\\tw_icon_unique.ddj",0,0);
    getControls.push_back(m_icon);

    RECT m_titleRect = {0,0,280,48};
    m_title = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_titleRect, 10, 0);
    m_title->SetFont(theApp.GetFont(2));
    m_title->m_FontTexture.SetColor(0xff0000);
    getControls.push_back(m_title);

    RECT m_toggleRect = {102,240,96,12};
    m_toggle = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_toggleRect, 11, 0);
    m_toggle->TB_Func_13("juicer\\dps\\com_side02_button.ddj",0,0);
    MoveGWnd(CLIENT_SCREEN_WIDTH-450,-1);

    ShowGWnd(false);
    return true;
}

void CIFDps::OnUpdate(){

}
void CIFDps::OnToggle() {
    if(m_toggle->GetPos().y > 0)
    {
        for(std::list<CIFWnd*>::iterator it = getControls.begin(); it != getControls.end();it++)
            (*it)->ShowGWnd(false);

        m_toggle->MoveGWnd(m_toggle->GetPos().x,0);

        m_toggle->TB_Func_13("juicer\\dps\\com_side_button.ddj",0,0);
    }
    else
    {
        for(std::list<CIFWnd*>::iterator it = getControls.begin(); it != getControls.end();it++)
            (*it)->ShowGWnd(true);

        m_toggle->TB_Func_13("juicer\\dps\\com_side02_button.ddj",0,0);

        m_toggle->MoveGWnd(m_toggle->GetPos().x,240);
    }
}
void CIFDps::ClearRecords() {
    for(int i=0;i<6;i++)
    {
        m_slots[i]->m_playername->SetText(L"");
        m_slots[i]->m_dmg->SetText(L"");
    }
}