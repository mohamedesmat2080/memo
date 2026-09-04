#include "IFButton.h"
#include "IFSlotWithHelp.h"
#include <BSLib/_internal/custom_stl.h>
#include <BSLib/multibyte.h>
#include <Juice/AccLock/IFAccLock.h>
#include <map>
#include <string>

//item chat linking
struct itemInfo
{
    std::list<BYTE> listBytes;
    int Index;
};
extern std::map<std::string, itemInfo> ChatItemInfoList;
extern int indexOffset;
extern time_t lastlinkTime;


//title , color things
extern std::map<std::n_wstring, UINT32> m_CharacterIcons;
extern std::map<int, IDirect3DBaseTexture9*> m_IconsData;
extern std::map<std::n_wstring, UINT32> m_VipIcons;
extern std::map<int, IDirect3DBaseTexture9*> m_VipIconsData;
extern std::map<std::n_wstring, std::n_wstring> m_CharName;
extern std::map<std::n_wstring, int> m_CharNameColor;
extern std::map<std::n_wstring, std::n_wstring> m_CustomTitle;
extern std::map<std::n_wstring, int> m_CustomTitleColor;


//lock helpers
extern int focusedEditID;

//emoji
extern std::map<std::string, std::string> emojiList;
