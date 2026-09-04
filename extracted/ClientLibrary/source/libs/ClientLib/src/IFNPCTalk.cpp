#include <CustomData/CustomDataManager.h>
#include <BSLib/Debug.h>
#include <Menu/IFMenu.h>
#include <ExtraUI/IFItemTranslationWnd.h>
#include <CustomData/CustomSettingManager.h>
#include "IFNPCTalk.h"
#include "GInterface.h"
#include "PartyData.h"
#include "CharacterDependentData.h"


void CIF_NPCTalk::OnListChatThingIMPL(int a1, int a2) {
    BS_DEBUG_LOW("%s(%d, %d)", __FUNCTION__, a1, a2);

    int id = GetCurrentEventMsgCtrlId();
    CIFListCtrl *pList;

    if (id == m_textBox->UniqueID()) {
        pList = m_textBox;
    }
    else {
        // Joymax was using this here ... why ... how ... ???
        assert(FALSE);
    }

    CIFListCtrl::SLineOfText *line = pList->sub_63A940();

    std::n_wstring str;
    line->m_font->GetText(&str);
    CIF_NPCWindow *pParent = (CIF_NPCWindow*)GetParentControl();
    if(pParent->GetNpcObjID() == 9252)
    {
        if(g_pCGInterface->m_IRM.GetResObj<CIFItemTranslationWnd>(1361, 1)->IsVisible())
            return;
        if(line->Index == m_CustomDataManager->m_NpcNewUIAction)
      {
         if(!g_pCGInterface->m_IRM.GetResObj<CIFItemTranslationWnd>(1361, 1)->IsVisible())
         {
             g_pCGInterface->m_IRM.GetResObj<CIFItemTranslationWnd>(1361, 1)->ShowGWnd(true);
             g_pCGInterface->m_IRM.GetResObj<CIFItemTranslationWnd>(1361, 1)->UpdateMenuSize();
             g_pCGInterface->GetMainPopup()->ShowGWnd(true);
             g_pCGInterface->GetMainPopup()->ShowSubPage(GDR_INVENTORY);
           /*  if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CIF_NPCWindow)))
             {
                 CIF_NPCWindow *pParent = (CIF_NPCWindow*)GetParentControl();
                 if(pParent->GetNpcObjID() == 9252)
                 {
                     pParent->OnCloseWndIMPL();
                 }
             }*/
         }
     }
    }
    else if(pParent->GetNpcObjID() == 46400)
    {
        if(line->Index == 0)
        {
            const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
            if(partyData.bIsPartyMaster && partyData.bInParty)
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(25);
                buf << byte(0); /// TG
                SendMsg(buf);
                pParent->OnCloseWndIMPL();
            }
            else
            {
                g_pCGInterface->ShowMessage_Warning(L"Only party master can be call shadow uniques.");
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
        }
    }
    else if(pParent->GetNpcObjID() == 46401)
    {
        if(line->Index == 0)
        {
            const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
            if(partyData.bIsPartyMaster && partyData.bInParty)
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(25);
                buf << byte(1); /// CERB
                SendMsg(buf);
                pParent->OnCloseWndIMPL();
            }
            else
            {
                g_pCGInterface->ShowMessage_Warning(L"Only party master can be call shadow uniques.");
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
        }
    }
    else if(pParent->GetNpcObjID() == 46402)
    {
        if(line->Index == 0)
        {
            const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
            if(partyData.bIsPartyMaster && partyData.bInParty)
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(25);
                buf << byte(2); /// IVY
                SendMsg(buf);
                pParent->OnCloseWndIMPL();
            }
            else
            {
                g_pCGInterface->ShowMessage_Warning(L"Only party master can be call shadow uniques.");
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
        }
    }
    else if(pParent->GetNpcObjID() == 46403)
    {
        if(line->Index == 0)
        {
            const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
            if(partyData.bIsPartyMaster && partyData.bInParty)
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(25);
                buf << byte(3); /// uruchı
                SendMsg(buf);
                pParent->OnCloseWndIMPL();
            }
            else
            {
                g_pCGInterface->ShowMessage_Warning(L"Only party master can be call shadow uniques.");
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
        }
    }
    else if(pParent->GetNpcObjID() == 46404)
    {
        if(line->Index == 0)
        {
            const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
            if(partyData.bIsPartyMaster && partyData.bInParty)
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(25);
                buf << byte(4); /// ISYU
                SendMsg(buf);
                pParent->OnCloseWndIMPL();
            }
            else
            {
                g_pCGInterface->ShowMessage_Warning(L"Only party master can be call shadow uniques.");
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
        }
    }
    else
    {
        reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int)>(0x00703f10)(this, a1, a2);
    }
}

void CIF_NPCTalk::FUN_006fcd60(int p1, int p2, int p3)
{
    //printf("%d %d %d \n", p1, p2, p3);
    if(m_Settings->EnableItemTranslation)
    {
        if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CIF_NPCWindow)))
        {
            CIF_NPCWindow *pParent = (CIF_NPCWindow*)GetParentControl();
            if(pParent->GetNpcObjID() == 9252)
            {
                if(p1 == 2 && p2 == 0 && p3 == 0)
                {
                    p1 = 3;

                    std::n_wstring strmsg = L"2. Exchange Items";
                    m_textBox->sub_64F8A0(strmsg, m_CustomDataManager->m_NpcNewUIAction, -1058140, -30208, -30208, 0, 1);
                }
            }
        }
    }

    if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CIF_NPCWindow)))
    {
        CIF_NPCWindow *pParent = (CIF_NPCWindow*)GetParentControl();
        if(pParent->GetNpcObjID() == 46400)
        {
            if (p1 == 1)
            {
                p1 = 2;
                std::n_wstring strmsg2 = L"1. Call Shadow Tigerwoman.";
                m_textBox->sub_64F8A0(strmsg2, 0, -1058140, -30208, -30208, 0, 1);
                pParent->ShowGWndIMPL(true);
                this->ShowGWnd(true);
            }

           // std::n_wstring strmsg = L"2. End conversation.";
            //m_textBox->sub_64F8A0(strmsg, 1, -1058140, -30208, -30208, 0, 1);

          //  return;
        }
        else if(pParent->GetNpcObjID() == 46401)
        {
            if (p1 == 1)
            {
                p1 = 2;
                std::n_wstring strmsg2 = L"1. Call Shadow Cerberus.";
                m_textBox->sub_64F8A0(strmsg2, 0, -1058140, -30208, -30208, 0, 1);
                pParent->ShowGWndIMPL(true);
                this->ShowGWnd(true);
            }

            // std::n_wstring strmsg = L"2. End conversation.";
            //m_textBox->sub_64F8A0(strmsg, 1, -1058140, -30208, -30208, 0, 1);

            //  return;
        }
        else if(pParent->GetNpcObjID() == 46402)
        {
            if (p1 == 1)
            {
                p1 = 2;
                std::n_wstring strmsg2 = L"1. Call Shadow Captain Ivy.";
                m_textBox->sub_64F8A0(strmsg2, 0, -1058140, -30208, -30208, 0, 1);
                pParent->ShowGWndIMPL(true);
                this->ShowGWnd(true);
            }

            // std::n_wstring strmsg = L"2. End conversation.";
            //m_textBox->sub_64F8A0(strmsg, 1, -1058140, -30208, -30208, 0, 1);

            //  return;
        }
        else if(pParent->GetNpcObjID() == 46403)
        {
            if (p1 == 1)
            {
                p1 = 2;
                std::n_wstring strmsg2 = L"1. Call Shadow Uruchi.";
                m_textBox->sub_64F8A0(strmsg2, 0, -1058140, -30208, -30208, 0, 1);
                pParent->ShowGWndIMPL(true);
                this->ShowGWnd(true);
            }

            // std::n_wstring strmsg = L"2. End conversation.";
            //m_textBox->sub_64F8A0(strmsg, 1, -1058140, -30208, -30208, 0, 1);

            //  return;
        }
        else if(pParent->GetNpcObjID() == 46404)
        {
            if (p1 == 1)
            {
                p1 = 2;
                std::n_wstring strmsg2 = L"1. Call Shadow Isyutaru.";
                m_textBox->sub_64F8A0(strmsg2, 0, -1058140, -30208, -30208, 0, 1);
                pParent->ShowGWndIMPL(true);
                this->ShowGWnd(true);
            }

            // std::n_wstring strmsg = L"2. End conversation.";
            //m_textBox->sub_64F8A0(strmsg, 1, -1058140, -30208, -30208, 0, 1);

            //  return;
        }
    }
    reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *, int, int, int)>(0x006fcd60)(this, p1, p2, p3);
}

void CIF_NPCTalk::WriteNpcInfo_Maybe()
{

    reinterpret_cast<void (__thiscall *)(CIF_NPCTalk *)>(0x00700440)(this);
    if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CIF_NPCWindow)))
    {
        CIF_NPCWindow *pParent = (CIF_NPCWindow*)GetParentControl();
        if(pParent->GetNpcObjID() == 9252)
        {
            FUN_006fcd60(2, m_CustomDataManager->m_NpcNewUIAction, 0);

        }
    }
  /*  if(NpcObjID == 9252)
    {
        if(PtrCIF_NPCTalk != NULL)
        {

        }
    }*/

}