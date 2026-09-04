#include <BSLib/_internal/_BSDynamic.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFEventScheduleSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFEventScheduleSlot)
public:
    CIFEventScheduleSlot(void);
    ~CIFEventScheduleSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
    CIFStatic* m_eventtext;
    CIFStatic* m_datetext;
};