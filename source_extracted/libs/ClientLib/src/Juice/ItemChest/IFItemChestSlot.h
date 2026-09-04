#pragma once

#include <BSLib/_internal/_BSDynamic.h>
#include <IFButton.h>
#include <IFSlotWithHelp.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFItemChestSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFItemChestSlot)
GFX_DECLARE_MESSAGE_MAP(CIFItemChestSlot)

public:
    CIFItemChestSlot(void);
    ~CIFItemChestSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void FillTheSlot(int itemIDint,int itemCount);
    void OnTake();
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
    CIFSlotWithHelp* m_itemInfo;
    CIFStatic* m_itemText;
    CIFStatic* m_fromText;
    CIFStatic* m_countText;
    CIFStatic* m_dateText;
    CIFButton* m_takeBtn;
};