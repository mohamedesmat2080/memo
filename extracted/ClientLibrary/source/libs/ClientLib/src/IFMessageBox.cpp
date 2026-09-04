#include "IFMessageBox.h"
#include <imgui/imgui.h>
#include <IFSystemMessage.h>
#include <GInterface.h>
#include <ICPlayer.h>

#include "Game.h"
#include "CharacterDependentData.h"
#include <TextStringManager.h>
#include <CustomInterface/IFMovePartyMember.h>
#include <CustomData/CustomCICPlayer.h>
#include <CustomInterface/IFSavedLocation.h>

#define GDR_MBIN_BTN_YES 200
#define GDR_MBIN_BTN_CANCEL 201
#define REVERSE_MOVETOPARTY 8000
#define REVERSE_MOVETOLOCATION 8001


GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFMessageBox, 0x00EE95A0)


GFX_MSGMAP* CIFMessageBox::MessageMap(){
    static const GFX_MSGMAP_ENTRY skillBoardMessageEntries[] =
            {
                    /* {GFX_WM_COMMAND, 0, 14, 14, BSSig_u12, 0,
                             (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnBtnClick))},
 */
                    {GFX_WM_COMMAND, 0, REVERSE_MOVETOPARTY, REVERSE_MOVETOPARTY, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFMessageBox::FUN_BTNPARTY))},
                    {GFX_WM_COMMAND, 0, REVERSE_MOVETOLOCATION, REVERSE_MOVETOLOCATION, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFMessageBox::FUN_BTNLOCATION))},
                    // Diğer özel mesaj girişleri buraya eklenebilir
            };

    static GFX_MSGMAP newmap =
            {
                    reinterpret_cast<const GFX_MSGMAP *>(0x00d9b174), skillBoardMessageEntries,
            };
    return &newmap;
}

void CIFMessageBox::MsgBoxStore() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063e700)(this);
//    m_IRM.GetResObj<CIFStatic>(7, 1)->SetText(stallType ? L"Silk" : L"Gold");
}

void CIFMessageBox::MsgBoxStoreMoney() {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063e940)(this);
    // m_IRM.GetResObj<CIFStatic>(7, 1)->SetText(stallType ? L"Silk" : L"Gold");
}

void CIFMessageBox::SetEditMsgBoxHandler(char Id) {
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *, char)>(0x00641f40)(this, Id);
    //original function doesn't create instances every run. that means it doesn't free memory up not sure though, i didn't go far with it. but for professional use you should.
    /*  if(Id ==0)
      {
          RECT m_cboxRect = {15,145,15,15};
          m_cbox
                  = (CIFCheckBox*)CreateInstance(this, GFX_RUNTIME_CLASS(CIFCheckBox), m_cboxRect, 13371, 0);
          m_cbox->FUN_00656d50(false);

          RECT m_textRect = {35,145,40,15};
          CIFStatic* m_text
                  = (CIFStatic*)CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_textRect, 13372, 0);
          m_text->SetText(L"Silk?");

          this->SetGWndSize(this->GetSize().width,this->GetSize().height + 35);
      }*/
}
int CIFMessageBox::OnClickConfirm(const char *a2)
{
    /* stallType = this->m_cbox->GetCheckedState_MAYBE();

     CMsgStreamBuffer buf(0x186D);
     buf << BYTE(this->m_cbox->GetCheckedState_MAYBE());
     SendMsg(buf);
 */
    return reinterpret_cast<int (__thiscall *)(CIFMessageBox *,const char *)>(0x00641DB0)(this,a2);
}

void CIFMessageBox::Close()
{
    reinterpret_cast<void*(__thiscall *)(CIFMessageBox *)>(0x0063b8b0)(this);
}
void CIFMessageBox::Test()
{
    reinterpret_cast<void*(__thiscall *)(CIFMessageBox *)>(0x0063c320)(this);

}

void CIFMessageBox::OpenItemMall()
{
    reinterpret_cast<void*(__thiscall *)(CIFMessageBox *)>(0x0063b6e0)(this);
}
void CIFMessageBox::ReverseMap()
{
    reinterpret_cast<void*(__thiscall *)(CIFMessageBox *)>(0x0063b9a0)(this);
}

void CIFMessageBox::DeadPoint()
{
    reinterpret_cast<void*(__thiscall *)(CIFMessageBox *)>(0x00641eb0)(this);

}

void CIFMessageBox::OnUpdateIMPL()
{
    switch (m_nMessageBoxStyleType) {
        case 15:
            if(GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CIFItemMall)))
                return;
            break;
    }
    reinterpret_cast<void (__thiscall *)(CIFMessageBox *)>(0x0063d340)(this);
}
void CIFMessageBox::SetMessageBoxStyle(int Id) {
    reinterpret_cast<void(__thiscall *)(CIFMessageBox *, int)>(0x0063EA80)(this, Id);
}

void CIFMessageBox::CreateMessageBox(int Id) {
    reinterpret_cast<void(__thiscall *)(CIFMessageBox *, int)>(0x00643C20)(this, Id);
}

void CIFMessageBox::SetMessageBoxParent(CIFWnd *pWnd) {
    reinterpret_cast<void(__thiscall *)(CIFMessageBox *, CIFWnd *)>(0x0063B5B0)(this, pWnd);
}

bool CIFMessageBox::OnCreateIMPL(long ln) {
    //printf("%s\n", __FUNCTION__);
    bool a = reinterpret_cast<bool (__thiscall *)(const CIFMessageBox *, long)>(0x0063e380)(this, ln);
    this->m_IRM.GetResObj(REVERSE_MOVETOPARTY, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->SetText(L"Move to the user's set location.");
    this->m_IRM.GetResObj(REVERSE_MOVETOPARTY, 1)->SetText(L"Move to party member.");
    this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->ShowGWnd(false);

    return a;
}

void CIFMessageBox::SetMsgBoxHandler(int Id, int a3)
{
    if (Id == 33) {

        reinterpret_cast<void(__thiscall*)(CIFMessageBox*, int, int)>(0x00644c90)(this, Id, a3);
        CIFMainPopup* popup = g_pCGInterface->GetMainPopup();
        CIFInventory* inventory = popup->GetInventory();

        CSOItem* item = inventory->GetItemBySlot(a3);

        if (item->m_blValid == 0) {
            return;
        }
        if(item->m_refObjItemId == 3795)
        {
            if(g_pCGInterface->m_IRM.GetResObj<CIFMovePartyMember>(MovePartyMember, 1)->IsVisible())
            {
                m_Player->ReverseSlot = 9999;
                g_pCGInterface->m_IRM.GetResObj<CIFMovePartyMember>(MovePartyMember, 1)->ShowGWnd(false);
                CGEffSoundBody::get()->PlaySound(L"snd_window_close");
            }
            if(g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->IsVisible())
            {
                m_Player->ReverseSlot = 9999;
                g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->ShowGWnd(false);
                CGEffSoundBody::get()->PlaySound(L"snd_window_close");
            }

            m_Player->ReverseSlot = a3;
            this->SetGWndSize(300, 280);


            this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->MoveGWnd(this->m_IRM.GetResObj(REVERSE_MOVETOPARTY, 1)->GetPos().x, this->m_IRM.GetResObj(REVERSE_MOVETOPARTY, 1)->GetPos().y + 31);

            this->m_IRM.GetResObj(REVERSE_MOVETOPARTY, 1)->ShowGWnd(true);

            this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->ShowGWnd(true);

            this->m_IRM.GetResObj<CIFButton>(GDR_MBIN_BTN_CANCEL, 1)->MoveGWnd(this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->GetPos().x, this->m_IRM.GetResObj(REVERSE_MOVETOLOCATION, 1)->GetPos().y + 31);
            if( !g_CCharacterDependentData.IsInParty())
            {
                this->m_IRM.GetResObj<CIFButton>(REVERSE_MOVETOPARTY, 1)->SetEnabledState(false);
            }
        }
        }

    else
    {
        reinterpret_cast<void(__thiscall*)(CIFMessageBox*, int, int)>(0x00644c90)(this, Id, a3);
    }
}

void CIFMessageBox::FUN_BTNPARTY()
{
    if(!g_pCGInterface->m_IRM.GetResObj<CIFMovePartyMember>(MovePartyMember, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFMovePartyMember>(MovePartyMember, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFMovePartyMember>(MovePartyMember, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }
    this->Close();
}

void CIFMessageBox::FUN_BTNLOCATION()
{
    if(!g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->LoadLocations();
        g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFSavedLocation>(SavedLocation, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }
    this->Close();
}
int CIFMessageBox::OnMouseLeftDownIMPL(int a1, int x, int y)
{
    return true;
}
void CIFMessageBox::ShowGWndIMPL(bool bVisible)
{
    if(!bVisible)
    {
        if(g_pCGInterface->IsVisible())
            CIFWnd::ShowGWnd(bVisible);
        CIFWnd::BringToFront();
        return;
    }
    if(g_pCGInterface->IsVisible())
    {
        CIFWnd::BringToFront();
        CIFWnd::ShowGWnd(bVisible);
    }
}