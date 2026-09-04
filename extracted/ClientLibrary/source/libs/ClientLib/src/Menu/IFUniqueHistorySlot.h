#pragma once
#include <IFStatic.h>
#include <IFWnd.h>
#include <IFBarWnd.h>

class CIFUniqueHistorySlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFUniqueHistorySlot)
GFX_DECLARE_MESSAGE_MAP(CIFUniqueHistorySlot)
public:
    CIFUniqueHistorySlot(void);
    ~CIFUniqueHistorySlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    int OnMouseLeftUp(int a1, int x, int y) override;
    void SetName(int Num, const wchar_t* uniquename, byte state, long time, const wchar_t* killer, int RegionID, int KilledX, int KilledY, int KilledZ);
    void ClearDDJ();
    void Clear();
public:
    unsigned __int64 times;
    int KilledRegID;
    int X;
    int Z;
    int Y;

};