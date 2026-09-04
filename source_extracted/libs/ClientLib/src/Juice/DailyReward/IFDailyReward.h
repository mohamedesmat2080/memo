#include "IFDailyRewardSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>
class CIFDailyReward : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFDailyReward)
GFX_DECLARE_MESSAGE_MAP(CIFDailyReward)

private:
    CIFDailyReward(void);
    ~CIFDailyReward(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    bool OnPageChange(int type);
    void ClearRecords();
private:
    CIFNormalTile* m_tile;
public:
    CIFDailyRewardSlot** m_slot;
    CIFSpinButton* m_spin;
    int totalCount;
};