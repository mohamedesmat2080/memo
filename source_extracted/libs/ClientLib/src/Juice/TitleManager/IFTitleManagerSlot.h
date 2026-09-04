#include <BSLib/_internal/_BSDynamic.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFTitleManagerSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFTitleManagerSlot)
public:
    CIFTitleManagerSlot(void);
    ~CIFTitleManagerSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void SetEnableState(bool state);
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
};