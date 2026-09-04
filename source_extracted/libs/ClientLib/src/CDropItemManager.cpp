#include "CDropItemManager.h"
#include "CIItem.h"
#include "EntityManagerClient.h"
#include "ICPlayer.h"
#include <BSLib/multibyte.h>
#include <remodel/MemberFunctionHook.h>

int CDropItemManager::sub_8AF7F0(int a2,char a3,int a4,DWORD a5)
{
    int petUniqueID = *(int *) ((DWORD32) (a4 - 0x6C0) + 0xF8);

    for (std::map<int, CIObject *>::iterator it = g_pGfxEttManager->entities.begin();
         it != g_pGfxEttManager->entities.end(); ++it) {
        //TODO REPLACE STRING COMPARISON WITH A POINTER TO CLASS
        if (!strcmp(it->second->GetRuntimeClass()->m_lpszClassName, "CIItem")) {
            CIItem* item = (CIItem*) it->second;
            int refObjID = *(int *) ((int) (it->second) + 0x21C); //item refobjid
            if(item->m_bPickAbbilty != 0)
                return 0;
            if(item->hasOwner && a2 > 127 && a2 < 139)
            {
                if(item->SomeCheckForPlayerOwnerName != *(int *) ((int) (g_pCICPlayer)) + 0x2094);
            }
            return 0;
        }
    }
    return 0;
}