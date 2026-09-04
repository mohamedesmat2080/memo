#include "IFSpinButton.h"
#include <BSLib/Debug.h>
#include <IFMainFrame.h>

GFX_IMPLEMENT_DYNCREATE(CIFSpinButton, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFSpinButton, CIFWnd)
            ONG_COMMAND(1, &CIFSpinButton::OnLeftBtnClick)
            ONG_COMMAND(2, &CIFSpinButton::OnRightBtnClick)
            ONG_WM_4001()
            ONG_WM_4002()
            ONG_WM_4003()
            ONG_VISIBLE_CHANGE()
            ONG_WM_4005()
            ONG_WM_4006()
GFX_END_MESSAGE_MAP()


CIFSpinButton::CIFSpinButton(void)
{
    BS_DEBUG("> " __FUNCTION__);
    this->index = 1;
}

CIFSpinButton::~CIFSpinButton(void)
{
    BS_DEBUG("> " __FUNCTION__);
}
bool CIFSpinButton::OnCreate(long ln)
{
    BS_DEBUG("> " __FUNCTION__ "(%d)", ln);

    // Populate inherited members
    CIFWnd::OnCreate(ln);

    RECT numStaticRect = { 16,4,18,12 };
    numStatic = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), numStaticRect, 0, 0);
    numStatic->TB_Func_13("interface\\store\\str_slot_02.ddj", 0, 0);
    numStatic->SetText(L"1");

    RECT leftButtonRect = { 0,0,16,16 };
    leftButton = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), leftButtonRect, 1, 0);
    leftButton->TB_Func_13("interface\\ifcommon\\com_left_arrow.ddj", 0, 0);

    RECT rightButtonRect = { 34,0,16,16 };
    rightButton = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), rightButtonRect, 2, 0);
    rightButton->TB_Func_13("interface\\ifcommon\\com_right_arrow.ddj", 0, 0);

    return true;
}
void CIFSpinButton::OnLeftBtnClick() {
    if (reinterpret_cast<bool(__thiscall*)(void*, int)>(this->OnChangeFun)(this->Parent, 1)) {
        wchar_t* indexStr = new wchar_t[5];
        wsprintfW(indexStr, L"%d", this->index);
        numStatic->SetText(indexStr);
    }
}
void CIFSpinButton::OnRightBtnClick() {
    if (reinterpret_cast<bool(__thiscall*)(void*, int)>(this->OnChangeFun)(this->Parent, 2)) {
        wchar_t* indexStr = new wchar_t[5];
        wsprintfW(indexStr, L"%d", this->index);
        numStatic->SetText(indexStr);
    }
}
void CIFSpinButton::SetOnChangeFun(void* parent, void* fun) {
    this->OnChangeFun = fun;
    this->Parent = parent;
}
void CIFSpinButton::OnUpdate()
{
}

int CIFSpinButton::On4001(int a1, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
    return 0;
}

int CIFSpinButton::On4002(int a1, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
    return 0;
}

int CIFSpinButton::On4003(int a1, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
    return 0;
}

int CIFSpinButton::OnVisibleStateChange(int newstate, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", newstate, a2);
    return 0;
}

int CIFSpinButton::On4005(int a1, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
    return 0;
}

int CIFSpinButton::On4006(int a1, int a2)
{
    BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
    return 0;
}