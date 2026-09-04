//
// Created by YUMBUL on 15.10.2024.
//

#include "CustomSettingManager.h"

CustomSettingManager* m_Settings;

CustomSettingManager* CustomSettingManager::getInstance() {
    if (m_Settings == NULL) {
        m_Settings = new CustomSettingManager();
    }
    return m_Settings;
}

CustomSettingManager::CustomSettingManager() {
    GrantName = false;
    IconManager = false;
    IconManagerRight = false;
    TitleManager = false;
    TitleManagerColor = false;
    RankingWnd = false;
    UniqueHistoryWnd = false;
    EventRegisterWnd = false;
    EventScheduleWnd = false;
    AchievementsWnd = false;
    SecondarySlot = false;
    OldLogin = false;
    OldExpBar = false;
    OldAlchemy = false;
    MoveSkillBoard = false;
    EnableServerInfoSkill = false;
    EnableOldMainPopup = false;
    HideOldTitleWhileNewTitle = false;
    ItemComparison = false;
    AutoSortButton = false;
    EnableAutoSkill = false;
    MaxMasteryLevel = 0;
    ServerMaxLevel = 0;
    EnableAutoStrInt = false;
    EnablePickSoxEffect = false;
    GuildJobMode = false;
    EnableUniqueTarget = false;
    EnableMacro = false;


    StartAutoSort = false;
    AutoSortRunning = false;

    NewCharInfoScreen = false;
    NewIDPWScreen = false;

    PSTitleIsLoaded = false;

    EnableChangeLog = false;
    ShowChangeLogFirstLogin = false;
    FixNewJobSuit = false;
    EnableOldItemMall = false;
    InsertCommaToPrices = false;
    EnableCharacterBound = false;
    EnableNewItemMall = false;
    ENABLE_EMOJI_SYSTEM = false;
    ENABLE_NEW_PT_MATCH = false;
    ENABLE_NEW_JOB_UI = false;
    ENABLE_NEW_ALCHEMY = false;
    EnableItemTranslation = false;
    ItemTranslationPayment = 0;
    ItemTranslationPrice = 0;

}