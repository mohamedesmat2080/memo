#include "IFUniqueLogSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFUniqueLog : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFUniqueLog)
GFX_DECLARE_MESSAGE_MAP(CIFUniqueLog)

private:
    CIFUniqueLog(void);
    ~CIFUniqueLog(void);
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
    CIFStatic* m_head3;
    CIFStatic* m_head4;
public:
    CIFUniqueLogSlot** m_slots;
    CIFSpinButton* m_spin;
    int totalCount;
};