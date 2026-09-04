#include "WndProc_Hook.h"
#include "GlobalDataManager.h"
#include "Hooks.h"
#include <BSLib/multibyte.h>
#include <CharacterDependentData.h>
#include <GInterface.h>
#include <GameDataExt.h>
#include <ICPlayer.h>
#include <IFChatViewer.h>
#include <IFEdit.h>
#include <Juice/PartyInfo/IFPartyInfo.h>
#include <PartyData.h>
#include <TextStringManager.h>
#include <ctime>
#include <sstream>

extern std::vector<WNDPROC> hooks_wndproc;
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
time_t lastClickTime;
bool upClick = false;
LRESULT CALLBACK WndProcHook(HWND hwnd, UINT msg, LPARAM lParam, WPARAM wParam)
{
    //todo replace that code (detects the last of focus of edits)
    if (GetAsyncKeyState(VK_LBUTTON) < 0 && upClick == false)
    {
        upClick = true;
    }
    if (GetAsyncKeyState(VK_LBUTTON) == 0 && upClick == true)
    {
        upClick = false;
        if (g_CurrentIfUnderCursor && !strcmp(g_CurrentIfUnderCursor->GetRuntimeClass()->m_lpszClassName, "CIFEdit")) {
            CIFEdit *CurEdit = (CIFEdit *) g_CurrentIfUnderCursor;
            int id = CurEdit->UniqueID();
            if(id == 2020 || id == 2021 || id == 2022)
            {
                focusedEditID = id-2020;
            }
        }
        if (g_CurrentIfUnderCursor && !strcmp(g_CurrentIfUnderCursor->GetRuntimeClass()->m_lpszClassName, "CIFButton")) {
            CIFButton *CurBtn = (CIFButton *) g_CurrentIfUnderCursor;
            int id = CurBtn->UniqueID();
            if(id == 5000)
            {
                CIFPartyInfo* partyWnd = (CIFPartyInfo*) g_pCGInterface->GetWindowByUniqueID(13401);
                const SPartyData &partyData = g_CCharacterDependentData.GetPartyData();
                if(partyData.NumberOfMembers > 0)
                {
                    for (int i = 0; i < partyData.NumberOfMembers; ++i) {
                        const SPartyMemberData &memberData = g_CCharacterDependentData.GetPartyMemberData(i);
                        wchar_t *NewMsg = new wchar_t[1024];
                        std::wstring mymsg;
                        wsprintfW(NewMsg, L"%s(Lv.%d)", memberData.m_charactername.c_str(), memberData.currentLevel);
                        partyWnd->m_slots[i]->m_playername->SetText(NewMsg);
                        if(memberData.m_guildname.size()> 0)
                            partyWnd->m_slots[i]->m_guildname->SetText(memberData.m_guildname.c_str());
                        else
                            partyWnd->m_slots[i]->m_guildname->SetText(L"<No Guild>");

                        partyWnd->m_slots[i]->m_movebtn->ShowGWnd(true);
                        wsprintfW(NewMsg, L"%d/%d HP", memberData.currentHP, memberData.maxHP);
                        partyWnd->m_slots[i]->m_status->SetText(NewMsg);
                        std::wstringstream LocName;
                        LocName <<memberData.region.r;
                        const std::n_wstring* nwRegionName = g_CTextStringManager->GetString(LocName.str().c_str());
                        mymsg = nwRegionName->c_str();
                        partyWnd->m_slots[i]->m_region->SetText(mymsg.c_str());
                        partyWnd->m_slots[i]->playername = memberData.m_charactername;
                        if(!strcmp(TO_STRING(g_pCICPlayer->GetName()).c_str(),TO_STRING(memberData.m_charactername).c_str()))
                        {
                            partyWnd->m_slots[i]->m_movebtn->SetEnabledState(false);
                        }
                    }
                    partyWnd->ShowGWnd(true);
                    partyWnd->BringToFront();
                }
            }
        }
    }

    if (KEY_DOWN(VK_LCONTROL)) {
        if (KEY_DOWN(VK_LBUTTON) && (time(NULL) - lastClickTime > 1)) {
            lastClickTime = time(NULL);
            if (g_CurrentIfUnderCursor && !strcmp(g_CurrentIfUnderCursor->GetRuntimeClass()->m_lpszClassName, "CIFSlotWithHelp")) {
                CIFSlotWithHelp *CurSlot = (CIFSlotWithHelp *) g_CurrentIfUnderCursor;
                byte SlotID = CurSlot->UniqueID() - 100 + 0xD;
                if (CurSlot->GetType() == 70 && CurSlot->ItemInfo) {
                    CIFWnd *WholeChat = (CIFWnd *) g_pCGInterface->m_IRM.GetResObj(56, 1);
                    CIFEdit *edit;
                    if(g_CGlobalDataManager)
                    {
                        const CItemData::SData *data = &g_CGlobalDataManager->GetItemData(CurSlot->ItemInfo->m_refObjItemId);
                        std::wstring NameStrID128 = g_CTextStringManager->GetStringByNameStrID(
                            data->m_nameStrId128)->c_str();
                        std::n_wstring ItemNameW = (NameStrID128).c_str();
                        UINT16 Size = ItemNameW.length();
                        wchar_t *NewMsg = new wchar_t[1024];
                        if (WholeChat && WholeChat->IsVisible()) {
                            edit = WholeChat->m_IRM.GetResObj<CIFEdit>(36, 1);
                        } else {
                            CIFChatViewer *chatView = (CIFChatViewer *) g_pCGInterface->m_IRM.GetResObj(1,
                                                                                                        1);
                            edit = chatView->m_InputBox;
                        }
                        std::wstring s = edit->GetCurrentText().c_str();

                        wsprintfW(NewMsg, L"%s<%s>", edit->GetText(), NameStrID128.c_str());
                        edit->SetText(NewMsg);
                        edit->SetFocus_MAYBE();
                        CMsgStreamBuffer buf(0x180A);
                        buf << std::n_string(TO_NSTRING(NameStrID128));
                        buf << SlotID<< CurSlot->ItemInfo->m_refObjItemId;
                        SendMsg(buf);
                    }
                }
            }
        }
    }


	for (std::vector<WNDPROC>::iterator it = hooks_wndproc.begin();
		it != hooks_wndproc.end();
		++it)
	{
		if ((*it)(hwnd, msg, lParam, wParam) == RESULT_DISCARD)
		{
			// Call default window proc because nothing happens otherwise ...
			return DefWindowProc(hwnd, msg, lParam, wParam);
		}
	}

	return reinterpret_cast<WNDPROC>(0x008311C0)(hwnd, msg, lParam, wParam);
}
