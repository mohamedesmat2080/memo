#include "IFDpsSlot.h"
#include <Game.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFDpsSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFDpsSlot, CIFWnd)
GFX_END_MESSAGE_MAP()
CIFDpsSlot::CIFDpsSlot(void)
{
}
CIFDpsSlot::~CIFDpsSlot(void)
{
}
bool CIFDpsSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_iconRect = {2,2,20,20};
    m_icon = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_iconRect, 0, 0);
    m_icon->TB_Func_13("interface\\mall\\mall_hot_icon.ddj",0,0);

    RECT m_playernameRect = {220,0,76,24};
    m_playername = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_playernameRect, 1, 0);
    m_playername->m_FontTexture.SetColor(0xFFA500);

    RECT m_dmgRect = {14,0,55,24};
    m_dmg = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_dmgRect, 2, 0);
    m_dmg->m_FontTexture.SetColor(0xFFA500);
    return true;
}
void CIFDpsSlot::OnUpdate() {

}
