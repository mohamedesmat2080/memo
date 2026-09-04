#include "IFPartyInfoSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFPartyInfo : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFPartyInfo)
GFX_DECLARE_MESSAGE_MAP(CIFPartyInfo)

private:
    CIFPartyInfo(void);
    ~CIFPartyInfo(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
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
    CIFStatic* m_head5;
public:
    CIFPartyInfoSlot** m_slots;
    int invSlot;
};