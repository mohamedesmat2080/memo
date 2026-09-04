#include <BSLib/_internal/_BSDynamic.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFUniqueLogSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFUniqueLogSlot)
public:
    CIFUniqueLogSlot(void);
    ~CIFUniqueLogSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
    CIFStatic* m_uniquetext;
    CIFStatic* m_killertext;
    CIFStatic* m_statetext;
    CIFStatic* m_datetext;
};