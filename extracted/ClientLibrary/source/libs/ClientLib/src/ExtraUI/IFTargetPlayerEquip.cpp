//
// Created by YUMBUL on 6.01.2025.
//

#include "IFTargetPlayerEquip.h"

#include <Game.h>
#include <BSLib/Debug.h>
#include <GInterface.h>
#include <BSLib/multibyte.h>
#include <CustomData/CustomSettingManager.h>
#include <IFRenderStatic.h>
#include <ICUser.h>



GFX_IMPLEMENT_DYNCREATE(CIFTargetPlayerEquip, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFTargetPlayerEquip, CIFMainFrame)
                   // ONG_COMMAND(11, &On_BtnClick)
//                    ONG_COMMAND(1000, &On_BtnClick)

GFX_END_MESSAGE_MAP()

#define WeaponSlot 106
#define ShieldSlot 107
CIFTargetPlayerEquip::CIFTargetPlayerEquip(void){
    TargetCharName = std::n_string();
}
CIFTargetPlayerEquip::~CIFTargetPlayerEquip(void){

}
bool CIFTargetPlayerEquip::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\iftargetplayerequipment.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    //m_IRM.GetResObj<CIFSlotWithHelp>(WeaponSlot, 1)->TB_Func_13("interface\\equipment\\equip_slot_weapon.ddj", 1, 1);

    //m_IRM.GetResObj<CIFSlotWithHelp>(ShieldSlot, 1)->TB_Func_13("interface\\equipment\\equip_slot_shield.ddj", 1, 1);

    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetClickable(false);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetClickable(false);


    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetType(19);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetType(19);



    UpdateMenuSize();
    this->ShowGWnd(false);
    return true;
}
void CIFTargetPlayerEquip::ClearSlots()
{
    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetSlotData(NULL);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetSlotData(NULL);

    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetSlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetSlotType(0);

    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetInventorySlotType(0);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetInventorySlotType(0);

    m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->TB_Func_13("", 0, 0);
    m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->TB_Func_13("", 0, 0);

}
void CIFTargetPlayerEquip::UpdateItemSlot(int ItemSlot, CSOItem* tempItemInfo, std::n_string CN16)
{
    if(ItemSlot == 0)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(100, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 1)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(101, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 2)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(102, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 3)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(103, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 4)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(104, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 5)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(105, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 6)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(106, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 7)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(107, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 9)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(109, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 10)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(110, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 11)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(111, 1)->SetSlotData(tempItemInfo);
    }
    else if(ItemSlot == 12)
    {
        m_IRM.GetResObj<CIFSlotWithHelp>(112, 1)->SetSlotData(tempItemInfo);
    }

}
void CIFTargetPlayerEquip::UpdateMenuSize()
{

}
void CIFTargetPlayerEquip::UpdateRenderStatic(CICUser* pObjectx)
{
    pObject = pObjectx;
    m_IRM.GetResObj<CIFRenderStatic>(500, 1);

    m_IRM.GetResObj<CIFRenderStatic>(500, 1)->Clear();
    this->StartTimer(1, 250);

}

void CIFTargetPlayerEquip::OnTimer(int timerId)
{
    if(timerId == 1)
    {
        this->KillTimer(1);
        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->FUN_005602c0(m_IRM.GetResObj<CIFRenderStatic>(500, 1)->GetCharacterObj(pObject->GetCommonData()->RefObjectId, pObject->m_pCCObjAnimation), 0);
        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->Test1();

        undefined4 uStack12;
        uStack12 = 0;
        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->Test(&uStack12);
        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->Test2(&uStack12, 0x420d1000);

        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->field_0x430 = 0x43fa0000;
        m_IRM.GetResObj<CIFRenderStatic>(500, 1)->Test3(0, 0);

          m_IRM.GetResObj<CIFRenderStatic>(500, 1)->yukariasagi = 15.000;
         m_IRM.GetResObj<CIFRenderStatic>(500, 1)->N00000609 = 1;

    }
}