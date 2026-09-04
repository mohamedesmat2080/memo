#include "IFMenu.h"
#include "IFGrantName.h"
#include "IFTitleManager.h"
#include "IFIconManager.h"
#include "IFDynamicRanking.h"
#include "IFUniqueHistory.h"
#include "IFEventRegister.h"
#include "IFEventSchedule.h"
#include "IFChangelog.h"
#include "IFAchievements.h"
#include <ctime>
#include <BSLib/Debug.h>
#include <Game.h>
#include <ICPlayer.h>
#include <GInterface.h>
#include <CustomData/CustomSettingManager.h>
#include <MacroAlchemy/IFAlchemyMacro.h>
#include <SRIFLib/NIFEnchantWnd.h>
#include <CharacterDependentData.h>
#include <ExtraUI/IFSettings.h>


#define GDR_MENU_BTN_GRANTNAME 12
#define GDR_MENU_BTN_TITLEMGR 13
#define GDR_MENU_BTN_ICONMGR 14
#define GDR_MENU_BTN_DYNAMICRANKING 15
#define GDR_MENU_BTN_UNIQUEHISTORY 16
#define GDR_MENU_BTN_EVENT_REGISTER 17
#define GDR_MENU_BTN_EVENT_SCHEDULE 18
#define GDR_MENU_BTN_ACHIEVEMENTS 19
#define GDR_MENU_BTN_CHANGELOG 20
#define GDR_MENU_BTN_ALCHEMY_MACRO 21
#define GDR_DISCORD 24
#define GDR_WEBSITE 25
#define GDR_SETTINGS 22
GFX_IMPLEMENT_DYNCREATE(CIFMenu, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFMenu, CIFMainFrame)
                    ONG_COMMAND(GDR_MENU_BTN_GRANTNAME, &On_BtnClickGrantName)
                    ONG_COMMAND(GDR_MENU_BTN_TITLEMGR, &On_BtnClickTitleManager)
                    ONG_COMMAND(GDR_MENU_BTN_ICONMGR, &On_BtnClickIconManager)
                    ONG_COMMAND(GDR_MENU_BTN_DYNAMICRANKING, &On_BtnClickRank)
                    ONG_COMMAND(GDR_MENU_BTN_UNIQUEHISTORY, &On_BtnClickUniqueLog)
                    ONG_COMMAND(GDR_MENU_BTN_EVENT_REGISTER, &On_BtnClickEventRegister)
                    ONG_COMMAND(GDR_MENU_BTN_EVENT_SCHEDULE, &On_BtnClickEventTimer)
                    ONG_COMMAND(GDR_MENU_BTN_ACHIEVEMENTS, &On_BtnClickAchievements)
                    ONG_COMMAND(GDR_MENU_BTN_CHANGELOG, &On_BtnClickChangelog)
                    ONG_COMMAND(GDR_MENU_BTN_ALCHEMY_MACRO, &On_BtnClickAlchemyMacro)
                    ONG_COMMAND(GDR_SETTINGS, &On_BtnSettings)
	ONG_CREATE()
	ONG_WM_4002()
	ONG_WM_4003()
	ONG_VISIBLE_CHANGE()
	ONG_WM_4005()
	ONG_MOVE()
GFX_END_MESSAGE_MAP()

CIFMenu::CIFMenu(void)
{
   /* PingRegionID = 0;
    PingPosX = 0;
    PingPosY = 0;
    PingPosZ = 0;
    Seconds = 0;*/
   
    CanSendPing = false;
	BS_DEBUG("> " __FUNCTION__);
}

CIFMenu::~CIFMenu(void)
{
	BS_DEBUG("> " __FUNCTION__);
}

bool CIFMenu::OnCreate(long ln)
{
	BS_DEBUG("> " __FUNCTION__ "(%d)", ln);

	// Populate inherited members
	CIFMainFrame::OnCreate(ln);

	m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifmenu.txt");
	m_IRM.CreateInterfaceSection("Create", this);

    this->SetText(L"Menu");

   /* m_IRM.GetResObj(7, 1)->SetStyleThingy(TOOLTIP);
    m_IRM.GetResObj(7, 1)->SetTooltip(L"Race");

    m_IRM.GetResObj(10, 1)->SetStyleThingy(TOOLTIP);
    m_IRM.GetResObj(10, 1)->SetTooltip(L"Guild");
*/


    m_IRM.GetResObj(GDR_MENU_BTN_GRANTNAME, 1)->SetText(L"Grant Name");
    m_IRM.GetResObj(GDR_MENU_BTN_TITLEMGR, 1)->SetText(L"Title Manager");
    m_IRM.GetResObj(GDR_MENU_BTN_ICONMGR, 1)->SetText(L"Icon Manager");
    m_IRM.GetResObj(GDR_MENU_BTN_DYNAMICRANKING, 1)->SetText(L"Dynamic Ranking");
    m_IRM.GetResObj(GDR_MENU_BTN_UNIQUEHISTORY, 1)->SetText(L"Unique History");
    m_IRM.GetResObj(GDR_MENU_BTN_EVENT_REGISTER, 1)->SetText(L"Event Register");
    m_IRM.GetResObj(GDR_MENU_BTN_EVENT_SCHEDULE, 1)->SetText(L"Event Schedule");
    m_IRM.GetResObj(GDR_MENU_BTN_ACHIEVEMENTS, 1)->SetText(L"Achievements");
    m_IRM.GetResObj(GDR_MENU_BTN_CHANGELOG, 1)->SetText(L"Changelog");
    m_IRM.GetResObj(GDR_MENU_BTN_ALCHEMY_MACRO, 1)->SetText(L"Alchemy Macro");

    m_IRM.GetResObj(GDR_SETTINGS, 1)->SetText(L"Settings");


    if(m_Settings->GrantName)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_GRANTNAME, 1)->SetEnabledState(true);
    }
    if(m_Settings->TitleManager)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_TITLEMGR, 1)->SetEnabledState(true);
    }
    if(m_Settings->IconManager)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_ICONMGR, 1)->SetEnabledState(true);
    }
    if(m_Settings->RankingWnd)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_DYNAMICRANKING, 1)->SetEnabledState(true);
    }
    if(m_Settings->UniqueHistoryWnd)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_UNIQUEHISTORY, 1)->SetEnabledState(true);
    }
    if(m_Settings->EventRegisterWnd)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_EVENT_REGISTER, 1)->SetEnabledState(true);
    }
    if(m_Settings->EventScheduleWnd)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_EVENT_SCHEDULE, 1)->SetEnabledState(true);
    }
    if(m_Settings->AchievementsWnd)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_ACHIEVEMENTS, 1)->SetEnabledState(true);
    }
    if(m_Settings->EnableChangeLog)
    {
        this->m_IRM.GetResObj<CIFButton>(GDR_MENU_BTN_CHANGELOG, 1)->SetEnabledState(true);
    }

    this->UpdateMenuSize();
    this->ShowGWnd(false);


	return true;
}

void CIFMenu::UpdateMenuSize()
{
    const ClientResolutonData &res = CGame::GetClientDimensionStuff();

    MoveGWnd(((res.width - GetSize().width) - 100), GetPos().y);
    this->BringToFront();

}
void CIFMenu::OnUpdate()
{

}
void CIFMenu::SetCharFace()
{
    if(g_pMyPlayerObj == NULL)
        return;
    m_IRM.GetResObj(6, 1)->SetText(g_pMyPlayerObj->GetCharName().c_str());

    std::wstring GuildName = g_pMyPlayerObj->GetGuildName().c_str();
    if(GuildName.empty())
    {
        m_IRM.GetResObj(9, 1)->SetText(L"<No Guild>");
    }
    else
    {
        m_IRM.GetResObj(9, 1)->SetText(GuildName.c_str());
    }

    int data = g_pMyPlayerObj->GetCommonData()->RefObjectId;
    switch(data)
    {
        case 1907: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man1.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);

        }
            break;
        case 1908: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man2.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1909: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man3.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1910: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man4.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1911: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man5.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1912: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man6.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1913: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man7.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1914: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man8.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1915: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man9.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1916: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man10.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1917: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man11.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1918: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man12.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1919: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_man13.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1920: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman1.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1921: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman2.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1922: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman3.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1923: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman4.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1924: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman5.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1925: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman6.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1926: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman7.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1927: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman8.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1928: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman9.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1929: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman10.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1930: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman11.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1931: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman12.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);
        }
            break;
        case 1932: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_ch_woman13.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_china16.ddj", 0,
                                              0);

        }
            break;

        case 14875: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man1.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14876: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man2.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14877: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man3.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14878: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man4.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);

        }
            break;
        case 14879: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man5.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14880: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man6.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);

        }
            break;
        case 14881: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man7.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14882: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man8.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14883: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man9.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14884: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man10.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);

        }
            break;
        case 14885: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man11.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14886: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man12.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14887: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_man13.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;

        case 14888: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman1.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14889: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman2.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14890: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman3.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14891: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman4.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14892: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman5.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14893: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman6.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14894: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman7.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14895: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman8.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14896: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman9.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14897: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman10.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14898: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman11.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);
        }
            break;
        case 14899: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman12.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);

        }
            break;
        case 14900: {
            m_IRM.GetResObj(4, 1)->TB_Func_13("interface\\character\\char_eu_woman13.ddj", 0, 0);
            m_IRM.GetResObj(7, 1)->TB_Func_13("interface\\ifcommon\\ifcommon\\com_kindred_europe16.ddj", 0,
                                              0);

        }
            break;
    }
}

int CIFMenu::OnCreatedInstance(UINT a1, UINT a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
	return 0;
}

int CIFMenu::On4002(int a1, int a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
	return 0;
}

int CIFMenu::On4003(int a1, int a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
	return 0;
}

int CIFMenu::OnVisibleStateChange(int newstate, int a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", newstate, a2);
	return 0;
}

int CIFMenu::On4005(int a1, int a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
	return 0;
}

int CIFMenu::OnWindowPosChanged(UINT a1, UINT a2)
{
	BS_DEBUG("> " __FUNCTION__ " ( %p, %p )", a1, a2);
	return 0;
}
void CIFMenu::On_BtnClickGrantName(){
    BS_DEBUG("> " __FUNCTION__);
    if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }

}
void CIFMenu::On_BtnClickTitleManager(){
    BS_DEBUG("> " __FUNCTION__);
    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ClearDDJ();
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x0);
        SendMsg(buf);
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ActivateTabPage(0);
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");

    }


}
void CIFMenu::On_BtnClickIconManager(){
    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ClearDDJ();
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x1);
        SendMsg(buf);
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->LoadItems();
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }

}
void CIFMenu::On_BtnClickRank()
{
    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->m_popup->m_text->SetText(L"");
        CMsgStreamBuffer buf(0x180A);
        byte type = 0;
        buf << type;
        SendMsg(buf);
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->Hide();
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");

    }
}

void CIFMenu::On_BtnClickUniqueLog(){
    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->Clear();
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x2);
        SendMsg(buf);


        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->UpdateRanks();
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");

    }
}
void CIFMenu::On_BtnClickEventRegister(){

    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ClearDDJ();
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x3);
        SendMsg(buf);


        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->UpdateEvents();
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");

    }


}
void CIFMenu::On_BtnClickEventTimer(){
    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->Clear();
        CMsgStreamBuffer buf(0x169A);
        buf << BYTE(0x4);
        SendMsg(buf);


        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->UpdateList();
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");

    }
}
void CIFMenu::On_BtnClickAchievements(){

    if (g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if(g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {

        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->Clear();
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ClearDDJ();
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ActivateTabPage(0);
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }
}
void CIFMenu::On_BtnClickChangelog(){
    if (g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFTitleManager>(TitleManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFIconManager>(IconManagerID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFDynamicRanking>(DynamicRankingID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventRegister>(EventRegisterID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFEventSchedule>(EventScheduleID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if (g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else if(g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    if (g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(false);
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFChangelog>(ChangelogID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }
}
void CIFMenu::On_BtnClickAlchemyMacro(){
    if(g_pCGInterface->m_IRM.GetResObj<CIFAlchemyMacro>(AlchemyMacro, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAlchemyMacro>(AlchemyMacro, 1)->OnCloseWnd();
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
    else
    {
        if(g_pCGInterface->GetGuiFromList<CNIFEnchantWnd>(168) != NULL)
        {
            return;
        }
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
        g_pCGInterface->m_IRM.GetResObj<CIFAlchemyMacro>(AlchemyMacro, 1)->ShowGWnd(true);
        g_pCGInterface->GetMainPopup()->ShowGWnd(true);
        g_pCGInterface->GetMainPopup()->ShowSubPage(GDR_INVENTORY);
        g_pCGInterface->m_IRM.GetResObj<CIFAlchemyMacro>(AlchemyMacro, 1)->UpdateMenuSize();
        g_pCGInterface->LockMovement(13);
    }
}
void CIFMenu::On_BtnSettings()
{
    if(!g_pCGInterface->m_IRM.GetResObj<CIFSettings>(SettingsWndID, 1)->IsVisible())
    {
        g_pCGInterface->m_IRM.GetResObj<CIFSettings>(SettingsWndID, 1)->UpdateMenuSize();
        g_pCGInterface->m_IRM.GetResObj<CIFSettings>(SettingsWndID, 1)->ShowGWnd(true);
        CGEffSoundBody::get()->PlaySound(L"snd_window_open");
    }
    else
    {
        g_pCGInterface->m_IRM.GetResObj<CIFSettings>(SettingsWndID, 1)->OnCloseWnd();
        CGEffSoundBody::get()->PlaySound(L"snd_window_close");
    }
}