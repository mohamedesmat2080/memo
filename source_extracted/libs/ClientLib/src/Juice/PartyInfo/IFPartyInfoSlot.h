#pragma once

#include <BSLib/_internal/_BSDynamic.h>
#include <IFButton.h>
#include <IFSlotWithHelp.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFPartyInfoSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFPartyInfoSlot)
GFX_DECLARE_MESSAGE_MAP(CIFPartyInfoSlot)

public:
    CIFPartyInfoSlot(void);
    ~CIFPartyInfoSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
    void OnMove();
private:
    CIFStatic* r_edge;
    CIFStatic* l_edge;
public:
    CIFStatic* m_playername;
    CIFStatic* m_guildname;
    CIFStatic* m_status;
    CIFStatic* m_region;
    CIFButton* m_movebtn;
    std::n_wstring playername;
};