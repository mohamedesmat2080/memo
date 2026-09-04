#include "CPSMission.h"
#include "../../../DevKit_DLL/src/Util.h"
#include "BSLib/BSLib.h"
#include "GameDataExt.h"
#include <BSLib\multiBYTE.h>
#include <GInterface.h>
#include <Game.h>
#include <GlobalDataManager.h>
#include <Juice/CharRank/IFCharRank.h>
#include <Juice/DailyReward/IFDailyReward.h>
#include <Juice/Dps/IFDps.h>
#include <Juice/EventSchedule/IFEventSchedule.h>
#include <Juice/Hwid/Hwid.h>
#include <Juice/ItemChest/IFItemChest.h>
#include <Juice/TitleManager/IFTitleManager.h>
#include <Juice/UniqueLog/IFUniqueLog.h>
#include <PSOuterInterface.h>
#include <TextStringManager.h>
#include <algorithm>
#include <sstream>
#include <string>


bool CPSMission::OnServerPacketRecv(CMsgStreamBuffer* msg) {
//    if(strcmp(g_CGame->str_gateway_server.c_str(),"185.98.63.100") != 0)
//    {
//        MessageBoxA(0, "Please contact me at discord if you saw this message Bimbum1337#5462", "Error", MB_OK);
//        std::exit(0);
//    }
    if (msg->msgid() == 0x180B) {
        std::n_string itemChatLinkKey;
        *msg >> itemChatLinkKey;
        std::list<BYTE> listBytes;
        while (msg->m_currentReadBytes != msg->m_availableBytesForReading) {
            BYTE x;
            *msg >> x;
            listBytes.push_back(x);
        }
        itemInfo info = itemInfo();
        info.listBytes = listBytes;
        ChatItemInfoList[itemChatLinkKey.c_str()] = info;
    }
    if (msg->msgid() == 0x180C) {
        byte totalCount, readCount, Index;
        *msg >> totalCount >> readCount;

        CIFCharRank *rankWnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(13392);
        rankWnd->ClearRecords();
        rankWnd->totalCount = totalCount;
        for (int i = 0; i < readCount; i++) {
            byte Race, Level;
            std::n_string Charname, Guildname;
            int Points;

            *msg >> Index >> Charname >> Race >> Level >> Points >> Guildname;

            wchar_t *str = new wchar_t[200];
            rankWnd->m_slots[i]->m_charnametext->SetText(TO_WSTRING(Charname).c_str());
            rankWnd->m_slots[i]->m_guildtext->SetText(TO_WSTRING(Guildname).c_str());
            wsprintfW(str, L"%d(%d)Lv", Level, Level);
            rankWnd->m_slots[i]->m_leveltext->SetText(str);
            wsprintfW(str, L"%d", Points);
            rankWnd->m_slots[i]->m_pointstext->SetText(str);
            wsprintfW(str, L"%d", Index + 1);
            rankWnd->m_slots[i]->m_ranktext->SetText(str);
            if (Race == 0)
                rankWnd->m_slots[i]->m_gradetext->TB_Func_13("interface\\ifcommon\\com_kindred_china.ddj", 0, 0);
            else
                rankWnd->m_slots[i]->m_gradetext->TB_Func_13("interface\\ifcommon\\com_kindred_europe.ddj", 0, 0);
        }
        rankWnd->m_state->SetText(L"");
    }
    if (msg->msgid() == 0x180D) {
        UINT16 totalCount;
        byte readCount, State;
        *msg >> totalCount >> readCount;

        CIFUniqueLog *uniqueWnd = (CIFUniqueLog *) g_pCGInterface->GetWindowByUniqueID(13393);
        uniqueWnd->ClearRecords();
        uniqueWnd->totalCount = totalCount;

        for (int i = 0; i < readCount; i++) {
            int refObjID;
            std::n_string Killer, Time;
            *msg >> refObjID >> Killer >> State >> Time;

            std::wstring uniqueName = L"<No name>";
            const CCharacterData *data = g_CGlobalDataManager->GetCharacterData(refObjID);
            if (data != NULL)
                uniqueName = g_CTextStringManager->GetStringByNameStrID(data->data.field_0x60.c_str())->c_str();

            uniqueWnd->m_slots[i]->m_uniquetext->SetText(uniqueName.c_str());
            uniqueWnd->m_slots[i]->m_killertext->SetText(TO_WSTRING(Killer).c_str());
            uniqueWnd->m_slots[i]->m_datetext->SetText(TO_WSTRING(Time).c_str());
            uniqueWnd->m_slots[i]->m_statetext->SetText(State == 0x1 ? L"Alive" : L"Dead");

            uniqueWnd->m_slots[i]->m_uniquetext->m_FontTexture.SetColor(State == 0x1 ? D3DCOLOR_RGBA(0, 255, 0, 255) : D3DCOLOR_RGBA(255, 0, 0, 255));
            uniqueWnd->m_slots[i]->m_killertext->m_FontTexture.SetColor(State == 0x1 ? D3DCOLOR_RGBA(0, 255, 0, 255) : D3DCOLOR_RGBA(255, 0, 0, 255));
            uniqueWnd->m_slots[i]->m_datetext->m_FontTexture.SetColor(State == 0x1 ? D3DCOLOR_RGBA(0, 255, 0, 255) : D3DCOLOR_RGBA(255, 0, 0, 255));
            uniqueWnd->m_slots[i]->m_statetext->m_FontTexture.SetColor(State == 0x1 ? D3DCOLOR_RGBA(0, 255, 0, 255) : D3DCOLOR_RGBA(255, 0, 0, 255));
        }
    }
    if (msg->msgid() == 0x180E) {
        byte totalCount, readCount;
        *msg >> totalCount >> readCount;

        CIFEventSchedule *eventWnd = (CIFEventSchedule *) g_pCGInterface->GetWindowByUniqueID(13394);
        eventWnd->ClearRecords();
        eventWnd->totalCount = totalCount;

        for (int i = 0; i < readCount; i++) {
            std::n_string eventName, timeRemain;
            *msg >> eventName >> timeRemain;
            eventWnd->m_slots[i]->m_eventtext->SetText(TO_WSTRING(eventName).c_str());
            eventWnd->m_slots[i]->m_datetext->SetText(TO_WSTRING(timeRemain).c_str());
        }
    }
    if (msg->msgid() == 0x180F) {
        std::n_string path;
        int IconID, IconsCount;
        *msg >> IconsCount;

        for (int i = 0; i < IconsCount; i++) {
            *msg >> IconID >> path;
            m_IconsData.insert(std::pair<int, IDirect3DBaseTexture9 *>(IconID, (IDirect3DBaseTexture9 *) Fun_CacheTexture_Create(path)));
        }
    }
    if (msg->msgid() == 0x181A) {
        std::n_string CharName;
        int IconID, Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> IconID >> CharName;
            std::map<std::n_wstring, UINT32>::iterator foundedInfo = m_CharacterIcons.find(TO_NWSTRING(CharName).c_str());
            if (foundedInfo != m_CharacterIcons.end())
                m_CharacterIcons[TO_NWSTRING(CharName).c_str()] = IconID;
            else
                m_CharacterIcons.insert(std::pair<std::n_wstring, UINT32>(TO_NWSTRING(CharName).c_str(), IconID));
        }
    }
    if (msg->msgid() == 0x181B) {
        std::n_string path;
        int IconID, IconsCount;
        *msg >> IconsCount;

        for (int i = 0; i < IconsCount; i++) {
            *msg >> IconID >> path;
            m_VipIconsData.insert(std::pair<int, IDirect3DBaseTexture9 *>(IconID, (IDirect3DBaseTexture9 *) Fun_CacheTexture_Create(path)));
        }
    }
    if (msg->msgid() == 0x181C) {
        std::n_string CharName;
        int IconID, Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> IconID >> CharName;
            std::map<std::n_wstring, UINT32>::iterator foundedInfo = m_VipIcons.find(TO_NWSTRING(CharName).c_str());
            if (foundedInfo != m_VipIcons.end())
                m_VipIcons[TO_NWSTRING(CharName).c_str()] = IconID;
            else
                m_VipIcons.insert(std::pair<std::n_wstring, UINT32>(TO_NWSTRING(CharName).c_str(), IconID));
        }
    }
    if (msg->msgid() == 0x181D) {
        std::n_string CharName, charnameRank;
        int Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> CharName >> charnameRank;
            std::map<std::n_wstring, std::n_wstring>::iterator foundedChar = m_CharName.find(TO_NWSTRING(CharName).c_str());
            if (foundedChar != m_CharName.end())
                m_CharName[TO_NWSTRING(CharName).c_str()] = TO_NWSTRING(charnameRank).c_str();
            else
                m_CharName.insert(std::pair<std::n_wstring, std::n_wstring>(TO_NWSTRING(CharName).c_str(), TO_NWSTRING(charnameRank).c_str()));
        }
    }
    if (msg->msgid() == 0x181E) {
        std::n_string CharName;
        int Color, Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> Color >> CharName;
            std::map<std::n_wstring, int>::iterator foundedInfo = m_CharNameColor.find(TO_NWSTRING(CharName).c_str());
            if (foundedInfo != m_CharNameColor.end())
                m_CharNameColor[TO_NWSTRING(CharName).c_str()] = Color;
            else
                m_CharNameColor.insert(std::pair<std::n_wstring, int>(TO_NWSTRING(CharName).c_str(), Color));
        }
    }
    if (msg->msgid() == 0x181F) {
        std::n_string CharName, titleName;
        int Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> CharName >> titleName;
            std::map<std::n_wstring, std::n_wstring>::iterator foundedChar = m_CustomTitle.find(TO_NWSTRING(CharName).c_str());
            if (foundedChar != m_CustomTitle.end())
                m_CustomTitle[TO_NWSTRING(CharName).c_str()] = TO_NWSTRING(titleName).c_str();
            else
                m_CustomTitle.insert(std::pair<std::n_wstring, std::n_wstring>(TO_NWSTRING(CharName).c_str(), TO_NWSTRING(titleName).c_str()));
        }
    }
    if (msg->msgid() == 0x182A) {
        std::n_string CharName;
        int Color, Count;
        *msg >> Count;

        for (int i = 0; i < Count; i++) {
            *msg >> Color >> CharName;
            std::map<std::n_wstring, int>::iterator foundedInfo = m_CustomTitleColor.find(TO_NWSTRING(CharName).c_str());
            if (foundedInfo != m_CustomTitleColor.end())
                m_CustomTitleColor[TO_NWSTRING(CharName).c_str()] = Color;
            else
                m_CustomTitleColor.insert(std::pair<std::n_wstring, int>(TO_NWSTRING(CharName).c_str(), Color));
        }
    }
    if (msg->msgid() == 0x182B) {
        int totalCount;
        byte readCount;
        *msg >> totalCount >> readCount;

        CIFItemChest *chestWnd = (CIFItemChest *) g_pCGInterface->GetWindowByUniqueID(13395);
        chestWnd->totalCount = totalCount;
        chestWnd->ClearRecords();

        for (int i = 0; i < readCount; i++) {
            int ItemID, Count, Index;
            std::n_string From, Date;
            *msg >> Index >> ItemID >> From >> Count >> Date;

            const CItemData::SData *data = &g_CGlobalDataManager->GetItemData(ItemID);

            std::wstring NameStrID128 = g_CTextStringManager->GetStringByNameStrID(
                                                                data->m_nameStrId128)->c_str();
            chestWnd->m_slots[i]->m_itemText->SetText(NameStrID128.c_str());

            chestWnd->m_slots[i]->m_takeBtn->ShowGWnd(true);
            chestWnd->m_slots[i]->m_fromText->SetText(TO_WSTRING(From).c_str());

            wchar_t *str = new wchar_t[200];
            wsprintfW(str, L"%d", Count);
            chestWnd->m_slots[i]->m_countText->SetText(str);

            chestWnd->m_slots[i]->FillTheSlot(ItemID, Count);

            chestWnd->m_slots[i]->m_takeBtn->setTag((void *) Index);
            chestWnd->m_slots[i]->m_dateText->SetText(TO_WSTRING(Date).c_str());
        }
    }
    if (msg->msgid() == 0x182C) {
        int count;
        *msg >> count;

        CIFTitleManager *titleWnd = (CIFTitleManager *) g_pCGInterface->GetWindowByUniqueID(13396);

        int color;
        std::map<std::n_wstring, int>::iterator findCustomTitleColor = m_CustomTitleColor.find(g_pCICPlayer->GetName().c_str());
        if (findCustomTitleColor != m_CustomTitleColor.end()) {
            color = findCustomTitleColor->second;
        } else
            color = 0xffff00;

        for (int i = 0; i < count; i++) {
            std::n_string title;
            *msg >> title;

            std::n_wstring wTitle = TO_NWSTRING(title);
            titleWnd->m_textbox->sub_64F8A0(wTitle, 0, color, color, -1, 0, 0);
        }
    }
    if (msg->msgid() == 0x182D) {
        byte type;
        *msg >> type;
        std::n_string CharName;
        *msg >> CharName;
        switch (type) {
            case 0: {
                std::map<std::n_wstring, std::n_wstring>::iterator foundedChar = m_CustomTitle.find(TO_NWSTRING(CharName).c_str());
                if (foundedChar != m_CustomTitle.end()) {
                    m_CustomTitle.erase(TO_NWSTRING(CharName).c_str());
                }

                CLASSLINK_LOOP_BEGIN(CICUser)
                if (wcscmp(obj->GetName().c_str(), TO_NWSTRING(CharName).c_str()) == 0)
                {
                    std::n_wstring title(L"");
                    if(obj->m_hwanlevel > 0)
                        obj->SetTitleName(obj->m_hwanlevel);
                    else
                        obj->fonttexture_title.sub_8B3B60(&title);
                }
                CLASSLINK_LOOP_END(CICUser)
            } break;
            case 1: {
                std::map<std::n_wstring, std::n_wstring>::iterator foundedChar = m_CharName.find(TO_NWSTRING(CharName).c_str());
                if (foundedChar != m_CharName.end()) {
                    m_CharName.erase(TO_NWSTRING(CharName).c_str());
                }

                CLASSLINK_LOOP_BEGIN(CICUser)
                if (wcscmp(obj->GetName().c_str(), TO_NWSTRING(CharName).c_str()) == 0) {
                    std::n_wstring name = TO_NWSTRING(CharName);
                    obj->fonttexture_playername.sub_8B3B60(&name);
                }
                CLASSLINK_LOOP_END(CICUser)
            } break;
            case 2: {
                std::map<std::n_wstring, UINT32>::iterator foundedIcon = m_CharacterIcons.find(TO_NWSTRING(CharName).c_str());
                if (foundedIcon != m_CharacterIcons.end())
                    m_CharacterIcons.erase(TO_NWSTRING(CharName).c_str());
            } break;
            case 3: {
                std::map<std::n_wstring, UINT32>::iterator foundedIcon = m_VipIcons.find(TO_NWSTRING(CharName).c_str());
                if (foundedIcon != m_VipIcons.end())
                    m_VipIcons.erase(TO_NWSTRING(CharName).c_str());
            } break;
            case 5: {
                std::map<std::n_wstring, int>::iterator foundedChar = m_CharNameColor.find(TO_NWSTRING(CharName).c_str());
                if (foundedChar != m_CharNameColor.end())
                    m_CharNameColor.erase(TO_NWSTRING(CharName).c_str());
            } break;
            case 6:
            {
                std::map<std::n_wstring, int>::iterator foundedChar = m_CustomTitleColor.find(TO_NWSTRING(CharName).c_str());
                if (foundedChar != m_CustomTitleColor.end())
                    m_CustomTitleColor.erase(TO_NWSTRING(CharName).c_str());
            }
             break;
        }
    }
    if (msg->msgid() == 0x182E)
    {
        CIFTitleManager *achievWnd = (CIFTitleManager *) g_pCGInterface->GetWindowByUniqueID(13396);
        int count;
        *msg >> count;

        int color;
        std::map<std::n_wstring, int>::iterator find = m_CharNameColor.find(g_pCICPlayer->GetName().c_str());
        if (find != m_CharNameColor.end()) {
            color = find->second;
        } else
            color = 0xffffff;

        for(int i=0;i<count;i++)
        {
            std::n_string achievment,Descr;
            byte isEnabled;
            *msg >> achievment >> Descr >>isEnabled;

            std::n_wstring wACh = TO_NWSTRING(achievment);
            achievWnd->m_textbox->sub_64F8A0(wACh, 0, color, color, -1, 0, 0);
            achievWnd->achievements[TO_NWSTRING(achievment)].Descr = TO_NWSTRING(Descr);
            achievWnd->achievements[TO_NWSTRING(achievment)].isEnabled = isEnabled;
        }
    }
    if (msg->msgid() == 0x182F)
    {
        int count,totalCount;
        *msg >> count >> totalCount;

        CIFDailyReward *dailyWnd = (CIFDailyReward *) g_pCGInterface->GetWindowByUniqueID(13398);
        dailyWnd->ClearRecords();
        dailyWnd->totalCount = totalCount;

        for(int i=0;i<count;i++)
        {
            int index,refObjID,itemCount;
            byte isTaken;
            *msg >> index >> refObjID >> itemCount >> isTaken;

            dailyWnd->m_slot[i]->index = index;
            std::string imgStr= "juicer\\days\\day_";
            std::ostringstream temp;
            temp << index+1;
            imgStr = imgStr + temp.str() + ".ddj";
            dailyWnd->m_slot[i]->m_day->TB_Func_13(imgStr.c_str(),0,0);
            wchar_t *str = new wchar_t[200];
            wsprintfW(str, L"%dx", itemCount);
            dailyWnd->m_slot[i]->m_Count->SetText(str);

            switch(isTaken)
            {
                case 0:
                    dailyWnd->m_slot[i]->m_btn->SetText(L"Claim Reward");
                    dailyWnd->m_slot[i]->m_btn->SetEnabledState(true);
                    break;
                case 1:
                    dailyWnd->m_slot[i]->m_btn->SetText(L"Taken");
                    dailyWnd->m_slot[i]->m_btn->SetEnabledState(false);
                    break;
                case 3:
                    dailyWnd->m_slot[i]->m_btn->SetText(L"Claim Reward");
                    dailyWnd->m_slot[i]->m_btn->SetEnabledState(false);
                    break;
            }
            dailyWnd->m_slot[i]->FillTheSlot(refObjID,itemCount);
            dailyWnd->m_slot[i]->ShowGWnd(true);

        }
    }
    if (msg->msgid() == 0x183A)
    {
        CIFDps* dpsWnd = (CIFDps*) g_pCGInterface->GetWindowByUniqueID(13400);
        dpsWnd->ShowGWnd(true);

        int refObjID,Count;
        *msg >> refObjID >> Count;

        std::wstring uniqueName = L"<No name>";
        const CCharacterData *data = g_CGlobalDataManager->GetCharacterData(refObjID);
        if (data != NULL)
            uniqueName = g_CTextStringManager->GetStringByNameStrID(data->data.field_0x60.c_str())->c_str();

        dpsWnd->m_title->SetText(uniqueName.c_str());
        dpsWnd->ClearRecords();

        for(int i=0;i<Count;i++)
        {
            std::n_string charname,dmg;
            *msg >> charname >> dmg;

            dpsWnd->m_slots[i]->m_playername->SetText(TO_WSTRING(charname).c_str());
            dpsWnd->m_slots[i]->m_dmg->SetText(TO_WSTRING(dmg).c_str());
        }
    }
    if (msg->msgid() == 0x183B)
    {
        BYTE Type;
        std::n_string notice;

        *msg >> Type >> notice;
        switch(Type)
        {
            case 1:
                g_pCGInterface->ShowMessage_Notice(Convert(notice));
                g_pCGInterface->GetSystemMessageView()->WriteMessage(0xff,D3DCOLOR_RGBA(0x68, 0xBB, 0xE3, 255), Convert(notice),0,1);
                break;
            case 2:
                g_pCGInterface->ShowMessage_Warning(Convert(notice));
                g_pCGInterface->GetSystemMessageView()->WriteMessage(0xff,D3DCOLOR_RGBA(0x68, 0xBB, 0xE3, 255), Convert(notice),0,1);
                break;
            case 3:
                g_pCGInterface->ShowMessage_Quest(Convert(notice));
                g_pCGInterface->GetSystemMessageView()->WriteMessage(0xff,D3DCOLOR_RGBA(0x2E, 0xFF, 0x2E, 255), Convert(notice),0,1);
                break;
        }
    }
    if(msg->msgid() == 0x183C)
    {
        int RankID,Count;
        std::n_string RankName;

        *msg >> Count;
        for(int i=0;i<Count;i++)
        {
            *msg >> RankID >> RankName;

            CIFCharRank *rankWnd = (CIFCharRank *) g_pCGInterface->GetWindowByUniqueID(13392);
            rankWnd->RankInfo[TO_NWSTRING(RankName)] = RankID;

            wchar_t buffer[255];
            std::n_wstring strmsg;
            swprintf_s(buffer, sizeof(buffer), TO_NWSTRING(RankName).c_str());
            strmsg.assign(buffer);
            rankWnd->m_popup->m_list->sub_64F8A0(strmsg, 0, 0xffffff, 0xffffff, -1, 0, 0);
        }
    }
    return reinterpret_cast<int(__thiscall*)(CPSMission*, CMsgStreamBuffer*)>(0x0084CAB0)(this, msg);
}

wchar_t* CPSMission::Convert(std::n_string name)
{
    wchar_t* szName;
    const char* p = name.c_str();
    int nChars = MultiByteToWideChar(CP_ACP, 0, p, -1, NULL, 0);
    szName = new WCHAR[nChars];
    MultiByteToWideChar(CP_ACP, 0, p, -1, (LPWSTR)szName, nChars);
    return szName;
}

//bool CPSVersionCheck::OnServerPacketRecv(CMsgStreamBuffer* msg) {
//
//	return reinterpret_cast<int(__thiscall*)(CPSVersionCheck*, CMsgStreamBuffer*)>(0x0086d2a0)(this, msg);
//}
bool CPSTitle::OnServerPacketRecv(CMsgStreamBuffer* msg) {
    if(msg->msgid() == 0x183B)
    {
        byte Type;
        *msg >> Type;
        switch(Type)
        {
            case 1:
                std::n_string notice = "You have reached the maximum allowed accounts per one ip.";
                ShowMessage(Convert(notice), 0xff0000);
                break;
        }
    }
    if (msg->msgid() == 0xB003) //notice
    {
        int Color;
        std::n_string notice;
        *msg >> notice >> Color;

        this->ShowMessage(TO_WSTRING(notice).c_str(), Color);

        msg->m_currentReadBytes = 0;
    }

    return reinterpret_cast<int(__thiscall*)(CPSTitle*, CMsgStreamBuffer*)>(0x0086bfc0)(this, msg);
}
wchar_t* CPSTitle::Convert(std::n_string name)
{
    wchar_t* szName;
    const char* p = name.c_str();
    int nChars = MultiByteToWideChar(CP_ACP, 0, p, -1, NULL, 0);
    szName = new WCHAR[nChars];
    MultiByteToWideChar(CP_ACP, 0, p, -1, (LPWSTR)szName, nChars);
    return szName;
}
bool CPSTitle::OnCreateTMEP(long ln)
{
    bool b = reinterpret_cast<bool(__thiscall*)(CPSTitle*, long)>(0x0086b190)(this, ln);

    return b;
}


CIFWnd* CIFTargetWindow::MobWindow()
{
    return  this->m_IRM.GetResObj(2, 1);
}

#define GP_LIMIT 0x7FFFFFFF

void CInterfaceNetSender::DonateGuildPoints(unsigned int GP)
{
    if (GuildSkillPoints >= GP_LIMIT)
    {
        if (g_pCGInterface)
        {
            g_pCGInterface->ShowMessage_Warning (L"ÄúÒÑ´ïµ½¹«»á¼¼ÄÜµãÊýÉÏÏÞ.");
            g_pCGInterface->WriteSystemMessage(SYSLOG_NONE, L"ÄúÒÑ´ïµ½¹«»á¼¼ÄÜµãÊýÉÏÏÞ.");
        }
    }
    else
    {
        unsigned int TotalGP = GuildSkillPoints + GP;
        if (TotalGP > GP_LIMIT && GuildSkillPoints < GP_LIMIT)
            GP = GP_LIMIT - GuildSkillPoints;
        reinterpret_cast<void(__thiscall*)(CInterfaceNetSender*, unsigned int)>(0x0081FAB0)(this, GP);
    }
}

