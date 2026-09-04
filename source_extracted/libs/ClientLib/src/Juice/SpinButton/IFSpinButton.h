#pragma once

#include <IFButton.h>
#include <IFStatic.h>
class CIFSpinButton : public CIFWnd
{
    //sorry but we going to use this concept one more time last time with chest xD
    GFX_DECLARE_DYNCREATE(CIFSpinButton)
    GFX_DECLARE_MESSAGE_MAP(CIFSpinButton)
public:
    CIFSpinButton(void);
    ~CIFSpinButton(void);

    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void OnLeftBtnClick();
    void OnRightBtnClick();
    void SetOnChangeFun(void* parent, void* fun);
private:
    int On4001(int a1, int a2);
    int On4002(int a1, int a2);
    int On4003(int a1, int a2);
    int OnVisibleStateChange(int a1, int a2);
    int On4005(int a1, int a2);
    int On4006(int a1, int a2);

public:
    int index;
    CIFStatic* numStatic;
private:
    void* OnChangeFun;
    void* Parent;
    CIFButton* leftButton;
    CIFButton* rightButton;
};