#pragma once
#include <IFCheckBox.h>
#include <IFEdit.h>
#include <IFMainFrame.h>
#include <IFNormalTile.h>
#include <IFStretchWnd.h>
#include <Juice/SpinButton/IFSpinButton.h>
class CIFAccLock : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFAccLock)
GFX_DECLARE_MESSAGE_MAP(CIFAccLock)

private:
    CIFAccLock(void);
    ~CIFAccLock(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    void MoveToCenter();
    void OnBtnsClick();
    void OnFocus();
    void SwapButtons(CIFButton* btn1, CIFButton* btn2);
    void changepwMode();
    void createpwMode();
    void enterpwMode();
private:
    CIFNormalTile* m_tile;
    CIFButton** m_btns;
    CIFButton* m_c;
    CIFButton* m_x;
    CIFFrame* m_frame2;
    CIFNormalTile* m_tile2;
    CIFFrame* m_frame3;
    CIFNormalTile* m_tile3;
    CIFFrame* m_frame4;
    CIFNormalTile* m_tile4;
    CIFEdit* m_edit1;
    CIFEdit* m_edit2;
    CIFEdit* m_edit3;
    CIFStatic* m_info;
    CIFButton* m_btn1;
    CIFButton* m_btn2;
    CIFButton* m_btn3;
    CIFCheckBox* m_cbox;
    CIFStatic* m_cboxtext;
    std::n_wstring text_holder1;
    std::n_wstring text_holder2;
    std::n_wstring text_holder3;
};