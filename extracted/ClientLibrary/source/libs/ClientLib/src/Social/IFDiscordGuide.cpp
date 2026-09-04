#include <GInterface.h>
#include <GEffSoundBody.h>
#include <TextStringManager.h>
#include "IFDiscordGuide.h"
#include "IFSocial.h"

GFX_IMPLEMENT_DYNCREATE(CIFDiscordGuide, CIFDecoratedStatic)


bool CIFDiscordGuide::OnCreate(long ln)
{
    CIFDecoratedStatic::OnCreate(ln);

    TB_Func_13("clientlibrary\\guides\\discord1.ddj", 0, 0);
    sub_634470("clientlibrary\\guides\\discord2.ddj");

    // TB_Func_13("interface\\vlib\\location_1.ddj", 0, 0);
    //  sub_634470("interface\\vlib\\location_2.ddj");

    set_N00009BD4(2);
    set_N00009BD3(500);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifsimple.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    RECT rect = { 0,0,40,40 };
    namelabel = this->m_IRM.GetResObj<CIFStatic>(1, 0);
    std::n_wstring msg(L"Discord Shortcut");
    namelabel->SetTooltip(msg);
    namelabel->SetStyleThingy(TOOLTIP);


    return true;
}

int CIFDiscordGuide::OnMouseLeftUp(int a1, int x, int y)
{
    if(g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->SocialID == 1 && g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->SocialID = 1;
        g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFSocial>(SocialWndID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }

    return 0;
}

void CIFDiscordGuide::OnCIFReady()
{
    CIFDecoratedStatic::OnCIFReady();
    sub_633990();
}

void CIFDiscordGuide::OnUpdate()
{

}

