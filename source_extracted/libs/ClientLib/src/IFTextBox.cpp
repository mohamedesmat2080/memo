#include "IFTextBox.h"
#include "../../../DevKit_DLL/src/Util.h"
#include "GInterface.h"
#include "GameDataExt.h"
#include "ICPlayer.h"
#include "IFChatViewer.h"
#include <remodel/MemberFunctionHook.h>
#include <ctime>


void CIFTextBox::sub_638B50(int a2) {
    reinterpret_cast<void (__thiscall *)(CIFTextBox *, int)>(0x638B50)(this, a2);
}

void CIFTextBox::sub_64E380(int a2) {
    reinterpret_cast<void (__thiscall *)(CIFTextBox *, int)>(0x64E380)(this, a2);
}

void CIFTextBox::sub_638C70(int a2) {
    reinterpret_cast<void (__thiscall *)(CIFTextBox *, int)>(0x638C70)(this, a2);
}

void CIFTextBox::sub_638D50(int a2) {
    reinterpret_cast<void (__thiscall *)(CIFTextBox *, int)>(0x638D50)(this, a2);
}

void CIFTextBox::sub_638D40(int a2) {
    reinterpret_cast<void (__thiscall *)(CIFTextBox *, int)>(0x638D40)(this, a2);
}

void CIFTextBox::sub_64F8A0(std::n_wstring &str, int a3, int a4, int a5, int a6, int a7, int a8) {
    reinterpret_cast<void(__thiscall*)(CIFTextBox*, std::n_wstring*, int, int, int, int, int, int)>(0x64F8A0)(this, &str, a3, a4, a5, a6, a7, a8);
}

void CIFTextBox::sub_64F660(std::n_string &str, int a3, int a4, int a5, int a6, int a7, int a8) {
    reinterpret_cast<void(__thiscall*)(CIFTextBox*, std::n_string*, int, int, int, int, int, int)>(0x64F660)(this, &str, a3, a4, a5, a6, a7, a8);
}
int CIFTextBox::OnHover(int a1,int a2){
    if(g_pCICPlayer)
    {
        //make sure that we dealing with the chat window
        CIFWnd* parentWnd = (CIFWnd *) this->GetParentControl();

        CIFSlotWithHelp* itemLinkSlot = (CIFSlotWithHelp*) g_pCGInterface->GetWindowByUniqueID(13402);

        if(parentWnd->UniqueID() == 1) {
            //initialize the slot
            if (!itemLinkSlot) {
                RECT rect = {0, 0, 0, 0};
                itemLinkSlot = (CIFSlotWithHelp *) g_pCGInterface->CreateInstance(g_pCGInterface, GFX_RUNTIME_CLASS(CIFSlotWithHelp), rect, 13402, 0);
                itemLinkSlot->ShowGWnd(false);
            }
            if(a2 == 0 && g_pCGInterface->m_helperWindow)
            {
                itemLinkSlot->ShowGWnd(false);
                itemLinkSlot->ItemInfo = NULL;
                g_pCGInterface->m_helperWindow->Reset();
                g_pCGInterface->m_helperWindow->ShowGWnd(false);
                g_pCGInterface->m_helperWindow->m_ownerWindow = NULL;
                return reinterpret_cast<int (__thiscall *)(CIFTextBox *, int,int)>(0x63A100)(this, a1,a2);
            }
            if(g_pCGInterface->m_helperWindow && !g_pCGInterface->m_helperWindow->IsVisible())
            {
                //get the full message sent by the player
                std::n_wstring fullMsg = getFullMsg(a1);

                //get the charname
                std::string CharName = getCharName(fullMsg);

                //clear the message from \t , -
                std::string fullmsgA = getFullMsgA(fullMsg);


                //here we getting the location of mouse which by it and some calculations we will identify the message
                POINT mMouse;
                RECT r1;
                GetWindowRect(theApp.GetHWnd(), &r1);
                GetCursorPos(&mMouse);

                int mStartX = mMouse.x - (r1.left + 29);
                int hoverPos = (mStartX + (indexOffset * 350)) / 6;
                int startIndex = fullmsgA.rfind("<", hoverPos);
                int endIndex = fullmsgA.find_first_of(">", hoverPos);
                int endIndexExtern = fullmsgA.find_first_of(">", hoverPos) + 5; //giving extra 5 cause of the width of the underlines
                int errorStartIndex = fullmsgA.rfind(">", hoverPos);
                int errorEndIndex = fullmsgA.find_first_of("<", hoverPos);

                if (startIndex > errorStartIndex && endIndexExtern < errorEndIndex || errorStartIndex == std::string::npos || errorEndIndex == std::string::npos) {
                    if (startIndex != std::string::npos && endIndexExtern != std::string::npos) {

                        std::string itemLinkID = CharName + fullmsgA.substr(startIndex, endIndex - startIndex + 1);

                        itemInfo info = itemInfo();
                        info = ChatItemInfoList[itemLinkID];

                        //checking if the item already on our list we will show the information , otherwise we will request it
                        if(info.listBytes.size()>0)
                        {
                            std::list<BYTE> byteList = info.listBytes;
                            std::list<BYTE>::iterator it;

                            CMsgStreamBuffer buf(0xB034);
                            for (it = byteList.begin(); it != byteList.end(); ++it){
                                buf  <<  BYTE(*it);
                            }

                            //reading the item information directly through a packet by sector1337 (no need for inv slot anymore)
                            CSOItem* tempItemInfo = new CSOItem();
                            tempItemInfo->ReadFromPacket(&buf,1);
                            tempItemInfo->SetEnabled(true);
                            if(tempItemInfo)
                            {
                                g_pCGInterface->m_helperWindow->ShowGWnd(true);
                                itemLinkSlot->ItemInfo = tempItemInfo;
                                POINT m_show_point;
                                m_show_point.x = mMouse.x - (r1.left + 100);
                                m_show_point.y = mMouse.y - r1.top;
                                itemLinkSlot->SetType(70);
                                itemLinkSlot->MoveGWnd(m_show_point.x+140, m_show_point.y);
                                itemLinkSlot->sub_686DB0();
                            }
                        }
                        else
                        {
                            if((time(NULL) - lastlinkTime) > 0.5)
                            {
                                lastlinkTime = time(NULL);
                                NEWMSG(0x180B)
                                    pReq << std::n_string(itemLinkID.c_str());
                                SENDMSG()
                            }

                        }
                    }
                }
            }
        }
    }
    return reinterpret_cast<int (__thiscall *)(CIFTextBox *, int,int)>(0x63A100)(this, a1,a2);
}

void CIFTextBox::RemoveLine(int line) {
    reinterpret_cast<void(__thiscall*)(CIFTextBox*, int)>(0x0063AFF0)(this, line);
}