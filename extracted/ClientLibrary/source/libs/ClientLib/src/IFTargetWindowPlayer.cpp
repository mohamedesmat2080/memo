#include <ExtraUI/IFTargetPlayerEquip.h>
#include <BSLib/multibyte.h>
#include "IFTargetWindowPlayer.h"
#include "IFStatic.h"
#include "unsorted.h"
#include "TextStringManager.h"
#include "ICPlayer.h"

#include "support/MemberFunctionHook.h"
#include "GInterface.h"
#include "IFRenderStatic.h"

GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFTargetWindowPlayer, 0x00eea5dc)

GFX_IMPLEMENT_DYNCREATE_FN(CIFTargetWindowPlayer, CIFWnd)

enum {
    GDR_TW_KINDRED_MARK = 10, // CIFStatic
    GDR_TWP_TEXT_NAME = 1, // CIFStatic
    GDR_TARGET_ITEM_INFO_BTN = 111
};

GFX_BEGIN_MESSAGE_MAP(CIFTargetWindowPlayer, CIFWnd)
                    ONG_COMMAND(GDR_TARGET_ITEM_INFO_BTN, &GetTargetPlayerInfo)
GFX_END_MESSAGE_MAP()

void CIFTargetWindowPlayer::ShowTargetWnd()
{
    if(!m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->IsVisible())
    {
        m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ShowGWnd(true);
        wnd_pos x = this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->GetPos();
        m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->MoveGWnd(x.x, x.y);

        wnd_size y =   m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->GetSize();
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->MoveGWnd(x.x + y.width, x.y);
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->TB_Func_13("interface\\quick_slot\\qsl_hclose_button.ddj", 1, 1);
    }
}
void CIFTargetWindowPlayer::UpdateRenderStatic()
{
    CICUser* pObject = (CICUser*)GetCharacterObjectByID_MAYBE(m_objectId);
    if(pObject != NULL) {
        CIFTargetPlayerEquip * objWnd = m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1);
        objWnd->UpdateRenderStatic(pObject);

    }

}
void CIFTargetWindowPlayer::GetTargetPlayerInfo()
{
    if(!m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->IsVisible())
    {
        CICUser* pObject = (CICUser*)GetCharacterObjectByID_MAYBE(m_objectId);
        if(pObject != NULL) {
            CIFTargetPlayerEquip * objWnd = m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1);

            if(objWnd->TargetCharName != "")
            {
                if(TO_NWSTRING(objWnd->TargetCharName) != pObject->GetName())
                {
                    CMsgStreamBuffer buf(0x169A);
                    buf << byte(23);
                    buf << pObject->GetName();
                    SendMsg(buf);
                }
                else
                {
                    ShowTargetWnd();
                }
            }
            else
            {
                CMsgStreamBuffer buf(0x169A);
                buf << byte(23);
                buf << pObject->GetName();
                SendMsg(buf);
            }
        }

    }
    else
    {
        m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->MoveGWnd(this->GetPos().x + 236, this->GetPos().y);
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->TB_Func_13("interface\\quick_slot\\qsl_hopen_button.ddj", 1, 1);
    }


}
void CIFTargetWindowPlayer::Hide()
{
    m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->MoveGWnd(this->GetPos().x + 236, this->GetPos().y);
    this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->TB_Func_13("interface\\quick_slot\\qsl_hopen_button.ddj", 1, 1);
}
bool CIFTargetWindowPlayer::OnCreate(long ln) {
    //printf("%s\n", __FUNCTION__);
    //return reinterpret_cast<bool (__thiscall *)(const CIFTargetWindowPlayer *, long)>(0x0069b180)(this, ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\iftw_player.txt");
    m_IRM.CreateInterfaceSection("Create", this);
    m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ShowGWnd(false);
    return true;
}

HOOK_ORIGINAL_MEMBER(0x0069b0d0, &CIFTargetWindowPlayer::FUN_0069b0d0);
void CIFTargetWindowPlayer::FUN_0069b0d0(int objectId) {
    m_objectId = objectId;
    CIGIDObject* pObject = GetCharacterObjectByID_MAYBE(m_objectId);
    if(pObject != NULL) {
        m_IRM.GetResObj<CIFStatic>(GDR_TWP_TEXT_NAME, 1)->SetText(pObject->GetName().c_str());
        m_IRM.GetResObj<CIFStatic>(GDR_TW_KINDRED_MARK, 1)->TB_Func_13(GetKindredTextureFilePath(pObject->GetCommonData()->Country, 0), 0, 0);
        m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->MoveGWnd(this->GetPos().x + 236, this->GetPos().y);
        this->m_IRM.GetResObj<CIFButton>(GDR_TARGET_ITEM_INFO_BTN, 1)->TB_Func_13("interface\\quick_slot\\qsl_hopen_button.ddj", 1, 1);

        m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ShowGWnd(false);
    }
}
void CIFTargetWindowPlayer::UpdateItemSlot(int ItemSlot, CSOItem* tempItemInfo, std::n_string CN16)
{
    m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->UpdateItemSlot(ItemSlot, tempItemInfo, CN16);
}
void CIFTargetWindowPlayer::UpdateCharName(std::n_string CN16)
{
    if(m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->TargetCharName != "")
    {
        if(m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->TargetCharName != CN16)
        {
            m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->TargetCharName = CN16;
            m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->ClearSlots();
        }
    }
    else
    {
        m_IRM.GetResObj<CIFTargetPlayerEquip>(1951, 1)->TargetCharName = CN16;
    }
}