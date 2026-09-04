//
// Created by YUMBUL on 14.07.2024.
//

#include <TextStringManager.h>
#include <ICPlayer.h>
#include <CustomData/CustomCICPlayer.h>
#include <GInterface.h>
#include <GlobalDataManager.h>
#include <IFPlayerMiniInfo.h>
#include "IFMacroMenuAutoScrollSlot.h"
GFX_IMPLEMENT_DYNCREATE(CIFMacroMenuAutoScrollSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFMacroMenuAutoScrollSlot, CIFWnd)


GFX_END_MESSAGE_MAP()
CIFMacroMenuAutoScrollSlot::CIFMacroMenuAutoScrollSlot(void){
    Macro_AutoScroll = false;
    AutoScrollTimerRunning = false;
}
CIFMacroMenuAutoScrollSlot::~CIFMacroMenuAutoScrollSlot(void){

}

bool CIFMacroMenuAutoScrollSlot::OnCreate(long ln) {

    // Populate inherited members
    CIFWnd::OnCreate(ln);
    wnd_rect sz;
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifmacromenuautoscrollslot.txt");
    m_IRM.CreateInterfaceSection("Create", this);
    m_IRM.GetResObj(30, 1)->SetText(L"Cancel");

    for(int i = 0; i < 8; i++)
    {
        Slot1[i] = m_IRM.GetResObj<CIFMacroSCSlot>(205+i, 1);
        Slot1[i]->m_pMySlot->m_pSlot->SetType(0xC);
        Slot1[i]->m_pMySlot->m_pSlot->SetSlot(205 + i);
    }
    this->m_IRM.GetResObj(29, 1)->ShowGWnd(false);
    return true;
}
void CIFMacroMenuAutoScrollSlot::ClearSlots()
{
    for(int i = 0; i < 8; i++)
    {
        this->m_IRM.GetResObj<CIFMacroSCSlot>(205 + i, 1)->m_pMySlot->m_pSlot->ClearSlot();
    }
}
void CIFMacroMenuAutoScrollSlot::OnUpdate(){
    if(Slot1[0] != NULL)
    {
        if(Slot1[0]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[0]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[0]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(6 ,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(6 ,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(6 ,1)->SetText(L"<None>");

        }
    }
    if(Slot1[1] != NULL)
    {
        if(Slot1[1]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[1]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[1]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(8 ,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(8 ,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(8 ,1)->SetText(L"<None>");

        }
    }
    if(Slot1[2] != NULL)
    {
        if(Slot1[2]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[2]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[2]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(10,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(10,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(10,1)->SetText(L"<None>");

        }
    }
    if(Slot1[3] != NULL)
    {
        if(Slot1[3]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[3]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[3]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(12,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(12,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(12,1)->SetText(L"<None>");
        }
    }
    if(Slot1[4] != NULL)
    {
        if(Slot1[4]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[4]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[4]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(14,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(14,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(14,1)->SetText(L"<None>");
        }
    }
    if(Slot1[5] != NULL)
    {
        if(Slot1[5]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[5]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[5]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(16,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(16,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(16,1)->SetText(L"<None>");
        }
    }
    if(Slot1[6] != NULL)
    {
        if(Slot1[6]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[6]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[6]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(18,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(18,1)-SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(18,1)->SetText(L"<None>");
        }
    }
    if(Slot1[7] != NULL)
    {
        if(Slot1[7]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[7]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                std::wstring ItemNames = g_CTextStringManager->GetString2(Slot1[7]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->NameStrID.c_str())->c_str();
                m_IRM.GetResObj(20,1)->SetText(ItemNames.c_str());
            }
            else
            {
                m_IRM.GetResObj(20,1)->SetText(L"<None>");
            }
        }
        else
        {
            m_IRM.GetResObj(20,1)->SetText(L"<None>");
        }
    }
}


void CIFMacroMenuAutoScrollSlot::AutoScrolling()
{
    if (!g_pMyPlayerObj)
        return;

    if(!AutoScrollTimerRunning)
    {
        AutoScrollTimerRunning = true;
        g_pCGInterface->StartTimer(START_AUTO_SCROLL_TIMER, 500);
    }

    for(int i = 0; i < 8; i++)
    {
        if(Slot1[i]->m_pMySlot->m_pSlot->ItemInfo != NULL)
        {
            if(Slot1[i]->m_pMySlot->m_pSlot->ItemInfo->GetItemData() != NULL)
            {
                if(Slot1[i]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->IsResurrectScroll())
                {
                    if(g_pMyPlayerObj->GetCurrentHp() == 0)
                    {
                        g_pCGInterface->GetMainPopup()->GetInventory()->pSlots[Slot1[i]->m_pMySlot->m_pSlot->GetInventorySlotType()]->UseItem();

                        break;
                    }
                }
                else if(Slot1[i]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->IsZerkScroll() && g_pMyPlayerObj->CHARACTER_STATUS != Dead)
                {
                    CIFPlayerMiniInfo *miniinfo = (CIFPlayerMiniInfo*)g_pCGInterface->m_IRM.GetResObj(11, 1);
                    if(miniinfo != NULL)
                    {
                        if(!miniinfo->GetZerkButtonState())
                        {
                            g_pCGInterface->GetMainPopup()->GetInventory()->pSlots[Slot1[i]->m_pMySlot->m_pSlot->GetInventorySlotType()]->UseItem();

                            break;
                        }
                    }
                }
                else
                {
                    if(g_pMyPlayerObj->CHARACTER_STATUS == Dead)
                        return;
                    wchar_t *test = (wchar_t *) Slot1[i]->m_pMySlot->m_pSlot->ItemInfo->GetItemData()->m_desc1_128.c_str();
                    int ID = g_CGlobalDataManager->GetEffectIdByName(test);
                    if(!g_pMyPlayerObj->TargetIsBuffInUse(ID, DWORD32(g_pMyPlayerObj)) && !g_pMyPlayerObj->CheckMagOverlap(ID , DWORD32(g_pMyPlayerObj))
                       && !g_pMyPlayerObj->CheckPhyOverlap(ID , DWORD32(g_pMyPlayerObj)))
                    {
                        g_pCGInterface->GetMainPopup()->GetInventory()->pSlots[Slot1[i]->m_pMySlot->m_pSlot->GetInventorySlotType()]->UseItem();

                        break;
                    }
                }
            }
        }
    }
}