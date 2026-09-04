#include "IFWheelFortune.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>
#include <ctime>
#include <sstream>

GFX_IMPLEMENT_DYNCREATE(CIFWheelFortune, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFWheelFortune, CIFMainFrame)
            ONG_COMMAND(6, &OnSpin)
GFX_END_MESSAGE_MAP()

CIFWheelFortune::CIFWheelFortune(void)
{
}
CIFWheelFortune::~CIFWheelFortune(void)
{
}
bool CIFWheelFortune::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,336,332 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);


    RECT m_wheelRect = { 50,60,282,282 };
    m_wheel = (CIFGIFCreator*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFGIFCreator), m_wheelRect, 4, 0);


    RECT m_wheelspinRect = {172,63,41,54};
    m_wheelspin = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_wheelspinRect, 5, 0);
    m_wheelspin->TB_Func_13("juicer\\wheel\\wheel_pin.ddj",0,0);

    RECT m_spinRect = {10,262,80,20};
    m_spin = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_spinRect, 6, 0);
    m_spin->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_spin->SetText(L"Spin");

    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    SetText(L"Wheel Of Fortune");
    return true;
}
void CIFWheelFortune::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}

void CIFWheelFortune::OnUpdate()
{

}
void CIFWheelFortune::OnSpin()
{
    m_wheel->ddjPath = "juicer\\wheel\\wheel_";
    m_wheel->ddjCount = 180;
    m_wheel->ddjMinSpeed = 0.00;
    m_wheel->ddjMaxSpeed = 0.08;
    m_wheel->ddjSpeedDelay = 0.04;
    m_wheel->wheelChoosenIndex = 65;
    m_wheel->ddjCurrentIndex = 0;
    m_wheel->operational = true;
}
