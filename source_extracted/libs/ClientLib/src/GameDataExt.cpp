#include "GameDataExt.h"

//item chat linking
std::map<std::string, itemInfo> ChatItemInfoList;
int indexOffset;
time_t lastlinkTime;


//title , color things
std::map<std::n_wstring, UINT32> m_CharacterIcons;
std::map<int, IDirect3DBaseTexture9*> m_IconsData;
std::map<std::n_wstring, UINT32> m_VipIcons;
std::map<int, IDirect3DBaseTexture9*> m_VipIconsData;
std::map<std::n_wstring, std::n_wstring> m_CharName;
std::map<std::n_wstring, int> m_CharNameColor;
std::map<std::n_wstring, std::n_wstring> m_CustomTitle;
std::map<std::n_wstring, int> m_CustomTitleColor;

//selectscreen passcode
int focusedEditID;

//emoji
std::map<std::string, std::string> emojiList;
