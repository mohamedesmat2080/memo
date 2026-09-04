#pragma once
#include <IFVerticalScroll.h>
#include "IFMainFrame.h"
#include "IFEdit.h"
#include "IFUniqueHistorySlot.h"

class CIFUniqueHistory : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFUniqueHistory)
GFX_DECLARE_MESSAGE_MAP(CIFUniqueHistory)
private:
    CIFUniqueHistory(void);
    ~CIFUniqueHistory(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void On_BtnClick();
public:
    undefined1 OnCloseWnd() override;
    void UpdateMenuSize();
    void Clear();
    void ClearDDJ();
    void UpdateRanks();

    bool DpsEnabled();
    struct UniqueHistory
    {
        std::wstring UniqueName;
        byte Status;
        long Elapsedtime;
        std::wstring Killer;
        int RegionID;
        int KilledX;
        int KilledY;
        int KilledZ;
    };
    std::map<int, UniqueHistory> UniqueHistoryList;

    int SelectedRegionID;
    int SelectedX;
    int SelectedZ;
    int SelectedY;

    int m_CurrentIndex;
    int m_MaxIndex;
void On_PrevBtn();
void On_NextBtn();
void LoadPage(int pageNumber);
void UpdateText(int Number);
};
