#include "IFCustomMessageBox.h"
#include <BSLib/Debug.h>
#include <Game.h>
#include <NetProcessIn.h>
#include <TextStringManager.h>
#include <GInterface.h>
#include <CustomData/CustomCICPlayer.h>
#define GDR_OK_BTN 5
#define GDR_CANCEL_BTN 6

GFX_IMPLEMENT_DYNCREATE(CIFCustomMessageBox, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFCustomMessageBox, CIFMainFrame)
                    ONG_COMMAND(GDR_OK_BTN, &CIFCustomMessageBox::On_BtnClick_1)
                    ONG_COMMAND(GDR_CANCEL_BTN, &CIFCustomMessageBox::On_BtnClick_2)
GFX_END_MESSAGE_MAP()

CIFCustomMessageBox::CIFCustomMessageBox(void) {
    ok = 0;
    cancel = 0;
    write = 0;
    selectedCheckBoxID = 0;
    BS_DEBUG_LOW(">" __FUNCTION__);
}
CIFCustomMessageBox::~CIFCustomMessageBox(void) {
    BS_DEBUG_LOW(">" __FUNCTION__);
    ok = NULL;
}

bool CIFCustomMessageBox::OnCreate(long ln) {
    //	printf("> " __FUNCTION__ "(%d)\n", ln);

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);
    // TB_Func_13("interface\\frame\\mframe_wnd_", 0, 0);
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifcustommessagebox.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->m_pTitleText->SetText(TSM_GETTEXTPTR(L"UIIT_STT_CONFIRM_BOX"));
    this->TB_Func_13("interface\\messagebox\\msgbox2_window_", 0, 0);

    m_pCloseBtn->ShowGWnd(false);

    m_IRM.GetResObj(6, 1)->SetText(L"Cancel");
    m_IRM.GetResObj(5, 1)->SetText(L"Confirm");
    this->ShowGWnd(false);
    UpdateMenuSize();


    return true;
}

void CIFCustomMessageBox::OnUpdate() {
}

void CIFCustomMessageBox::UpdateMenuSize() {
    if(selectedCheckBoxID == 1)
    {
        m_IRM.GetResObj<CIFTextBox>(4, 1)->SetText(L"When you agree to use the function, your stat points will be given as str. This function continues to run in the background. Do you approve?");
    }
    else if(selectedCheckBoxID == 2)
    {
        m_IRM.GetResObj<CIFTextBox>(4, 1)->SetText(L"When you agree to use the function, your stat points will be given as int. This function continues to run in the background. Do you approve?");
    }
    else if(selectedCheckBoxID == 3)
    {
        m_IRM.GetResObj<CIFTextBox>(4, 1)->SetText(L"When you agree to use the function, the area of expertise that is open in the skill window will be opened automatically. This function does not run in the background. Do you approve?");
    }
    else if(selectedCheckBoxID == 4)
    {
        m_IRM.GetResObj<CIFTextBox>(4, 1)->SetText(L"hen you agree to use the function, the skills in the field of expertise that are open in the skill window will be openedautomatically. This function does not run in the background. Do you approve?");
    }
    USHORT PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height / 2) - (this->GetSize().height / 2);
    PosX = (g_CGame->GetRes().res->width / 2) - (this->GetSize().width / 2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}

void CIFCustomMessageBox::On_BtnClick_1() {
    if(selectedCheckBoxID == 1)
    {
        CIFMainPopup * p = g_pCGInterface->GetMainPopup();
        p->GetPlayerInfo()->EnableAutoStr();
    }
    else  if(selectedCheckBoxID == 2)
    {
        CIFMainPopup * p = g_pCGInterface->GetMainPopup();
        p->GetPlayerInfo()->EnableAutoInt();
    }
    else if (selectedCheckBoxID == 3 && selectedMasteryID > 0)
    {
        CIFMainPopup * p = g_pCGInterface->GetMainPopup();
        p->GetSkill()->m_skillBoard->EnableAutoMastery(selectedMasteryID);
    }
    else if (selectedCheckBoxID == 4 && selectedMasteryID > 0)
    {
        CIFMainPopup * p = g_pCGInterface->GetMainPopup();
        p->GetSkill()->m_skillBoard->EnableAutoSkill(selectedMasteryID);
    }
    this->ShowGWnd(false);

}

void CIFCustomMessageBox::On_BtnClick_2() {
    this->ShowGWnd(false);
}
