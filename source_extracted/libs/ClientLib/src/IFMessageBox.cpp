//
// Created by Kurama on 1/5/2022.
//

#include "IFMessageBox.h"
#include "Game.h"
#include "GInterface.h"
#include "ICPlayer.h"
#include "CharacterDependentData.h"

#include <BSLib/Debug.h>
#include <Juice/PartyInfo/IFPartyInfo.h>
#include <remodel/MemberFunctionHook.h>

#define GDR_STA_TITLE 1000
#define GDR_STA_DRAG 1001

GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFMessageBox, 0x00ee95a0)

GFX_IMPLEMENT_DYNCREATE_FN(CIFMessageBox, CIFFrame)

GFX_BEGIN_MESSAGE_MAP(CIFMessageBox, CIFWnd)
GFX_END_MESSAGE_MAP()

void CIFMessageBox::OnTimer(int timerId) {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *, int)>(0x0063bf40)(this, timerId);
}

bool CIFMessageBox::OnCreate(long ln) {
    CIFFrame::OnCreate(ln);

    m_IRM.LoadFromFile("resinfo\\ifmessagebox.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->TB_Func_13("interface\\messagebox\\msgbox2_window_.ddj", 0, 0);

    wnd_rect iStack44 = this->GetBounds();

    wnd_rect sz;
    sz.pos.x = 10;
    sz.pos.y = 12;
    sz.size.height = 12;
    sz.size.width = iStack44.size.width - 21;

    m_title = (CIFStatic *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), sz, GDR_STA_TITLE, 0);

    sz.pos.y = 0;
    sz.size.height = 34;

    m_Draghandler = (CIFDragableArea *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFDragableArea), sz, GDR_STA_DRAG, 0);

    RECT rect = {0};

    m_title->SetSomeRect(rect);
    m_title->SetFont(theApp.GetFont(0));
    m_title->TB_Func_5(1);
    m_title->TB_Func_6(0);


    m_Draghandler->SetSomeRect(rect);
    m_Draghandler->SetFont(theApp.GetFont(0));
    m_Draghandler->TB_Func_5(1);
    m_Draghandler->TB_Func_6(0);

    UpdateLayout();

    FUNC_63B420();
    return true;
}

void CIFMessageBox::OnUpdate() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063d340)(this);
}

void CIFMessageBox::SetGWndSize(int width, int height) {
    CIFTileWnd::SetGWndSize(width, height);
    UpdateLayout();
}

void CIFMessageBox::ShowGWnd(bool bVisible) {
    if (!bVisible) {
        if (g_pCGInterface->IsVisible())
            CGEffSoundBody::get()->PlaySound(L"snd_window_close");
        CIFWnd::ShowGWnd(bVisible);
        return;
    }
    N0000028D = 1;
    N0000028E = 0;
    if (g_pCGInterface->IsVisible())
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    CIFWnd::BringToFront();
    CIFWnd::ShowGWnd(bVisible);
}

bool CIFMessageBox::SetText(const wchar_t *src) {
    return m_title->SetText(src);
}

void CIFMessageBox::UpdateLayout() {
    wnd_rect rect = GetBounds();

    m_title->MoveGWnd(rect.pos.x + 10, rect.pos.y + 12);
    m_title->SetGWndSize(rect.size.width - 21, 12);

    m_Draghandler->MoveGWnd(rect.pos.x + 10, rect.pos.y + 12);
    m_Draghandler->SetGWndSize(rect.size.width - 21, 34);

    this->SetGWndSize(m_Draghandler->GetSize().width - 32, m_Draghandler->GetPos().y - 56);
}

void CIFMessageBox::FUN_0063b4d0() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063b4d0)(this);
}

void CIFMessageBox::MakeMsgBox(BoxSections sectionId) {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *, BoxSections)>(0x0070cf50)(this, sectionId);
}

void CIFMessageBox::SetEditMsgBoxHandler(char Id) {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *, char)>(0x00641f40)(this, Id);
}

void CIFMessageBox::SetMsgBoxHandler(int Id, int invSlot) {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *, int, int)>(0x00644c90)(this, Id, invSlot);
    if(Id == 33)
    {
        this->SetGWndSize(300,239);

        RECT m_btnPartyRect = {40, 280, 250, 24};
        CIFButton* m_btnParty = (CIFButton*)CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btnPartyRect, 5000, 0);
        m_btnParty->SetText(L"Move to party members");
        m_btnParty->TB_Func_13("interface\\bimbum\\stretch_button.ddj", 1, 1);
        m_btnParty->m_FontTexture.SetColor(0xfbf7d2);
        m_btnParty->ShowGWnd(true);
        m_btnParty->BringToFront();

        m_btnParty->MoveGWnd(N00000258[1]->GetPos().x,N00000258[1]->GetPos().y);
        N00000258[1]->MoveGWnd(N00000258[1]->GetPos().x,m_btnParty->GetPos().y+31);

        CIFPartyInfo* partyWnd = (CIFPartyInfo*) g_pCGInterface->GetWindowByUniqueID(13401);
        partyWnd->invSlot = invSlot;
    }
}

void CIFMessageBox::MsgBoxINIF() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063e570)(this);
}

void CIFMessageBox::MsgBoxStore() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063e700)(this);
}

void CIFMessageBox::MsgBoxStoreMoney() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063e940)(this);
}

void CIFMessageBox::MsgBoxInsertMsg() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063ff90)(this);
}

void CIFMessageBox::MsgBoxDivideCount() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640020)(this);
}

void CIFMessageBox::MsgBoxStoreConfirm() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640300)(this);
}

void CIFMessageBox::MsgBoxInsertCouponNumber() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640210)(this);
}

void CIFMessageBox::MentorMatchRegister() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640660)(this);
}

void CIFMessageBox::MsgBoxSimple() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640120)(this);
}

void CIFMessageBox::MsgBoxTaxModify() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640540)(this);
}

void CIFMessageBox::MsgBoxTaxLevy() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x00640420)(this);
}

void CIFMessageBox::MsgBoxMakeItem() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x006405d0)(this);
}

void CIFMessageBox::MsgBoxMakeItemCancel() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x006433b0)(this);
}
int CIFMessageBox::FUNC_63B420()
{
    return  reinterpret_cast<int (__thiscall *)(CIFMessageBox **)>(0x0063B420)((CIFMessageBox**)this);
}