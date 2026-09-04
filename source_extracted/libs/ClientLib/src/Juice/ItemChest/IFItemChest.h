#include "IFItemChestSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFItemChest : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFItemChest)
GFX_DECLARE_MESSAGE_MAP(CIFItemChest)

private:
    CIFItemChest(void);
    ~CIFItemChest(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    bool OnPageChange(int type);
    void ClearRecords();
    void OnTakeAll();
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
public:
    CIFItemChestSlot** m_slots;
    CIFSpinButton* m_spin;
    int totalCount;
    CIFStatic* m_invText;
    CIFButton* m_takeallBtn;

};