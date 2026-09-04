//
// Created by Admin on 21/11/2021.
//

#include "IFCOS.h"
#include "ICPlayer.h"
#include "GInterface.h"
#include "ICCos.h"
#include "IFCOSStatus.h"
#include "IFCOSManager.h"
#include "TextStringManager.h"
#include <Game.h>
#include <IFNormalTile.h>
#include <SRIFLib/NInterfaceResource.h>
#include <CustomData/CustomDataManager.h>

#define GDR_TEST_BTN 13312321
#define GDR_TEST_BTN2 133123212
GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFCOS, 0x00EEC048)

GFX_MSGMAP* CIFCOS::MessageMap(){
    static const GFX_MSGMAP_ENTRY skillBoardMessageEntries[] =
            {
                    /* {GFX_WM_COMMAND, 0, 14, 14, BSSig_u12, 0,
                             (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnBtnClick))},
 */
                    {GFX_WM_COMMAND, 0, 1000, 1000, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFCOS::ActiveTab2))},
                    {GFX_WM_COMMAND, 0, 1001, 1001, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFCOS::ActiveTab3))},
                    // Diğer özel mesaj girişleri buraya eklenebilir
            };

    static GFX_MSGMAP newmap =
            {
                    reinterpret_cast<const GFX_MSGMAP *>(0x00dba074), skillBoardMessageEntries,
            };
    return &newmap;
}
void CIFCOS::PressTabs()
{

    if(g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1) != NULL)
    {
        for(std::list<CIFCOSStatus*>::iterator it = g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1)->N00000A57.begin();
            it != g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1)->N00000A57.end(); ++it)
        {
            if((*it)->status == 1)
            {
                if((*it)->m_petUniqueID != 0)
                {
                    CICharactor* pUser = GetCharacterObjectByID_MAYBE((*it)->m_petUniqueID);
                    if (pUser != NULL) {
                        if(m_CustomDataManager->m_RefFellowPetSystem.find(pUser->GetCommonData()->NameStrID) != m_CustomDataManager->m_RefFellowPetSystem.end())
                        {
                            int id = GetCurrentEventMsgCtrlId();
                            if(id == 100)
                            {
                                this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
                                this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);
                                this->m_SelectableArea->sub_64CC30(1);
                                if(this->m_CosInfo != NULL)
                                {
                                    this->m_CosInfo->ShowTab1();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    reinterpret_cast<void(__thiscall*)(CIFCOS*)>(0x0079ff10)(this);


}
void CIFCOS::ActiveTab2()
{
    int id = GetCurrentEventMsgCtrlId();
    if(id == 1000)
    {
        this->m_SelectableArea->sub_64CC30(0);
        this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(1);
        this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);
        if(this->m_CosInfo != NULL)
        {
            this->m_CosInfo->HideTab1();
        }
    }
}
void CIFCOS::ActiveTab3() {
    int id = GetCurrentEventMsgCtrlId();
    if(id == 1001)
    {
        this->m_SelectableArea->sub_64CC30(0);
        this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
        this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(1);
        if(this->m_CosInfo != NULL)
        {
            this->m_CosInfo->HideTab1();
            this->m_CosInfo->HideTab2();
        }
    }
}
bool CIFCOS::OnCreateIMPL(long ln){
    bool b = reinterpret_cast<bool(__thiscall *)(CIFCOS *, long)>(0x0079f9a0)(this, ln);
    this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CE30("clientlibrary\\fellowpets\\pet_item_tab_on.ddj",
                                                                  "clientlibrary\\fellowpets\\pet_item_tab_off.ddj", "clientlibrary\\fellowpets\\pet_item_tab_disable.ddj");
    this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
    this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CE30("clientlibrary\\fellowpets\\pet_skill_tab_on.ddj",
                                                                  "clientlibrary\\fellowpets\\pet_skill_tab_off.ddj", "clientlibrary\\fellowpets\\pet_skill_tab_disable.ddj");

    this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);
    this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->ShowGWnd(false);

    if(g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1) != NULL)
    {
        for(std::list<CIFCOSStatus*>::iterator it = g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1)->N00000A57.begin();
            it != g_pCGInterface->m_IRM.GetResObj<CIFCOSManager>(39, 1)->N00000A57.end(); ++it)
        {
            if((*it)->status == 1)
            {
                if((*it)->m_petUniqueID != 0)
                {
                    CICharactor* pUser = GetCharacterObjectByID_MAYBE((*it)->m_petUniqueID);
                    if (pUser != NULL) {
                        if(m_CustomDataManager->m_RefFellowPetSystem.find(pUser->GetCommonData()->NameStrID) != m_CustomDataManager->m_RefFellowPetSystem.end())
                        {
                            this->m_SelectableArea->sub_64CE30("clientlibrary\\fellowpets\\pet_info_tab_on.ddj",
                                                               "clientlibrary\\fellowpets\\pet_info_tab_off.ddj",
                                                               "clientlibrary\\fellowpets\\pet_info_tab_disable.ddj");
                            this->m_SelectableArea->sub_64CC30(1);
                            this->m_SelectableArea->SetText(L"");
                            this->m_SelectableArea1->ShowGWnd(false);
                            this->m_SelectableArea2->ShowGWnd(false);
                            this->SetGWndSize(355, 482);
                            this->m_IRM.GetResObj<CIFFrame>(0, 1)->SetGWndSize(331, 403);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->ShowGWnd(true);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->ShowGWnd(true);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);

                            // this->m_IRM.GetResObj<CIFCOSInfo>(121, 1)->SetPet2Gui();
                            break;
                        }
                        else
                        {
                            this->m_SelectableArea->sub_64CE30("interface\\ifcommon\\com_long_tab_on.ddj",
                                                               "interface\\ifcommon\\com_long_tab_off.ddj",
                                                               "interface\\ifcommon\\com_long_tab_disable.ddj");

                            this->m_SelectableArea->SetText(TSM_GETTEXTPTR(L"UIIT_STT_COSNEWUI_TABMENU_BASICINFO"));
                            this->m_SelectableArea1->ShowGWnd(true);
                            this->m_SelectableArea2->ShowGWnd(true);
                            this->SetGWndSize(355, 390);
                            this->m_IRM.GetResObj<CIFFrame>(0, 1)->SetGWndSize(331, 314);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->ShowGWnd(false);
                            this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->ShowGWnd(false);

                            //this->m_IRM.GetResObj<CIFCOSInfo>(121, 1)->SetNormalPetGui();
                            break;
                        }
                    }
                }
            }
        }
    }

    return b;
}

void CIFCOS::Switch(int p1, int i)
{
    //printf("%p \n", this);
    reinterpret_cast<void(__thiscall*)(CIFCOS*, int, int)>(0x0079ff50)(this, p1, i);
    if(i == 3)
    {
        CICharactor* pUser = GetCharacterObjectByID_MAYBE(p1);
        if (pUser != NULL) {
            if(m_CustomDataManager->m_RefFellowPetSystem.find(pUser->GetCommonData()->NameStrID) != m_CustomDataManager->m_RefFellowPetSystem.end())
            {
                this->m_SelectableArea->sub_64CE30("clientlibrary\\fellowpets\\pet_info_tab_on.ddj",
                                                   "clientlibrary\\fellowpets\\pet_info_tab_off.ddj", "clientlibrary\\fellowpets\\pet_info_tab_disable.ddj");
                this->m_SelectableArea->sub_64CC30(1);
                this->m_SelectableArea->SetText(L"");
                this->m_SelectableArea1->ShowGWnd(false);
                this->m_SelectableArea2->ShowGWnd(false);
                this->SetGWndSize(355, 482);
                this->m_IRM.GetResObj<CIFFrame>(0, 1)->SetGWndSize(331, 403);
                this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
                this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->ShowGWnd(true);
                this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);

                //  this->m_IRM.GetResObj<CIFCOSInfo>(121, 1)->SetPet2Gui();
            }
            else
            {
                this->m_SelectableArea->sub_64CE30("interface\\ifcommon\\com_long_tab_on.ddj",
                                                   "interface\\ifcommon\\com_long_tab_off.ddj", "interface\\ifcommon\\com_long_tab_disable.ddj");
                this->m_SelectableArea->sub_64CC30(1);
                this->m_SelectableArea->SetText(TSM_GETTEXTPTR(L"UIIT_STT_COSNEWUI_TABMENU_BASICINFO"));
                this->m_SelectableArea1->ShowGWnd(true);
                this->m_SelectableArea2->ShowGWnd(true);
                this->SetGWndSize(355, 390);
                this->m_IRM.GetResObj<CIFFrame>(0, 1)->SetGWndSize(331, 314);
                this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj<CIFSelectableArea>(1000, 1)->sub_64CC30(0);
                this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->ShowGWnd(false);
                this->m_IRM.GetResObj<CIFSelectableArea>(1001, 1)->sub_64CC30(0);

                //this->m_IRM.GetResObj<CIFCOSInfo>(121, 1)->SetNormalPetGui();
            }
        }

    }

}

void CIFCOS::FUN_0079fcd0(int p1)
{
    //printf("%d \n", p1);
    reinterpret_cast<void(__thiscall*)(CIFCOS*, int)>(0x007a0140)(this, p1);
}
void CIFCOS::FUN_0079fc20(int p1)
{
    //printf("%d \n", p1);
    reinterpret_cast<void(__thiscall*)(CIFCOS*, int)>(0x0079fc20)(this, p1);
}
void CIFCOS::OnUpdateIMPL() {
    reinterpret_cast<void(__thiscall*)(const CIFCOS*)>(0x006528a0)(this);

    CICCos* pUser = static_cast<CICCos*>(GetCharacterObjectByID_MAYBE(this->m_petUniqueID));
    if (pUser == NULL) {
        return;
    }

    if (pUser != NULL)
    {

    }
}

