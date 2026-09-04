//
// Created by YUMBUL on 17.06.2024.
//
#include <BSLib/multibyte.h>
#include "IFItemMallZzim.h"
#include "GInterface.h"
#include "InterfaceNetSender.h"
#include "GlobalDataManager.h"


void CIFItemMallZzim::OnBuyAll_BtnClick() {
    if(g_CGlobalDataManager->GetEmptyInventorySlots() >=  g_pCGInterface->m_lstReservedItemMallData.size())
    {
        CIFItemMall* pItemMall = g_pCGInterface->GetItemMall();

        m_pMessageBox = pItemMall->m_pMessageBox;

        if(!pItemMall->m_bIsMessageBoxCreated) {
            m_pMessageBox->SetMessageBoxParent(this);

            BringToFront();

            g_pCGInterface->sub_79A620();

            pItemMall->m_bIsMessageBoxCreated = true;

            int nTotalSilkPrice = 0;

            std::list<SReservedItemMallData>::const_iterator  it = g_pCGInterface->m_lstReservedItemMallData.begin();
            for(; it != g_pCGInterface->m_lstReservedItemMallData.end(); ++it) {
                nTotalSilkPrice += (*it).pItemMallData->CSOItemPackages->GetPackageItemData()->m_nSilkPrice;
            }

            m_pMessageBox->CreateMessageBox(2);
            m_pMessageBox->ShowGWnd(true);
            m_pMessageBox->SetMessageBoxStyle(15);

            CIFStatic* pPriceStatic = m_pMessageBox->GetGuiFromList<CIFStatic>(2);
            pPriceStatic->SetTextFormatted(L"%d",nTotalSilkPrice);
            pPriceStatic->m_FontTexture.SetColor(0xFFFFD953);
            pPriceStatic->MoveGWnd(pPriceStatic->GetPos().x - 30, pPriceStatic->GetPos().y + 4);
        }
    }
}

void CIFItemMallZzim::OnBuyAllCallBack() {

    std::list<SReservedItemMallData>::const_iterator it = g_pCGInterface->m_lstReservedItemMallData.begin();

    if(it != g_pCGInterface->m_lstReservedItemMallData.end()) {
        int nRealIndex = g_pCGInterface->GetRealItemIndex(it->pItemMallData->m_usShopId, it->pItemMallData->m_btTab,
                                                          it->pItemMallData->CSOItemPackages);

        std::n_string strCodeName = TO_NSTRING(it->pItemMallData->CSOItemPackages->GetPackageItemData()->m_codeName128).c_str();

        CStringData *data = new CStringData();

        data->str = strCodeName;

        g_pCInterfaceNetSender->BuyItemMallItem(
                it->pItemMallData->CSOItemPackages->GetPackageItemData()->m_id,
                it->pItemMallData->m_usShopId,
                it->pItemMallData->m_btTab,
                nRealIndex,
                1,
                0,
                0,
                0,
                *data);

        if (!g_pCGInterface->m_lstReservedItemMallData.empty()) {
            g_pCGInterface->m_lstReservedItemMallData.pop_front();
        }

        RefreshReservedItems();
    }

}

void CIFItemMallZzim::RefreshReservedItems() {
    reinterpret_cast<int(__thiscall *)(CIFItemMallZzim *, int)>(0x007D0EA0)(this, 0);
}