#include "Util.h"

#include <sys/stat.h>

#include <memory/hook.h>

#include "hooks/Hooks.h"
#include "hooks/GFXVideo3d_Hook.h"
#include "hooks/CGame_Hook.h"

#include "ICMonster.h"
#include "ICPlayer.h"
#include "QuickStart.h"
#include "hooks/WndProc_Hook.h"
#include <BSLib/Debug.h>
#include <BSLib/multibyte.h>
#include <CDropItemManager.h>
#include <CPSMission.h>
#include <GFX3DFunction/RStateMgr.h>
#include <GInterface.h>
#include <GameDataExt.h>
#include <ICUser.h>
#include <IFChatViewer.h>
#include <IFMessageBox.h>
#include <NetProcessIn.h>
#include <NetProcessSecond.h>
#include <NetProcessThird.h>
#include <PSCharacterSelect.h>
#include <GlobalDataManager.h>

std::vector<const CGfxRuntimeClass *> register_objects;
std::vector<overrideFnPtr> override_objects;

QuickStart quickstart;

void Setup() {

#ifdef CONFIG_DEBUG_CONSOLE
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONIN$", "r", stdin);
#endif

    vftableHook(0x00E0963C, 17, addr_from_this(&CGFXVideo3D_Hook::CreateThingsHook));
    vftableHook(0x00E0963C, 26, addr_from_this(&CGFXVideo3D_Hook::EndSceneHook));
    vftableHook(0x00E0963C, 20, addr_from_this(&CGFXVideo3D_Hook::SetSizeHook));

    vftableHook(0x00db95a4, 10, addr_from_this(&CGInterface::OnCreateIMPL));
    vftableHook(0x00dd811c, 10, addr_from_this(&CPSCharacterSelect::OnCreateIMPL));

    vftableHook(0x00de2e7c, 15, addr_from_this(&CICUser::Func_15_impl));
    vftableHook(0x00de256c, 15, addr_from_this(&CICharactor::Func_15_impl));
    vftableHook(0x00de2c24, 15, addr_from_this(&CICPlayer::Func_15_impl));
    vftableHook(0x00de26c4, 15, addr_from_this(&CICMonster::Func_15_impl));

    replaceAddr(0x00831337 + 4, (int) &WndProcHook);

    placeHook(0x0065c6f0, addr_from_this(&CAlramGuideMgrWnd::GetGuide));

    replaceOffset(0x008491d1, addr_from_this(&CGame_Hook::LoadGameOption));

    replaceOffset(0x00832a11, addr_from_this(&CGame_Hook::InitGameAssets_Impl));

    replaceOffset(0x0084c9bf, addr_from_this(&CNetProcessIn::RegisterPacketHandlers));
    replaceOffset(0x00898656, addr_from_this(&CNetProcessSecond::RegisterPacketHandlers));
    replaceOffset(0x008a4876, addr_from_this(&CNetProcessThird::RegisterPacketHandlers));

    replaceOffset(0x009ded0d, addr_from_this(&CRStateMgr::FUN_00470060));

    placeHook(0x0049d620, Put);
    replaceAddr(0x00dd92d4, addr_from_this(&CPSTitle::OnServerPacketRecv));
    replaceAddr(0x00DD440C, addr_from_this(&CPSMission::OnServerPacketRecv));
    replaceOffset(0x0070BCF2, addr_from_this(&CInterfaceNetSender::DonateGuildPoints));
    replaceAddr(0x00dd8134, addr_from_this(&CPSCharacterSelect::OnServerPacketRecv));

    //item linking
    replaceOffset(0x0063AA3C, addr_from_this(&CIFTextBox::OnHover));
    replaceOffset(0x0063AD88, addr_from_this(&CIFTextBox::OnHover));

    //stall network show hide
    replaceAddr(0x00d7110a + 1, addr_from_this(&CGInterface::OnKeyDown));

    //pick pet filter
    //replaceOffset(0x008AFCF9,addr_from_this(&CDropItemManager::sub_8AF7F0));

    //static alchemy materials
    patchAlchemy();

    //charname color fix
    replaceOffset(0x009D0221, addr_from_this(&CIGIDObject::UpdateNameColor));

    //account lock on start appear disappear
    replaceOffset(0x0085DDE1, addr_from_this(&CPSCharacterSelect::FUN_0085B680));
    replaceOffset(0x0085C488, addr_from_this(&CPSCharacterSelect::OnDisappear));

    //move to party member reverse
    replaceOffset(0x006ec77b, addr_from_this(&CIFMessageBox::SetMsgBoxHandler));
    //vftableHook(0x00d9b9cc,10, addr_from_this(&CIFMessageBox::OnCreate));

    //helperwnd size
    //replaceOffset(0x00687111, addr_from_this(&CIFHelperBubbleWindow::sub_777020));
    //onselect object 0093F670

    //patch title on jobmode
    SetNop((void*)0x009C28C5, 16);

    //fix item mall crash
    DWORD v10 = 0x2EE0;
    MEMUTIL_WRITE_VALUE(DWORD, 0x00BA2156 + 6, v10);

#ifdef CONFIG_DEBUG_REDIRECT_PUTDUMP
    replaceAddr(0x00832927 + 1, (int) &DebugPrintCallback);
#endif // CONFIG_DEBUG_REDIRECT_PUTDUMP

#ifdef CONFIG_TRANSLATIONS_DEBUG
    placeHook(0x008C9C30, addr_from_this(&CTextStringManager::GetString));
#endif // CONFIG_TRANSLATIONS_DEBUG

    replaceOffset(0x008774f4, (int)&WriteToChatWindow);
    replaceOffset(0x00877b5c, (int)&WriteToChatWindow);

    placeHook(0x007a9bd0, addr_from_this(&CIFChatViewer::ShowHideControls));

    quickstart.Setup();
}

bool DoesFileExists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void RegisterObject(const CGfxRuntimeClass *obj) {
    register_objects.push_back(obj);
}

void OverrideObject(overrideFnPtr fn) {
    override_objects.push_back(fn);
}

void InstallRuntimeClasses(CGame *) {
    // Replace Create & Delete for existing classes
    // Note: We can't just inject existing objects like we would do with new objects.
    //       Joymax uses == on GFX_RUNTIME_CLASS(), so we would end up breaking this comparison

    for (std::vector<const CGfxRuntimeClass *>::const_iterator it = register_objects.begin();
         it != register_objects.end(); ++it) {
        reinterpret_cast<void (__thiscall *)(const CGfxRuntimeClass *, const char *, void *, void *,const CGfxRuntimeClass *, size_t, int)>(0x00B9C9C0)(*it,(*it)->m_lpszClassName, (*it)->m_pfnCreateObject, (*it)->m_pfnDeleteObject, (*it)->m_pBaseClass, (*it)->m_nObjectSize, 0);
    }

    for (std::vector<overrideFnPtr>::const_iterator it = override_objects.begin(); it != override_objects.end(); ++it) {
        (*it)();
    }
}
std::n_wstring getFullMsg(int hoveredIndex)
{
    CIFChatViewer* chatView = (CIFChatViewer*)g_pCGInterface->m_IRM.GetResObj(1, 1);
    CIFListCtrl* pList = chatView->m_CurrentActiveChatlist;
    CIFListCtrl::SLineOfText* line;
    std::n_wstring str;
    int indexOffs = 0;
    std::n_wstring fullStr=L"";
    do
    {
        if((hoveredIndex - indexOffs) <0)
            break;
        line = pList->GetTextByIndex(hoveredIndex - indexOffs);
        if (!line)
            break;
        line->m_font->GetText(&str);
        if (str.empty())
            break;
        if(str.find(L":") == std::n_wstring::npos)
            indexOffs++;
        else
            break;
    } while (true);

    for(int i=indexOffs;i >=0;i--)
    {

        line = pList->GetTextByIndex(hoveredIndex - i);
        if(line)
        {
            line->m_font->GetText(&str);
            fullStr = fullStr+str;
        }
        if(indexOffs ==0)
        {
            line = pList->GetTextByIndex(hoveredIndex+1);
            if (line)
            {
                line->m_font->GetText(&str);
                if(str.find(L":") == std::n_wstring::npos)
                {
                    fullStr = fullStr+str;
                }
            }
        }
    }
    indexOffset = indexOffs;
    return fullStr;
}
std::string getCharName(std::n_wstring fullMsg)
{
    if(fullMsg.find(L"(FR") != std::string::npos)
        return acp_encode(fullMsg.substr(0, fullMsg.find_first_of(L"(")));
    else if(fullMsg.find(L"(Pa") != std::string::npos)
        return acp_encode(fullMsg.substr(0, fullMsg.find_first_of(L"(")));
    else if(fullMsg.find(L"(Gu") != std::string::npos)
        return acp_encode(fullMsg.substr(0, fullMsg.find_first_of(L"(")));
    else if(fullMsg.find(L"(TO") != std::string::npos)
        return TO_STRING(g_pCICPlayer->GetCharName());
    else
        return acp_encode(fullMsg.substr(0, fullMsg.find_first_of(L":")));
}
std::string getFullMsgA(std::n_wstring fullMsg)
{
    std::string fullStrA=acp_encode(fullMsg);
    size_t start_pos2 = 0;
    const std::string& from2="_";
    const std::string& to2="";
    while((start_pos2 = fullStrA.find(from2, start_pos2)) != std::string::npos) {
        fullStrA.replace(start_pos2, from2.length(), to2);
        start_pos2 += to2.length(); // Handles case where 'to' is a substring of 'from'
    }
    size_t start_pos = 0;
    const std::string& from="      ";
    const std::string& to="";
    while((start_pos = fullStrA.find(from, start_pos)) != std::string::npos) {
        fullStrA.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return fullStrA;
}
int getEmptySlotsCount()
{
    //TODO silly code :)
    int invCount = *(byte*) ((DWORD32)g_pCICPlayer + 0x2048);

    int count = 0;

    for (int i = 0; i < invCount; i++) {
        CIFInventory *pInventory = g_pCGInterface->GetMainPopup()->GetInventory();
        CIFSlotWithHelp* pSlot = *reinterpret_cast<CIFSlotWithHelp**>(pInventory->pSlots + (i * 4));

        if(!pSlot->ItemInfo)
            count++;
    }

    return count;
}
void disableItemLog(bool disable)
{
    BYTE Value_1 = 0xEB;
    BYTE Value_2 = 0x37;
    BYTE Value_3 = 0x75;
    if(!disable)
    {
        Value_1 = 0x72;
        Value_2 = 0x5;
        Value_3 = 0x74;
    }
    memoryAction(0x0087C1F0,2,false);
    *(BYTE*)(0x0087C1F0) = Value_1;
    *(BYTE*)(0x0087C1F0 + 1) = Value_2;
    memoryAction(0x0087C1F0,2,true);

    memoryAction(0x0087C15B,1,false);
    *(BYTE*)(0x0087C15B) = Value_3;
    memoryAction(0x0087C15B,1,true);

}
void patchAlchemy()
{
    JMPFunction(0x00059C95D, 0x0059CA04);
    SetNop((void*)0x0059C962, 3);
    SetNop((void*)0x0059CA0A, 1);
    SetNop((void*)0x0059CA24, 5);
    SetNop((void*)0x0059DF54, 6);
    SetNop((void*)0x0059DF5A, 1);
    SetNop((void*)0x0059DF61, 5);
    PatchMe(0x0059EB2F, 0x6A);
    PatchMe(0x0059EB2F + 1, 0x01);
    SetNop((void*)0x0059EECE, 16);
}