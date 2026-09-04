// CustomDataManager.cpp

#include "CustomDataManager.h"

CustomDataManager* m_CustomDataManager;

CustomDataManager* CustomDataManager::getInstance() {
    if (m_CustomDataManager == NULL) {
        m_CustomDataManager = new CustomDataManager();
    }
    return m_CustomDataManager;
}

CustomDataManager::CustomDataManager() {
    _ActiveTitleNameNew = std::map<std::n_wstring, std::n_wstring>();
    _ActiveTitleColors = std::map<std::n_wstring, unsigned int>();
    m_IconsData = std::map<int, void*>();
    MediaIcons = std::map<int, std::n_string>();
    m_LeftCharIcons = std::map<std::n_wstring, unsigned __int32>();
    m_RightCharIcons = std::map<std::n_wstring, unsigned __int32>();

    m_RefAchievement = std::map<int, Achievements>();
    m_RefAchievementCondition = std::vector<SRefAchievementCondition>();

    UniqueTargetHashmap = std::map<int, int>();
    UniqueTargetHashmapPlayer = std::map<int, int>();

    g_GroupSpawn_Type = NULL;
    g_despawned_objects = std::vector<DWORD>();

    m_RefFellowPetSystem = std::map<std::n_wstring, FellowPetStruct>();
    m_EventMapSettings = std::map<int, SEventMapSettings>();;
    CharInfo = std::vector<CharInfoStruct>();


    FacebookUrl=  std::n_string();
    DiscordUrl=  std::n_string();
    WebSiteUrl=  std::n_string();
    MapIcon = NULL;
    CustomItemMallItemList = std::map<int, CustomItemMallItemStruct>();
    AvatarMallItemList = std::map<int, AvatarMallStruct>();
    HideEffects = std::map<int, SHideEffect>();

    emojiList = std::map<std::wstring, std::string>();
    emojiListData = std::map<std::string, IDirect3DBaseTexture9 *>();

    m_NewAlchemyProgress = 0;
    m_NewAlchemyWorking = false;

    m_NpcNewUIAction = 750;
    m_NpcNewUIClose = 15000;
    m_NpcNewUICallTG = 30000;
}
