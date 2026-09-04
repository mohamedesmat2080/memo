#include "IFCOSSetup.h"
#include "BSLib/multibyte.h"
#include "Game.h"


enum
{
    Button = 5500
};

GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFCOSSetup, 0x00eec0e8)
GFX_IMPLEMENT_DYNCREATE_FN(CIFCOSSetup, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFCOSSetup, CIFWnd)
                    ONG_COMMAND(Button, &CIFCOSSetup::On_BtnClick)
GFX_END_MESSAGE_MAP()
CIFCOSSetup::CIFCOSSetup(void)
{
}
CIFCOSSetup::~CIFCOSSetup(void)
{
}

bool CIFCOSSetup::OnCreateIMPL(long ln)
{

    RECT RefReshRect = { 220,263,76,24 };
    bool b = reinterpret_cast<bool (__thiscall *)(CIFCOSSetup *, long)>(0x007a86c0)(this, ln);
    CIFButton* m_mybutton;
    m_mybutton = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), RefReshRect, Button, 0);
    m_mybutton->TB_Func_13("interface\\ifcommon\\com_button.ddj",0,0);
    m_mybutton->SetText(L"Pick Filter");
    m_mybutton->ShowGWnd(true);
    m_mybutton->BringToFront();
    return  b;
}

void CIFCOSSetup::OnUpdateIMPL() {
    reinterpret_cast<void (__thiscall *)(CIFCOSSetup *)>(0x006528a0)(this);
}
void CIFCOSSetup::On_BtnClick() {
}
