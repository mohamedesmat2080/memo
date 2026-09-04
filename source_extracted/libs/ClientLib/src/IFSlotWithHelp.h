#pragma once

#include "IFWnd.h"
#include "SOItem.h"

class CIFSlotWithHelp : public CIFWnd
{
    GFX_DECLARE_DYNAMIC_EXISTING(CIFSlotWithHelp, 0x00ee9B18)
public:
    void sub_686C50(DWORD32 a2) const;
    void sub_686DB0() const;
    void sub_66B460(DWORD32 a2) const;
    void sub_67CFB0() const;
    float sub_6871d0(int a2) const;
    void SetType(int Value);
    int GetType();
    char sub_66B460(int a2) const;
public:
    int Type;//36C
    char pad_0x0370[0x20];//370
    CSOItem* ItemInfo;
    char pad_0x036C[0x35C-4-0x28];
}; //Size=0x06C8