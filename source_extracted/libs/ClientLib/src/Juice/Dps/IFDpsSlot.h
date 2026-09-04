#include <BSLib/_internal/_BSDynamic.h>
#include <IFButton.h>
#include <IFFrame.h>
#include <IFNormalTile.h>
#include <IFSlotWithHelp.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFDpsSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFDpsSlot)
GFX_DECLARE_MESSAGE_MAP(CIFDpsSlot)
public:
    CIFDpsSlot(void);
    ~CIFDpsSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
private:
    CIFStatic* m_icon;
public:
    CIFStatic* m_playername;
    CIFStatic* m_dmg;

};