#include <CustomData/CustomSettingManager.h>
#include "AlramGuideMgrWnd.h"

#include "IFConfirmReputationGuide.h"
#include "IFEventGuide.h"
#include "IFQuestInfoGuide.h"
#include "IFLetterAlarmGuide.h"
#include "IFServerEventGuide.h"
#include "IFOpenMarketAlramGuide.h"
#include "IFEventGuideSecond.h"
#include "IFMenuGuide.h"

#include "GInterface.h"
#include "Game.h"
#include "IFMagicStateBoard.h"
#include "IFStateSlot.h"
#include "Guides/IFChestGuide.h"
#include "Social/IFFacebookGuide.h"
#include "Social/IFDiscordGuide.h"
#include "Web/IFWebGuide.h"
#include <DailyLogin/IFDailyLoginGuide.h>
#include <MacroAlchemy/IFAlchemyMacroGuide.h>
#include "Macro/IFMacroGuide.h"

GFX_IMPLEMENT_DYNAMIC_EXISTING(CAlramGuideMgrWnd, 0x00ee99a8)

#define ALRAM_GUIDE_ICON_SIZE 42
#define ALRAM_GUIDE_MAX_ICONS_LINE 4

CAlramGuideMgrWnd::CAlramGuideMgrWnd() {
    m_btGuidesCount = 0;
}

void CAlramGuideMgrWnd::PrepareGuides() {
    wnd_pos posMgr = GetPos();

    listGUIDES::iterator it = m_listGuides.begin();
    for (DWORD dwPosIndex = 1; it != m_listGuides.end(); ++it, dwPosIndex++) {
        posMgr.x -= ALRAM_GUIDE_ICON_SIZE;
        CIFWnd *pGuideIcon = (*it);

        pGuideIcon->MoveGWnd(posMgr.x, posMgr.y);
        pGuideIcon->ShowGWnd(true);

        if (m_btGuidesCount != 0) {
            // if we reached our max icons per line
            if (!(dwPosIndex % ALRAM_GUIDE_MAX_ICONS_LINE)) {
                posMgr.y += (ALRAM_GUIDE_ICON_SIZE + 2);
                posMgr.x = GetPos().x;
            }
        }
    }

    if (m_Settings->MoveSkillBoard) {
        const ClientResolutonData &res = CGame::GetClientDimensionStuff();
        CIFWnd *playerinfo = g_pCGInterface->m_IRM.GetResObj(11, 1);
        CIFWnd *wind = g_pCGInterface->m_IRM.GetResObj(22, 1);

        /// x 220 y 10
        wind->MoveGWnd(((playerinfo->GetPos().x + playerinfo->GetSize().width) + 27), wind->GetPos().y);
    }

}

void CAlramGuideMgrWnd::RemoveAllGuides() {
    listGUIDES::iterator it = m_listGuides.begin();
    for (; it != m_listGuides.end(); ++it)
        (*it)->EraseWindowObj();
}

bool CAlramGuideMgrWnd::IsAvailableGuide(DWORD dwID) {
    listGUIDES::iterator it = m_listGuides.begin();
    for (; it != m_listGuides.end(); ++it)
        if ((*it)->UniqueID() == dwID)
            return true;
    return false;
}

CIFWnd *CAlramGuideMgrWnd::GetGuide(DWORD dwID) {
    listGUIDES::iterator it = m_listGuides.begin();
    for (; it != m_listGuides.end(); ++it)
        if ((*it)->UniqueID() == dwID)
            return (*it);
    return NULL;
}

CAlramGuideMgrWnd::~CAlramGuideMgrWnd() {
    RemoveAllGuides();
}

CIFWnd *CAlramGuideMgrWnd::CreateGuideIcon(int nWndID) {
    // Try to find the element in the list
    listGUIDES::iterator it = m_listGuides.begin();
    for (; it != m_listGuides.end(); ++it) {
        if ((*it)->UniqueID() == nWndID) {
            return *it;
        }
    }

    // List did not contain the element, try to create it
    RECT rect = {0,
                 0,
                 ALRAM_GUIDE_ICON_SIZE, 
                 ALRAM_GUIDE_ICON_SIZE};

    CIFWnd* pObj = 0;

    switch(nWndID) {
        case GDR_CONFIRMREPUTATION_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFConfirmReputationGuide), rect, GDR_CONFIRMREPUTATION_GUIDE, 0);
            break;

        case GDR_EVENTGUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFEventGuide), rect, GDR_EVENTGUIDE, 0);
            break;

        case GDR_QUESTINFO_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFQuestInfoGuide), rect, GDR_QUESTINFO_GUIDE, 0);
            break;

        case GDR_LETTER_ALARM_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFLetterAlarmGuide), rect, GDR_LETTER_ALARM_GUIDE, 0);
            break;

        case GDR_SERVEREVENT_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFServerEventGuide), rect, GDR_SERVEREVENT_GUIDE, 0);
            break;

        case GDR_OPENMARKETALRAM_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFOpenMarketAlramGuide), rect, GDR_OPENMARKETALRAM_GUIDE, 0);
            break;

        case GDR_EVENTGUIDE_SECOND:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFEventGuideSecond), rect, GDR_EVENTGUIDE_SECOND, 0);
            break;
        case GDR_CHEST_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFChestGuide), rect, GDR_CHEST_GUIDE, 0);
            break;
        case GDR_MENU_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFMenuGuide), rect, GDR_MENU_GUIDE, 0);
            break;

        case GDR_FACEBOOK_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFFacebookGuide), rect, GDR_FACEBOOK_GUIDE, 0);
            break;

        case GDR_DISCORD_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFDiscordGuide), rect, GDR_DISCORD_GUIDE, 0);
            break;

        case GDR_WEB_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFWebGuide), rect, GDR_WEB_GUIDE, 0);
            break;

        case GDR_DAILY_LOGIN:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFDailyLoginGuide), rect, GDR_DAILY_LOGIN, 0);
            break;

        case GDR_MACRO_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFMacroGuide), rect, GDR_MACRO_GUIDE, 0);
            break;
        case GDR_ALCHEM_MACRO_GUIDE:
            pObj = (CIFWnd *) CreateInstance(this, GFX_RUNTIME_CLASS(CIFAlchemyMacroGuide), rect, GDR_ALCHEM_MACRO_GUIDE, 0);
            break;

        default:
            assert(false);
    }
    m_listGuides.push_back(pObj);
    m_btGuidesCount++;

    PrepareGuides();

    return pObj;
}

void CAlramGuideMgrWnd::RemoveGuide(DWORD dwID) {
    listGUIDES::iterator it = m_listGuides.begin();
    for (; it != m_listGuides.end(); ++it) {
        if ((*it)->UniqueID() != dwID)
            continue;
        (*it)->ShowGWnd(false);
        m_listGuides.erase(it);
        // Hmmm why this is not m_btGuidesCount minus too?
        break;
    }
}
