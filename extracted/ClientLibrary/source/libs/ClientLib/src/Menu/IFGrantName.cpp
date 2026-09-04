#include <Game.h>
#include <ICPlayer.h>
#include <GInterface.h>
#include <TextStringManager.h>
#include <BSLib/multibyte.h>
#include <GlobalDataManager.h>
#include <Macro/IFMacroMenuAutoHunt.h>
#include <EntityManagerClient.h>
#include <ICMonster.h>
#include <NavMesh/LocationInfo.h>
#include <NavMesh/IRegionManager.h>
#include <Macro/IFMacroMenu.h>
#include <sstream>
#include <ExtraUI/IFCounterWnd.h>
#include "IFGrantName.h"



#define GDR_GRANTNAME_BTN_APPLY 9
#define GDR_GRANTNAME_EDIT 7
#define GDR_GRANTNAME_BG3 52
#define GDR_GRANTNAME_BG2 51
#define GDR_GRANTNAME_BG1 50
#define GDR_GRANTNAME_LABEL 53


GFX_IMPLEMENT_DYNCREATE(CIFGrantName, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFGrantName, CIFMainFrame)
                    ONG_COMMAND(GDR_GRANTNAME_BTN_APPLY, &On_BtnClick)
                    ONG_COMMAND(1000, &On_BtnClick)

GFX_END_MESSAGE_MAP()

CIFGrantName::CIFGrantName(void){
}
CIFGrantName::~CIFGrantName(void){

}

bool CIFGrantName::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifgrantname.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    this->SetText(L"Change the Grant Name");

    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->JustifyHorizontal(JUSTIFY_CENTER);
    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->JustifyVertical(JUSTIFY_BOTTOM);

    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetTooltip(L"Enter the grant name");
    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetStyleThingy(TOOLTIP);


    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->AddValue_404(4);
    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetValue_404(2);
    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetMaxLength(12);
    wnd_size t =  m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->GetSize();

    this->m_IRM.GetResObj(9, 1)->SetText(L"Confirm");
    this->m_IRM.GetResObj(8, 1)->SetText(L"Grant name must consist 2~12 English letters.");

    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetTextmode(t.width);

  /*  RECT m_pSlotRect = { this->GetPos().x , this->GetPos().y, 32, 32 };
    CIFSlotWithHelp*m_pMySlot = (CIFSlotWithHelp*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSlotWithHelp), m_pSlotRect, this->UniqueID()+70, 0);

    m_pMySlot->SetSlot(700);
    m_pMySlot->SetSlotType(0xC);*/
    UpdateMenuSize();
    this->ShowGWnd(false);
    return true;
}

void CIFGrantName::UpdateMenuSize()
{

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}


void CIFGrantName::OnUpdate()
{

}
void CIFGrantName::Clear()
{
    m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetText(L"");
}

#define DAT_00eedf08 (*(void **) 0x00eedf08)        /// charID ?
int getDecimalFromLast4HexDigits(int value) {
    std::stringstream ss;
    ss << std::hex << value;
    std::string hexStr = ss.str();

    // Son 4 basamağı al
    std::string last4HexDigits = hexStr.substr(hexStr.length() - 4);

    // Tekrar decimal formata çevir
    int decimalValue;
    std::stringstream ss2;
    ss2 << std::hex << last4HexDigits;
    ss2 >> decimalValue;

    return decimalValue;
}

void CIFGrantName::On_BtnClick() {

  //  g_sNewInterfaceMgr.InstantiateDimensional("clientlibrary\\nifsafetydevice.2dt", this, false);
  //  g_sNewInterfaceMgr.InstantiateDimensional("clientlibrary\\auto_login.2dt", this, false);

 /*   g_pCGInterface->m_IRM.GetResObj<CIFTimerWnd>(410, 1)->ShowGWnd(true);
    g_pCGInterface->m_IRM.GetResObj<CIFTimerWnd>(410, 1)->SetTimer(40, 0);
    g_pCGInterface->m_IRM.GetResObj<CIFTimerWnd>(410, 1)->StartTimerWnd2();
    g_pCGInterface->m_IRM.GetResObj<CIFCounterWnd>(CounterWndNew, 1)->UpdateMenuSize();
*/

    std::n_wstring guildname2 = g_pMyPlayerObj->GetGuildName().c_str();
    if (guildname2.empty()) {
        g_pCGInterface->ShowMessage_Warning(L"You must enter the guild.");
        return;
    }
    if ( m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->GetNText().length() >= 2) {
        CMsgStreamBuffer buf(0x207B);
        std::n_string grant = TO_NSTRING(this-> m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->GetNText());
        buf << grant;
        SendMsg(buf);
        m_IRM.GetResObj<CIFEdit>(GDR_GRANTNAME_EDIT, 1)->SetText(L"");

    } else {
        g_pCGInterface->ShowMessage_Warning(L"Grant name must be consist min 2 characters.");
        return;
    }

}
