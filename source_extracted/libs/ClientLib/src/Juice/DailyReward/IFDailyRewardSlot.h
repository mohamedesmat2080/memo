#include <BSLib/_internal/_BSDynamic.h>
#include <IFButton.h>
#include <IFFrame.h>
#include <IFNormalTile.h>
#include <IFSlotWithHelp.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFDailyRewardSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFDailyRewardSlot)
GFX_DECLARE_MESSAGE_MAP(CIFDailyRewardSlot)
public:
    CIFDailyRewardSlot(void);
    ~CIFDailyRewardSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void FillTheSlot(int itemIDint,int itemCount);
    void OnTake();
public:
    CIFFrame* m_frame;
    CIFNormalTile* m_tile;
    CIFSlotWithHelp* m_slot;
    CIFStatic* m_Name;
    CIFStatic* m_Count;
    CIFButton* m_btn;
    CIFStatic* m_day;
    int index;
};