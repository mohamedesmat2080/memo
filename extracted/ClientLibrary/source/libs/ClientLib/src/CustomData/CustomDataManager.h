#pragma once
#include <map>
#include <string>
#include <BSLib/_internal/custom_stl.h>
#include <Rpc.h>
#include <d3d9.h>


class CustomDataManager {
private:
    CustomDataManager();
public:
    // Singleton pattern için getInstance() fonksiyonu ekleyebilirsiniz.
    CustomDataManager* getInstance();

    std::map<std::n_wstring, unsigned int> _ActiveTitleColors;
    std::map<int, void*> m_IconsData;
    std::map<int, std::n_string> MediaIcons;



    std::map<std::n_wstring, unsigned __int32> m_LeftCharIcons;
    std::map<std::n_wstring, unsigned __int32> m_RightCharIcons;
    std::map<std::n_wstring, std::n_wstring> _ActiveTitleNameNew;
    struct Achievements
    {
        int ID;
        byte Category;
        std::wstring Name;
        byte RewardType;
        std::wstring RewardTitleName;
        int RewardSkillPoint;
        __int64 RewardGold;
    };
    std::map<int, Achievements> m_RefAchievement;

    struct SRefAchievementCondition
    {
        int ID;
        std::wstring Name;
        int RefAchievementID;
        __int64 CompleteCount;
        byte Type;
    };
    std::vector<SRefAchievementCondition> m_RefAchievementCondition;



    std::map<int, int> UniqueTargetHashmap;
    std::map<int, int> UniqueTargetHashmapPlayer;


    unsigned __int8 g_GroupSpawn_Type;
    std::vector<DWORD> g_despawned_objects;
    struct FellowPetStruct
    {
        std::n_wstring NameStrID;
        int SkillID_1;
        int SkillID_2;
        int SkillID_3;
        int SkillID_4;
        int SkillID_5;


        byte Active_Level_1;
        byte Active_Level_2;
        byte Active_Level_3;
        byte Active_Level_4;
        byte Active_Level_5;

        byte SkillType_1;
        byte SkillType_2;
        byte SkillType_3;
        byte SkillType_4;
        byte SkillType_5;

        int SelfSkill_1;
        byte SelfSkill_Active_Level_1;
        int SelfSkill_2;
        byte SelfSkill_Active_Level_2;
    };

    std::map<std::n_wstring, FellowPetStruct> m_RefFellowPetSystem;

    struct SEventMapSettings
    {
        int RegionID;
        bool EventSuit;
        bool HideBuffViewer;
        bool DisablePetSpawn;
        bool DisableParty;
        bool AutoCape;
        bool HideMiniMap;
    };
    std::map<int, SEventMapSettings> m_EventMapSettings;

    struct CharInfoStruct
    {
        std::n_wstring CharName16;
        std::n_wstring RegionName;
        byte DeleteStatus;
    };
    std::vector<CharInfoStruct> CharInfo;

    std::n_string FacebookUrl;
    std::n_string DiscordUrl;
    std::n_string WebSiteUrl;
    void* MapIcon;
    void* PingIcon;




    struct CustomItemMallItemStruct
    {
        int ID;
        std::n_wstring Category;
        byte Type;
        int ItemID;;
        int ItemCount;
        int SilkPrice;
        bool ShowInNewBest;
        int ItemIndex;
        int NewBestItemIndex;
    };
    std::map<int, CustomItemMallItemStruct> CustomItemMallItemList;

    struct AvatarMallStruct
    {
        int ID;
        int CategoryID;
        int ItemID;
        int Silk;
        int ItemIndex;
        int PetObjID;
    };
    std::map<int, AvatarMallStruct> AvatarMallItemList;

    struct SHideEffect
    {
        int SkillID;
        bool JobMode;
        bool MapSettings;
    };
    std::map<int, SHideEffect> HideEffects;
    std::map<std::wstring, std::string> emojiList;
    std::map<std::string, IDirect3DBaseTexture9 *> emojiListData;
    void * font;
    int m_NewAlchemyProgress;
    bool m_NewAlchemyWorking;
    int m_NpcNewUIAction;
    int m_NpcNewUIClose;
    int m_NpcNewUICallTG;
};

extern CustomDataManager* m_CustomDataManager;
