//
// Created by YUMBUL on 13.10.2023.
//

#include "IFMacroSlotWep.h"
#include <GInterface.h>
#include <GlobalDataManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFMacroSlotWep, CIFWnd)

GFX_BEGIN_MESSAGE_MAP(CIFMacroSlotWep, CIFWnd)

GFX_END_MESSAGE_MAP()


CIFMacroSlotWep::CIFMacroSlotWep(void) {

}

CIFMacroSlotWep::~CIFMacroSlotWep(void) {

}

bool CIFMacroSlotWep::OnCreate(long ln) {
    CIFWnd::OnCreate(ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifmacroslot.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    RECT m_pSlotRect = { this->GetPos().x , this->GetPos().y, 32, 32 };
    m_pMySlot = m_IRM.GetResObj<CIFSlotWithHelpEx>(1, 0);
    m_pMySlot->m_pSlot->SetSlotData(NULL);
    m_pMySlot->m_pSlot->SetSlotType(0);
    m_pMySlot->m_pSlot->SetInventorySlotType(0);
    m_pMySlot->m_pSlot->SetUniqueID(10006);

    m_pMySlot->MoveGWnd(this->GetPos().x, this->GetPos().y);
    m_pMySlot->m_pSlot->BringToFront();
    this->ShowGWnd(true);
    return true;
}
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)
void CIFMacroSlotWep::ClearSlot() {
    m_pMySlot->m_pSlot->SetSlotData(NULL);
    m_pMySlot->m_pSlot->SetSlotType(0);
    m_pMySlot->m_pSlot->SetInventorySlotType(0);
    m_pMySlot->m_pSlot->TB_Func_13("", 0, 0);
}

void CIFMacroSlotWep::OnUpdate() {
    if(m_pMySlot != NULL)
    {
        if(m_pMySlot->m_pSlot != NULL)
        {
            m_pMySlot->m_pSlot->ShowGWnd(true);
            m_pMySlot->m_pSlot->BringToFront();
        }
    }
}
void CIFMacroSlotWep::LoadSlot(int SlotSetq, int SlotType, int Data) {
    m_pMySlot->m_pSlot->SetSlot(SlotSetq);
    m_pMySlot->m_pSlot->GetIconSlot(SlotType, Data);
    m_pMySlot->m_pSlot->SetInventorySlotType(Data);
}


void CIFMacroSlotWep::UseItem(int Slot) {
    if (m_pMySlot->m_pSlot->GetSlotType() != 0x47) {
        if (m_pMySlot->m_pSlot->GetSlotType() == 0x46) {
            int TID = g_CGlobalDataManager->GetItem(m_pMySlot->m_pSlot->ItemInfo->m_refObjItemId)->GetData().m_typeId.m_type_id_value;
            if (TID == 25388 || TID == 4908 || TID == 6956 || TID == 9004 || TID == 11052 || TID == 13100 || TID == 21292 || TID == 31532 || TID == 17196 || TID == 15148 || TID == 23340 || TID == 27436 || TID == 19244 || TID == 29484)
                SendPacketMoveItem(m_pMySlot->m_pSlot->GetInventorySlotType(), 6);
            else if (TID == 2604 || TID == 4652)
                SendPacketMoveItem(m_pMySlot->m_pSlot->GetInventorySlotType(), 7);
            else {
                m_pMySlot->m_pSlot->SetSlot(m_pMySlot->m_pSlot->GetInventorySlotType());
                m_pMySlot->m_pSlot->SetType(m_pMySlot->m_pSlot->GetSlotType());
                m_pMySlot->m_pSlot->FUN_00682040();
            }

        } else if (m_pMySlot->m_pSlot->GetSlotType() == 0x4E) {
            m_pMySlot->m_pSlot->SetSlot(m_pMySlot->m_pSlot->GetInventorySlotType());
            m_pMySlot->m_pSlot->SetType(m_pMySlot->m_pSlot->GetSlotType());
            m_pMySlot->m_pSlot->FUN_00682040();

        } else {
            m_pMySlot->m_pSlot->SetType(m_pMySlot->m_pSlot->GetSlotType());
            m_pMySlot->m_pSlot->SetSlot(m_pMySlot->m_pSlot->m_SkillID);
            m_pMySlot->m_pSlot->FUN_00682040();
        }
        m_pMySlot->m_pSlot->SetType(0xC);
        m_pMySlot->m_pSlot->SetSlot(Slot);
    }
}
void CIFMacroSlotWep::SendPacketMoveItem(BYTE SLot, BYTE SlotTo) {
    NEWMSG(0x7034)
        pReq << BYTE(0x00) << BYTE(SLot + 0xD) << BYTE(SlotTo) << UINT16(0x00);
    SENDMSG()
}
void CIFMacroSlotWep::Func_26(int a1) {
    //printf("%p \n", a1);
    CIFSlotWithHelp *pWnd = (CIFSlotWithHelp *) a1;
    if (pWnd != (CIFSlotWithHelp*) 0x0) {
        pWnd->ClearSlot();
        SaveSlotData(pWnd);
    }

}

//bool CIFMySlot::Func_25(int a1) {
//    //printf("%p \n", a1);
//    //CIFSlotWithHelp *pWnd = (CIFSlotWithHelp *) a1;
//    //if (pWnd == (CIFSlotWithHelp *) 0x0) {
//    //    ClearSlot();
//    //}
//    return false;
//}


bool CIFMacroSlotWep::Func_28(CGWnd* a1, int a2, int a3) {
    if (a2 == 0) {
        return true;
    }
    CIFSlotWithHelp *birakilanslot = (CIFSlotWithHelp *) a1;
    int SlotType = birakilanslot->GetParentWindowId();
    switch (SlotType) {
        case 0x46:
        {
            if(this->m_pMySlot->m_pSlot->GetSlot() == 153 || this->m_pMySlot->m_pSlot->GetSlot() == 154)
            {
                if(birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3
                   && birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1
                   && birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 6)
                {
                    m_pMySlot->m_pSlot->CopySlot(birakilanslot);
                     SaveSlotData(m_pMySlot->m_pSlot);
                }

            }
            else if(this->m_pMySlot->m_pSlot->GetSlot() == 155 || this->m_pMySlot->m_pSlot->GetSlot() == 156)
            {
                if(birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID1() == 3
                   && birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID2() == 1
                   && birakilanslot->ItemInfo->GetItemData()->m_typeId.getTypeID3() == 4)
                {
                    m_pMySlot->m_pSlot->CopySlot(birakilanslot);

                     SaveSlotData(m_pMySlot->m_pSlot);
                }

            }
        } break;
        default:
            break;
    }
    return true;
}
void CIFMacroSlotWep::RenderMyself(){
    CIFSlotWithHelp *pWnd = (CIFSlotWithHelp *) GetWndByGID(m_nMagicCubeSlotGID);
    if (pWnd != NULL)
        pWnd->Func_24(this->GetPos().x, this->GetPos().y);
}

void CIFMacroSlotWep::UpdatehgWndSlot(CIFSlotWithHelp *pSlot) {
    if (pSlot == NULL) {
        m_nMagicCubeSlotGID = 0;
        return;
    }
    m_nMagicCubeSlotGID = pSlot->GethgWnd();
    pSlot->ShowGWnd(true);
}

void CIFMacroSlotWep::SaveSlotData(CIFSlotWithHelp* Slot) {
    if (Slot->GetType() == 0xC) {
        CMsgStreamBuffer buf(0x7158);
        //printf("%d %d %d %d\n", Slot->GetSlotType(), Slot, m_pMySlot->m_pSlot->GetInventorySlotType(), m_pMySlot->m_pSlot->GetSkillSlotInDex());
        switch (Slot->GetSlotType()) {
            case 0x25:
            case 0x46:
            case 0x47:
            case 0x4a:
            case 0x4e: {
                buf << BYTE(0x01) << BYTE(Slot->GetSlot()) << BYTE(Slot->GetSlotType()) << UINT32(Slot->GetInventorySlotType());
                SendMsg(buf);
            } break;
            case 0x49:
            {
                buf << BYTE(0x01) << BYTE(Slot->GetSlot()) << BYTE(Slot->GetSlotType()) << UINT32(Slot->GetSkillSlotInDex());
                SendMsg(buf);
            }
                break;
            case 0:
            {
                buf << BYTE(0x01) << BYTE(Slot->GetSlot()) << BYTE(0x0) << UINT32(0x00);
                SendMsg(buf);
            } break;
        }
    }
}
