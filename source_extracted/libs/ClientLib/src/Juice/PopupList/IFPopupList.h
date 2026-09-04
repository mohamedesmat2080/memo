#include "IFMainFrame.h"
#include "IFTextBox.h"
#include "IFVerticalScroll.h"
#include "IFEdit.h"


class CIFPopupList : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFPopupList)
GFX_DECLARE_MESSAGE_MAP(CIFPopupList)

public:
    CIFPopupList(void);
    ~CIFPopupList(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
private:
void OnSelect(int ,int);
void OnShowList();
private:
    CIFStatic* m_bg;
    CIFButton* m_btn;
public:
    CIFStatic* m_text;
    CIFTextBox* m_list;
    CIFStatic* m_listbg;
};

