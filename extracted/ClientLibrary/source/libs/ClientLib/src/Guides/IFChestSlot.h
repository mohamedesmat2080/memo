#pragma once
#include <IFStatic.h>
#include <IFWnd.h>
#include <IFBarWnd.h>

class CIFChestSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFChestSlot)
GFX_DECLARE_MESSAGE_MAP(CIFChestSlot)
public:
    CIFChestSlot(void);
    ~CIFChestSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void SetName(int FakeNum, int DbID, int ItemID, int Quantity, const wchar_t * Date, const wchar_t * Type, byte plus);
    void TakeBtn();
public:
    int DatabaseID;
    void Clear();
};