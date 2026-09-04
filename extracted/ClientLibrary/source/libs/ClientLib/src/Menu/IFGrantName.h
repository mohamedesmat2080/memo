#pragma once
#include "IFMainFrame.h"
#include "IFEdit.h"

class CIFGrantName : public CIFMainFrame
{
GFX_DECLARE_DYNCREATE(CIFGrantName)
GFX_DECLARE_MESSAGE_MAP(CIFGrantName)
private:
    CIFGrantName(void);
    ~CIFGrantName(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void On_BtnClick();
public:
    void UpdateMenuSize();
    void Clear();
};
