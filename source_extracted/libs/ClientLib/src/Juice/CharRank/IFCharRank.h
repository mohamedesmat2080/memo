#include "IFCharRankSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/PopupList/IFPopupList.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFCharRank : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFCharRank)
GFX_DECLARE_MESSAGE_MAP(CIFCharRank)

private:
    CIFCharRank(void);
    ~CIFCharRank(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    bool OnPageChange(int type);
    void ClearRecords();
    void OnRankChange();
private:
    CIFFrame* m_frame;
    CIFNormalTile* m_tile;
    CIFNormalTile* m_tile2;
    CIFStretchWnd* m_stretch;
    CIFStatic* m_head;
    CIFStatic* m_head2;
    CIFStatic* m_head3;
    CIFStatic* m_head4;
    CIFStatic* m_head5;
    CIFStatic* m_head6;
    CIFStatic* m_infobg;
    CIFStatic* m_infotext;
    CIFStatic* m_searchtext;
    CIFButton* m_searchbtn;
public:
    CIFCharRankSlot** m_slots;
    CIFPopupList* m_popup;
    CIFStatic* m_state;
    CIFSpinButton* m_spin;
    int totalCount;
    std::map<std::n_wstring, int> RankInfo;
};