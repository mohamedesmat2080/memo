//
// Created by YUMBUL on 15.06.2024.
//

#include "SOItemPackage.h"


CRefPackageItemData *CSOItemPackage::GetPackageItemData() const {
    return m_pPackageItemData;
}
CSOItem *CSOItemPackage::GetSOItem() const {
    if(m_vPSOItem.size() > 0) return m_vPSOItem[0];

    return NULL;
}
