#pragma once

#include <BSLib/_internal/_BSDynamic.h>
#include <IFButton.h>
#include <IFSlotWithHelp.h>
#include <IFStatic.h>
#include <IFStretchWnd.h>
#include <IFWnd.h>
class CIFEmojiSlot : public CIFWnd
{
GFX_DECLARE_DYNCREATE(CIFEmojiSlot)
GFX_DECLARE_MESSAGE_MAP(CIFEmojiSlot)

public:
    CIFEmojiSlot(void);
    ~CIFEmojiSlot(void);
    bool OnCreate(long ln) override;
    void OnUpdate() override;
public:
    CIFStatic** m_emojis;
};