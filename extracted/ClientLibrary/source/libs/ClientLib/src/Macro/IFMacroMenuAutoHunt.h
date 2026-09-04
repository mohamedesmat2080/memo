#pragma once
#include <IFSelectableArea.h>
#include <IFCheckBox.h>
#include <IFScrollManager.h>
#include <IObject.h>
#include <IFEdit.h>
#include <ICharactor.h>
#include <CustomInterface/IFPopupList.h>
#include <CustomInterface/IFPopupList2.h>
#include "d3dx9math.h"
#include "IFMacroMenuAutoHuntPartySlot.h"

enum eRepairSetting : byte
{
    USE_REPAIR_HAMMER,
    RETURN_TOWN,
};
enum eBackTownHour : byte
{
    HOUR_1 = 0,
    HOUR_3 = 1,
    HOUR_5 = 2,
    HOUR_10 = 3,
    HOUR_24 = 4
};
enum eTownSetting : byte
{
    RES_SCROLL = 0,
    GO_TOWN = 1,
    WAIT_FOR_RES = 2
};
enum eZerkSetting : byte
{
    All = 0,
    Giant = 1,
};
enum eAutoHuntSetting : byte
{
    RADUIS_SETTING = 0,
    ZERK_SETTING = 1,
    RETURN_TOWN_SETTING = 2,
    BACK_HOUR_SETTING = 3,
    REPAIR_SETTING = 4
};
class CIFMacroMenuAutoHunt : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFMacroMenuAutoHunt)
GFX_DECLARE_MESSAGE_MAP(CIFMacroMenuAutoHunt)
private:
    CIFMacroMenuAutoHunt(void);
    ~CIFMacroMenuAutoHunt(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    int Func_4(int a2) override;
    void OnUnknownStuff();
    CIFSelectableArea** m_pTabsSecond;
    static const int numberOfTabs = 2;
    static const int tabWidth = 200;
    static const int tabHeight = 24;
    static const int tabFirstId = 300;
    undefined1 OnCloseWnd() override;


public:
    bool Macro_AutoHunt;
    CIFPopupList2* m_popup;

    bool MacroAutoTownTimerRunning;
    bool AutoHuntTimerRunning;
    bool MacroAutoInviteRunning;

    CIFCheckBox* AutoPartyInviteCheckBox;
    CIFCheckBox* AutoPartyAcceptCheckBox;
    CIFCheckBox* AutoPartyOnlyFromList;
    CIFCheckBox* GDR_PT_EXP_FREE_ITEM_AUTO_CHECKBOX;
    CIFCheckBox* GDR_PT_EXP_AUTO_ITEM_NOCheckBox;
    CIFCheckBox* GDR_PT_EXP_AUTO_ITEM_AUTOCheckBox;
    CIFCheckBox* GDR_PT_EXP_FREE_ITEM_FREECheckBox;
    CIFCheckBox* GDR_ACCEPT_JOIN_PT_REQ;
    CIFCheckBox* GDR_AUTO_REFORM_PARTY;
    CIFCheckBox* DONT_ATTACK_MONSTERCB;
    CIFCheckBox* AutoResPtMembersCB;
    CIFEdit* GDR_AUTO_REFORM_TITLE;
    CIFEdit* GDR_AUTO_REFORM_PT_LVL_MIN;
    CIFEdit* GDR_AUTO_REFORM_PT_LVL_MAX;

    CIFCheckBox* AutoReturnHPMPLessCheckbox;
    CIFCheckBox* AutoUseNasrun;
    CIFCheckBox* AutoUseNasrunIgnore;
    CIFPopupList* RadiusList;
    CIFPopupList* ZerkList;
    CIFPopupList* TownList;
    CIFPopupList* HourList;
    CIFPopupList* RepairList;
    void UpdateMenuSize();
    void ActivateTabPage(BYTE page);
    void LoadSetting();

    void SaveButton();
    void CancelBtn();

    void AddPtMemberBtn();
    void RemovePtMemberBtn();

    std::n_wstring SelectedPartyMemberName;
    std::map<std::n_wstring, std::n_wstring> AutoPartyMemberList;
    CIFMacroMenuAutoHuntPartySlot* m_PartySlot[7];
    std::map<eAutoHuntSetting, int> AutoHuntSetting;

    uregion GetRegion() const;
    D3DVECTOR GetLocation() const;
    void BackTown();
    void CheckNasrun();
    void CheckNasrunIgnore();
    void ClearPartySlotDDJ();
    void InviteNearPartyMembers();
    void PartyExpItemSetting();
    void LoadPartyExpItemSetting(int Id);
    void AutoReformParty();

    CICharactor* LastSelectedMonster;
    uregion StartRegion;
    D3DVECTOR StartPosition;
    CRTNavCellQuad* m_navcell; //0x0074
    void* m_object_under_foot; //0x0078
void StartAutoHunt();
void LoadPartyMembers();
};
