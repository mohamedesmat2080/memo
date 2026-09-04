//
// Created by YUMBUL on 24.08.2024.
//

#include <CustomData/CustomCICPlayer.h>
#include "IFGhachaSelectWnd.h"
#include "IFCheckBox.h"

#define START_GAME_BUTTON 10

#define Title 602
bool CIFGhachaSelectWnd::OnCreateIMPL(long ln)
{
    bool aa = reinterpret_cast<bool(__thiscall *)(CIFGhachaSelectWnd *, long)>(0x0074a7c0)(this, ln);
    wnd_size size = this->GetSize();
    this->SetGWndSize(size.width + 200, size.height);

    wnd_size titlesize = this->m_IRM.GetResObj(602, 1)->GetSize();

    this->m_IRM.GetResObj(602, 1)->SetGWndSize(titlesize.width + 200, titlesize.height);

    this->m_IRM.GetResObj<CIFCheckBox>(701, 1)->SetCheckBoxState(false);
    this->m_IRM.GetResObj<CIFCheckBox>(702, 1)->SetCheckBoxState(false);
    m_Player->m_MagicPopSettings = 700;
    m_Player->m_MagicPopTimerRunning = false;
    this->m_IRM.GetResObj(705, 1)->SetText(L"Auto play and stop when win");
    this->m_IRM.GetResObj(704, 1)->SetText(L"Auto refill magic pop slot");
    this->m_IRM.GetResObj(703, 1)->SetText(L"None");



    return aa;
}

GFX_MSGMAP* CIFGhachaSelectWnd::MessageMap(){
    static const GFX_MSGMAP_ENTRY skillBoardMessageEntries[] =
            {
                    /* {GFX_WM_COMMAND, 0, 14, 14, BSSig_u12, 0,
                             (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnBtnClick))},
 */
                    {GFX_WM_COMMAND, 0, 700, 700, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFGhachaSelectWnd::ClickNone))},
                    {GFX_WM_COMMAND, 0, 701, 701, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFGhachaSelectWnd::ClickAutoRefill))},
                    {GFX_WM_COMMAND, 0, 702, 702, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFGhachaSelectWnd::ClickAutoPlay))},
            };

    static GFX_MSGMAP newmap =
            {
                    reinterpret_cast<const GFX_MSGMAP *>(0x00db4b20), skillBoardMessageEntries,
            };
    return &newmap;
}
void CIFGhachaSelectWnd::ClickNone()
{
    m_Player->m_MagicPopSettings = 700;
    this->m_IRM.GetResObj<CIFCheckBox>(700, 1)->SetCheckBoxState(true);
    this->m_IRM.GetResObj<CIFCheckBox>(701, 1)->SetCheckBoxState(false);
    this->m_IRM.GetResObj<CIFCheckBox>(702, 1)->SetCheckBoxState(false);
}
void CIFGhachaSelectWnd::ClickAutoRefill()
{
    m_Player->m_MagicPopSettings = 701;
    this->m_IRM.GetResObj<CIFCheckBox>(700, 1)->SetCheckBoxState(false);
    this->m_IRM.GetResObj<CIFCheckBox>(701, 1)->SetCheckBoxState(true);
    this->m_IRM.GetResObj<CIFCheckBox>(702, 1)->SetCheckBoxState(false);
}
void CIFGhachaSelectWnd::ClickAutoPlay()
{
    m_Player->m_MagicPopSettings = 702;
    this->m_IRM.GetResObj<CIFCheckBox>(700, 1)->SetCheckBoxState(false);
    this->m_IRM.GetResObj<CIFCheckBox>(701, 1)->SetCheckBoxState(false);
    this->m_IRM.GetResObj<CIFCheckBox>(702, 1)->SetCheckBoxState(true);
}