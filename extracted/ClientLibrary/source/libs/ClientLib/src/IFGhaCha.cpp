//
// Created by YUMBUL on 24.08.2024.
//

#include <CustomData/CustomCICPlayer.h>
#include "IFGhaCha.h"
#include "IFInventory.h"
#include "GInterface.h"

#define WaitTimer 1313
#define StartTimerr 1314

void CIFGhaCha::PlayButton()
{
    if(!m_Player->m_MagicPopTimerRunning)
    {
        m_Player->m_MagicPopTimerRunning = true;
        this->StartTimer(WaitTimer, 500);
    }

    reinterpret_cast<void(__thiscall *)(CIFGhaCha *)>(0x007459b0)(this);
}

void CIFGhaCha::OnTimerIMPL(int timerId)
{
    //printf("%d \n", timerId);
    reinterpret_cast<void(__thiscall *)(CIFGhaCha *, int)>(0x00746c70)(this, timerId);

    if(timerId == WaitTimer)
    {
        if(m_Player->m_MagicPopSettings == 701)
        {
            if(MagicPopSlot != NULL)
            {
                if(MagicPopSlot->ItemInfo != NULL)
                {
                    if(MagicPopSlot->ItemInfo->GetItemData() != NULL)
                    {
                        if(!MagicPopSlot->ItemInfo->GetItemData()->IsMagicPop())
                        {
                            CSOItem *mynewitem;
                            int Slot = -1;
                            CIFInventory *inventory = g_pCGInterface->GetMainPopup()->GetInventory();
                            for (int is = 0; is < inventory->InventorySlotCount(); is++) {
                                CSOItem *newitem = inventory->GetItemBySlot(is);
                                if (newitem->m_blValid != 0) {
                                    if (newitem->GetItemData() != NULL) {
                                        if(newitem->GetItemData()->IsMagicPop())
                                        {
                                            Slot = is;
                                            mynewitem = newitem;
                                            break;
                                        }
                                    }
                                }
                            }

                            if (Slot != -1) {
                                MagicPopSlot->SetSlotData(mynewitem);
                                MagicPopSlot->SetInventorySlotType(Slot);
                                PlayButtonMaybe->SetEnabledState(true);
                                this->KillTimer(WaitTimer);
                                if(m_Player->m_MagicPopTimerRunning)
                                {
                                    m_Player->m_MagicPopTimerRunning = false;
                                }
                            }
                            else
                            {
                                this->KillTimer(WaitTimer);
                                if(m_Player->m_MagicPopTimerRunning)
                                {
                                    m_Player->m_MagicPopTimerRunning = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        this->KillTimer(WaitTimer);
                        if(m_Player->m_MagicPopTimerRunning)
                        {
                            m_Player->m_MagicPopTimerRunning = false;
                        }
                    }
                }
                else
                {
                    this->KillTimer(WaitTimer);
                    if(m_Player->m_MagicPopTimerRunning)
                    {
                        m_Player->m_MagicPopTimerRunning = false;
                    }
                }
            }
            else
            {
                this->KillTimer(WaitTimer);
                if(m_Player->m_MagicPopTimerRunning)
                {
                    m_Player->m_MagicPopTimerRunning = false;
                }
            }
        }
        else if(m_Player->m_MagicPopSettings == 702)
        {
            if(MagicPopSlot != NULL)
            {
                if(MagicPopSlot->ItemInfo != NULL)
                {
                    if(MagicPopSlot->ItemInfo->GetItemData() != NULL)
                    {
                        if(!MagicPopSlot->ItemInfo->GetItemData()->IsMagicPop() && MagicPopSlot->ItemInfo->GetItemData()->RefObjectId != 9239)
                        {
                            CSOItem *mynewitem;
                            int Slot = -1;
                            CIFInventory *inventory = g_pCGInterface->GetMainPopup()->GetInventory();
                            for (int is = 0; is < inventory->InventorySlotCount(); is++) {
                                CSOItem *newitem = inventory->GetItemBySlot(is);
                                if (newitem->m_blValid != 0) {
                                    if (newitem->GetItemData() != NULL) {
                                        if(newitem->GetItemData()->IsMagicPop())
                                        {
                                            Slot = is;
                                            mynewitem = newitem;
                                            break;
                                        }
                                    }
                                }
                            }

                            if (Slot != -1) {
                                MagicPopSlot->SetSlotData(mynewitem);
                                MagicPopSlot->SetInventorySlotType(Slot);
                                PlayButtonMaybe->SetEnabledState(true);

                                if(m_Player->m_MagicPopTimerRunning)
                                {
                                    m_Player->m_MagicPopTimerRunning = false;
                                    this->KillTimer(WaitTimer);
                                    this->StartTimer(StartTimerr, 1000);
                                }

                            }
                        }
                    }
                }
            }

        }
    }
    else if(timerId == StartTimerr)
    {
        KillTimer(StartTimerr);
        PlayButton();
    }
}