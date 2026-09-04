//
// Created by YUMBUL on 19.03.2023.
//

#include "GObjSiegeStruct.h"
#include "Game.h"
#include <SqlConnection/sqlCon.h>
unsigned int CGObjSiegeStruct::SiegeStructDamageMeter(unsigned short a1, int a2) {
    try {
        //std::cout << __FUNCTION__ << " -> ID " << pObj->GetRefObjID() << " CGObjNPC" << std::endl;
        unsigned int aa = reinterpret_cast<unsigned int(__thiscall *)(CGObjSiegeStruct *, unsigned short, int)>(0x004cf860)(this, a1, a2);

        bool isUniqueMonster = false; 

        if (CSqlCon::ServerFortressDpsInfo.find(this->GetRefObjID()) != CSqlCon::ServerFortressDpsInfo.end())
        {
            isUniqueMonster = true;
        }
        if (!isUniqueMonster)
            return aa;

        std::map<DWORD, SAggroMapSecondPairItem>::iterator itCur = MyMap.begin();
        std::map<DWORD, SAggroMapSecondPairItem>::iterator itEnd = MyMap.end();

        if (itCur == itEnd)
            return aa;

        std::map<DWORD, SAggroMapSecondPairItem>::iterator first_record =
                MyMap.begin();

        int firstPlayerGID = first_record->first;

        //Can cast to CGObjPC ?
        CGObjPC *pFirstObj = reinterpret_cast<CGObjPC *>(
                g_pCGame->GetGameObjById(firstPlayerGID));
        int sayi = 0;
        while (pFirstObj == NULL) {
            ++itCur;
            std::pair<DWORD, SAggroMapSecondPairItem> aggro_info2 = *itCur;
            SAggroMapSecondPairItem gid_dmg_pair = aggro_info2.second;
            pFirstObj = reinterpret_cast<CGObjPC *>(
                    g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
            sayi++;
        }


        CMsg *pMsg = pFirstObj->AllocMsg(0x5016);

        *pMsg << this->GetRefObjID();
        *pMsg << MyMap.size() - sayi;
        *pMsg << pFirstObj->RegionID;
        while (itCur != itEnd) {
            std::pair<DWORD, SAggroMapSecondPairItem> aggro_info = *itCur;

            SAggroMapSecondPairItem gid_dmg_pair = aggro_info.second;

            CGObjPC* pPC = reinterpret_cast<CGObjPC*>(g_pCGame->GetGameObjById(gid_dmg_pair.dwPlayerGID));
            if (pPC != NULL) {
                if (pPC->MyGuild != NULL) {
                    std::string Test = pPC->MyGuild->IntanceGuild->MyGuildName;
                    *pMsg << Test;
                }
            } else {
                std::string Testss = "";
                *pMsg << Testss;
            }
            *pMsg << gid_dmg_pair.dwDamage;


            ++itCur;
        }

        pFirstObj->SendMsg(pMsg);
        pMsg->m_wWriteDataArrayPos = 0;

        return aa;
    } catch (...) {
        unsigned int aa = reinterpret_cast<unsigned int(__thiscall *)(CGObjSiegeStruct *, unsigned short, int)>(0x004cf860)(this, a1, a2);

        return aa ;
    }
}