#include "IFDpsSlot.h"
#include "IFEdit.h"
#include "IFMainFrame.h"
#include "IFTextBox.h"
#include "IFVerticalScroll.h"


class CIFDps : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFDps)
GFX_DECLARE_MESSAGE_MAP(CIFDps)

public:
    CIFDps(void);
    ~CIFDps(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void OnToggle();
    void ClearRecords();
private:
    CIFStatic* m_wnd;
    CIFStatic* m_icon;
    CIFButton* m_toggle;
    std::list<CIFWnd*> getControls;
public:
    CIFDpsSlot** m_slots;
    CIFStatic* m_title;
};

