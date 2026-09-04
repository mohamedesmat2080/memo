#include "IFTitleManagerSlot.h"
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFSelectableArea.h>
#include <IFStretchWnd.h>
#include <IFTextBox.h>
#include <IFVerticalScroll.h>
#include <Juice/SpinButton/IFSpinButton.h>

class CIFTitleManager : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFTitleManager)
GFX_DECLARE_MESSAGE_MAP(CIFTitleManager)

private:
    CIFTitleManager(void);
    ~CIFTitleManager(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void OnSelect(int a1 , int a2);
public:
    void MoveToCenter();
    void ClearEnabledStates();
    void OnSet();
    void OnRemove();
    void OnTab();
    void ShowTitleWnd();
    void ShowAchievementWnd();
private:
    CIFFrame* m_frame;
    CIFFrame* m_frame2;
    CIFFrame* m_frame3;
    CIFNormalTile* m_tile;
    CIFNormalTile* m_tile2;
    CIFNormalTile* m_tile3;
    CIFNormalTile* m_tile4;
    CIFStatic* m_head;
    CIFStatic* m_head2;
    CIFStatic* m_text1;
    CIFButton* m_buynewBtn;
    CIFStatic* m_head3;
    CIFStatic* m_head4;
public:
    CIFSelectableArea** m_Tabs;
    CIFVerticalScroll* m_vericalSroll;
    CIFVerticalScroll* m_vericalSroll2;
    CIFTitleManagerSlot** m_slots;
    CIFButton* m_setBtn;
    CIFButton* m_removeBtn;
    CIFTextBox* m_textbox;
    CIFTextBox* m_textbox2;
    CIFStatic* m_text2;
    std::n_wstring selectedTitle;
    struct acheivInfo{
        std::n_wstring Descr;
        bool isEnabled;
    };
    std::map<std::n_wstring, acheivInfo> achievements;
};