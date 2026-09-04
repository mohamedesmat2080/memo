#include "IFEventScheduleSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFEventSchedule : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFEventSchedule)
GFX_DECLARE_MESSAGE_MAP(CIFEventSchedule)

private:
    CIFEventSchedule(void);
    ~CIFEventSchedule(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    bool OnPageChange(int type);
    void ClearRecords();
private:
    CIFFrame* m_frame;
    CIFNormalTile* m_tile;
    CIFNormalTile* m_tile2;
    CIFStretchWnd* m_stretch;
    CIFStatic* m_head;
    CIFStatic* m_head2;
public:
    CIFEventScheduleSlot** m_slots;
    CIFSpinButton* m_spin;
    int totalCount;
};