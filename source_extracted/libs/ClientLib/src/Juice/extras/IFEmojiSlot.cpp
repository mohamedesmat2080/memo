#include "IFEmojiSlot.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <GInterface.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFEmojiSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFEmojiSlot, CIFWnd)
GFX_END_MESSAGE_MAP()
CIFEmojiSlot::CIFEmojiSlot(void)
{
}
CIFEmojiSlot::~CIFEmojiSlot(void)
{
}
bool CIFEmojiSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);

    RECT m_emojisRect = {0,0,1,1};
    m_emojis = new CIFStatic*[5]; //number of emojis

    for(int i=0;i<5;i++)
        m_emojis[i] = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_emojisRect, 0+i, 0);

    ShowGWnd(false);
    return true;
}
void CIFEmojiSlot::OnUpdate() {
}