#pragma once
#include <IFSelectableArea.h>
#include "IFMainFrame.h"
#include "IFEdit.h"
#include "IFAchievementsSlot.h"
#include <IFBarWnd.h>
#include <IFScrollManager.h>
#include <IFTextBox.h>
class CIFAchievements : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFAchievements)
GFX_DECLARE_MESSAGE_MAP(CIFAchievements)
private:
    CIFAchievements(void);
    ~CIFAchievements(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void On_BtnClick();
    void OnUnknownStuff();
    int Func_4(int a2) override;
    int Func_36(int a1, short action, int a3, int a4) override;

    CIFSelectableArea **m_pTabs; //0x04A4 is a pointer, type unknown
    static const int numberOfTabs = 2;
    static const int tabWidth = 74;
    static const int tabHeight = 24;
    static const int tabMarginLeft = 18;
    static const int tabFirstId = 100;
    CIFAchievementsSlot* m_slots[20];
    CIFScrollManager* m_scroll;
    CIFVerticalScroll* vscroll;
    CIFTextBox* descbox;
    void On_BtnClickAll();
    void On_BtnClickGeneral();
    void On_BtnClickQuest();
    void On_BtnClickUnique();
    void On_BtnClickMatch();
    void On_BtnClickEvent();
    void On_BtnClickRemove();
    void OnTimer(int) override;
    void RemoveButtonDelay(int timeoutSeconds);
    void UseButtonDelay(int timeoutSeconds);
public:
    void UpdateMenuSize();
    void Clear();
    void ClearDDJ();
    void ActivateTabPage(BYTE page);
    void SetDescBoxText(std::n_wstring string);
    void SetUseButtonState(bool s);

    int SelectedItemID;
    byte ActiveTabNumber;
    int LastActiveCategory;


void OnListUpdated();
};
