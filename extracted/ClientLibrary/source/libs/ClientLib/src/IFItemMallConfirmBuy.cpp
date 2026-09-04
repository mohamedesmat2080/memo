//
// Created by maximus on 12/24/2023.
//

#include <CustomData/CustomSettingManager.h>
#include "IFItemMallConfirmBuy.h"
#include "BSLib/multibyte.h"
#include "GlobalDataManager.h"
#include "IFInventory.h"
#include "IFItemMallShopSlot.h"
#include "InterfaceNetSender.h"

CSOItemPackage *CIFItemMallConfirmBuy::GetPackageItem() const {
    return m_pSOPackageItem;
}

void CIFItemMallConfirmBuy::OnBuy_BtnClick() {
    if(m_Settings->EnableOldItemMall)
    {
        CIFWnd *pParent = (CIFWnd *) GetParentControl();
        if (pParent && pParent->IsSame(GFX_RUNTIME_CLASS(CIFItemMallShopSlot))) {
            CIFItemMallShopSlot *pSlot = (CIFItemMallShopSlot *) pParent;

            std::n_string strCodeName = TO_NSTRING(GetPackageItem()->GetPackageItemData()->m_codeName128).c_str();

            if(pSlot != NULL)
            {
                CStringData *data = new CStringData();

                data->str = strCodeName;

                int nRealIndex = GetRealItemIndex(pSlot->GetItemMallData()->m_usShopId, pSlot->GetItemMallData()->m_btTab,
                                                  GetPackageItem());

                g_pCInterfaceNetSender->BuyItemMallItem(
                        GetPackageItem()->GetPackageItemData()->m_id,
                        pSlot->GetItemMallData()->m_usShopId,
                        pSlot->GetItemMallData()->m_btTab,
                        nRealIndex,
                        m_usCount,
                        0,
                        m_nField7E8,
                        m_nField7EC,
                        *data);

                pSlot->OnConfirmBuySectionControl(false);
            }
        }
        else
        {
            reinterpret_cast<void (__thiscall *)(CIFItemMallConfirmBuy *)>(0x007c4880)(this);
        }
    }
    else
    {
        reinterpret_cast<void (__thiscall *)(CIFItemMallConfirmBuy *)>(0x007c4880)(this);
    }
    //
}

int CIFItemMallConfirmBuy::GetRealItemIndex(int nShopId, int nTabIndex, CSOItemPackage* pPackageItem) {
    CRefShopdata *pShopData = g_CGlobalDataManager->m_refShopDataMap[nShopId];

    if (pShopData && pShopData->m_vRefShopTabGroupData.size() > 0) {

        CRefShopTabGroupData *pTabGroupData = pShopData->m_vRefShopTabGroupData[0];

        if (pTabGroupData && pTabGroupData->m_vRefShopTabData.size() > nTabIndex) {

            CRefShopTabData *pTabData = pTabGroupData->m_vRefShopTabData[nTabIndex];

            for (int i = 0; i < pTabData->m_vPackageItems.size(); i++) {

                if (pTabData->m_vPackageItems[i] && pTabData->m_vPackageItems[i] == pPackageItem) {
                    return i;
                }
            }
        }
    }

    return -1;
}
void CIFItemMallConfirmBuy::OnBuy_BtnClickEnter(int p1, int p2,int p3) {
    if(m_Settings->EnableOldItemMall)
    {
        CIFWnd *pParent = (CIFWnd *) GetParentControl();
        if (pParent && pParent->IsSame(GFX_RUNTIME_CLASS(CIFItemMallShopSlot))) {
            CIFItemMallShopSlot *pSlot = (CIFItemMallShopSlot *) pParent;

            std::n_string strCodeName = TO_NSTRING(GetPackageItem()->GetPackageItemData()->m_codeName128).c_str();

            if(pSlot != NULL)
            {
                CStringData *data = new CStringData();

                data->str = strCodeName;

                int nRealIndex = GetRealItemIndex(pSlot->GetItemMallData()->m_usShopId, pSlot->GetItemMallData()->m_btTab,
                                                  GetPackageItem());

                g_pCInterfaceNetSender->BuyItemMallItem(
                        GetPackageItem()->GetPackageItemData()->m_id,
                        pSlot->GetItemMallData()->m_usShopId,
                        pSlot->GetItemMallData()->m_btTab,
                        nRealIndex,
                        m_usCount,
                        0,
                        m_nField7E8,
                        m_nField7EC,
                        *data);

                pSlot->OnConfirmBuySectionControl(false);
            }
        }
        else
        {
            reinterpret_cast<void(__thiscall*)(CIFItemMallConfirmBuy*, int, int, int)>(0x007c5dc0)(this, p1, p2, p3);
        }
    }
    else
    {
        reinterpret_cast<void(__thiscall*)(CIFItemMallConfirmBuy*, int, int, int)>(0x007c5dc0)(this, p1, p2, p3);
    }

}


