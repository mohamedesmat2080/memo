#include "IFItemMall.h"
#include "Game.h"
#include "GInterface.h"

undefined1 CIFItemMall::OnCloseWnd_IMPL() {
    reinterpret_cast<int(__thiscall *)(CIFItemMall *)>(0x007BD530)(this);

    this->m_IRM.GetResObj<CIFButton>(4, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFButton>(6, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFButton>(7, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFButton>(9, 1)->ShowGWnd(true);


    g_pCGInterface->OnItemMallSectionControl(false);

    return 1;
}

bool CIFItemMall::OnCreateIMPL(long ln) {


    bool b = reinterpret_cast<bool (__thiscall *)(CIFItemMall *, long)>(0x007c09e0)(this, ln);

    this->m_IRM.GetResObj<CIFButton>(4, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFButton>(6, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFButton>(7, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFButton>(9, 1)->ShowGWnd(false);


    return b;
}

void CIFItemMall::UpdateMenuSize()
{
    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void CIFItemMall::SetBuyItemCount(int nItemCount){
    m_nBuyItemCount = nItemCount;
}
int CIFItemMall::GetBuyItemCount() const{
    return m_nBuyItemCount;
}
