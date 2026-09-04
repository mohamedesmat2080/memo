//
// Created by YUMBUL on 18.01.2024.
//

#include "IFSkillBoard.h"
#include "IFCheckBox.h"
#include "GInterface.h"
#include "ICPlayer.h"
#include "GlobalDataManager.h"
#include "TextStringManager.h"
#include <IFSkill.h>
#include <CustomData/CustomCICPlayer.h>
#include <CustomData/CustomSettingManager.h>
/*


GFX_IMPLEMENT_DYNCREATE_FN(CIFSkillBoard, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFSkillBoard, CIFMainFrame)
                    ONG_COMMAND(14, &CIFSkillBoard::OnBtnClick)
                    ONG_COMMAND(1001, &CIFSkillBoard::OnClickAutoSkill)
GFX_END_MESSAGE_MAP()

*/
//GFX_IMPLEMENT_DYNAMIC_EXISTING(CIFSkillBoard, 0x00eea61c)

#define GDR_MAX_MASTERY_CHECKBOX 300
#define GDR_MAX_SKILL_CHECKBOX 301

#define GDR_MAXMASTERY_LABEL 302
#define GDR_MAXSKILL_LABEL 303

CIFSkillBoard::CIFSkillBoard(){
    reinterpret_cast<void*(__thiscall*)(CIFSkillBoard*)>(0x0069bb00)(this);
}

CIFSkillBoard::~CIFSkillBoard(){
    reinterpret_cast<void*(__thiscall*)(CIFSkillBoard*)>(0x0069bff0)(this);
}

GFX_MSGMAP* CIFSkillBoard::MessageMap(){
    static const GFX_MSGMAP_ENTRY skillBoardMessageEntries[] =
            {
                    /* {GFX_WM_COMMAND, 0, 14, 14, BSSig_u12, 0,
                             (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnBtnClick))},
 */
                    {GFX_WM_COMMAND, 0, 300, 300, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnClickAutoMastery))},
                    {GFX_WM_COMMAND, 0, 301, 301, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnClickAutoSkill))},
                    // Diğer özel mesaj girişleri buraya eklenebilir
            };

    static GFX_MSGMAP newmap =
            {
                    reinterpret_cast<const GFX_MSGMAP *>(0x00da5aec), skillBoardMessageEntries,
            };
    return &newmap;
}


bool CIFSkillBoard::OnCreateIMPL(long ln) {
    //printf("%s\n", __FUNCTION__);
    bool a = reinterpret_cast<bool (__thiscall *)(const CIFSkillBoard *, long)>(0x0069c4e0)(this, ln);
    if(m_Settings->EnableAutoSkill)
    {
        m_IRM.GetResObj<CIFStatic>(GDR_MAXMASTERY_LABEL, 1)->SetText(L"Auto Mastery");
        m_IRM.GetResObj<CIFStatic>(GDR_MAXSKILL_LABEL, 1)->SetText(L"Auto Skill");
        this->m_IRM.GetResObj<CIFStatic>(12, 1)->ShowGWnd(false);
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(false);
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(false);

        m_IRM.GetResObj<CIFStatic>(GDR_MAXSKILL_LABEL, 1)->ShowGWnd(true);
        m_IRM.GetResObj<CIFStatic>(GDR_MAXMASTERY_LABEL, 1)->ShowGWnd(true);
    }


    return a;
}


void CIFSkillBoard::OnUpdate() {

    reinterpret_cast<void (__thiscall*)(CIFSkillBoard*)>(0x006528a0)(this);
}
void CIFSkillBoard::OnBtnClick()
{
    reinterpret_cast<void*(__thiscall*)(CIFSkillBoard*)>(0x0069bca0)(this);
}
void CIFSkillBoard::SelectMastery(int p1, int p2) {

    if(m_Settings->EnableAutoSkill)
    {
        if(m_Player->Selected_AutoMasteryId != this->SelectedMasteryID)
        {
            m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(false);
            m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(false);
            m_Player->Selected_AutoMasteryId = 0;
            m_Player->Enabled_AutoMastery = false;
            m_Player->Selected_AutoSkillId = 0;
            m_Player->Enabled_AutoSkill = false;
        }
    }

    reinterpret_cast<void *(__thiscall *) (CIFSkillBoard *, int, int)>(0x0069d7b0)(this, p1, p2);
}

void CIFSkillBoard::OnClickAutoSkill(){
    if(m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        g_pCGInterface->CreateCustomMessageBox(4, this->SelectedMasteryID);
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(false);
    }
    else if(!m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        m_Player->Enabled_AutoSkill = false;
        m_Player->Selected_AutoSkillId = 0;
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(false);
    }
}


void CIFSkillBoard::OnClickAutoMastery(){
    if(m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        g_pCGInterface->CreateCustomMessageBox(3, this->SelectedMasteryID);
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(false);
    }
    else if(!m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        m_Player->Enabled_AutoMastery = false;
        m_Player->Selected_AutoMasteryId = 0;
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(false);
    }
}

#define SKILLTIMER 110078
#define AUTOSKILLTIMER 110079
void CIFSkillBoard::EnableAutoMastery(int MasteryID)
{
    m_Player->Enabled_AutoMastery = true;
    m_Player->Selected_AutoMasteryId = this->SelectedMasteryID;
    m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(true);
    if (g_pMyPlayerObj != 0x0) {
        if (g_pMyPlayerObj->CHARACTER_STATUS != Dead && g_pMyPlayerObj->CHARACTER_STATUS != Stall) {

            int masterylevel = 0;
            for (std::map<int, sMasteryData *>::iterator it = g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_masteryData.begin(); it != g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_masteryData.end(); it++) {
                if (it->second->nMasteryId == MasteryID) {
                    masterylevel = it->second->btLevel;
                    break;
                }
            }
            if (masterylevel >= 0 && masterylevel + 1 < m_Settings->ServerMaxLevel) {
                if (g_pMyPlayerObj->IsChinese()) {
                    if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.GetCurrentMasteryCount() < m_Settings->MaxMasteryLevel) {

                        if (masterylevel < g_pMyPlayerObj->GetCurrentLevel()) {
                            //printf("%d %d %d\n", masterylevel, g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.GetCurrentMasteryCount(), m_Settings->MaxMasteryLevel);

                            unsigned int nReqExp = g_CGlobalDataManager->GetLevelData(masterylevel + 1).m_expM;
                            if (g_pMyPlayerObj->m_nSkillPoint >= nReqExp) {

                                UpdateMastery(MasteryID, SelectedMasteryLevel);

                                return;
                            }
                        }
                    }
                }
                else if (g_pMyPlayerObj->IsEurope()) {
                    if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.GetCurrentMasteryCount() < (m_Settings->ServerMaxLevel * 2)) {
                        if (masterylevel < g_pMyPlayerObj->GetCurrentLevel()) {
                            unsigned int nReqExp = g_CGlobalDataManager->GetLevelData(masterylevel + 1).m_expM;
                            if (g_pMyPlayerObj->m_nSkillPoint >= nReqExp) {

                                UpdateMastery(MasteryID, SelectedMasteryLevel);

                                return;
                            }
                        }
                    }
                }
            }
        }
    }
}


void CIFSkillBoard::EnableAutoSkill(int MasteryID)
{
    if(this->SelectedMasteryLevel != 0)
    {
        m_Player->Enabled_AutoSkill = true;
        m_Player->Selected_AutoSkillId = 0;
        m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(true);

        UpdateAutoSkill(this->SelectedMasteryID);
    }
    else
    {
        g_pCGInterface->ShowMessage_Notice(TSM_GETTEXTPTR(L"VFILTER_MAX_SKILL_ERROR"));
    }
}
void CIFSkillBoard::UpdateAutoSkill(int MasteryID)
{
    if(this-> m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        int SelectedSkill = SelectSkillbyMasteryID(this->SelectedMasteryID);
        //printf("%d \n", SelectedSkill);
        if (SelectedSkill <= 100000 && SelectedSkill > 0) {
            CMsgStreamBuffer buf(0x70A1);
            buf << SelectedSkill;
            SendMsg(buf);
            StartTimer(AUTOSKILLTIMER, 130);
        }
    }
}
int CIFSkillBoard::SelectSkillbyMasteryID(int MasteryID) {

    if(g_pMyPlayerObj)
    {
        for (std::map<unsigned __int32, CSkillData *>::iterator skillIter = g_CGlobalDataManager->m_skillDataMap.begin();
             skillIter != g_CGlobalDataManager->m_skillDataMap.end(); ++skillIter) {

            if (skillIter->second->UI_SkillTab != 255 || skillIter->second->UI_SkillPage != 255) {
                if (skillIter->second->ReqCommon_Mastery1 == MasteryID && skillIter->second->ID != 11828) {

                    if (!g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.SkillIsLearned_MAYBE(skillIter->second->ID) && g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.find(skillIter->second->ID) == g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_SkillData.end()) {

                        if (skillIter->second->ReqCommon_MasteryLevel1 <= g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.m_masteryData[MasteryID]->btLevel && skillIter->second->ReqLearn_SP <= g_pMyPlayerObj->m_nSkillPoint) {
                            if (skillIter->second->ReqLearn_Skill1 > 0 && skillIter->second->ReqLearn_Skill2 > 0) {
                                CSkillData *SkillLearnCondition1 = g_CGlobalDataManager->GetSkillbyGroupIDandBasicLevel(skillIter->second->ReqLearn_Skill1, skillIter->second->ReqLearn_SkillLevel1);
                                CSkillData *SkillLearnCondition2 = g_CGlobalDataManager->GetSkillbyGroupIDandBasicLevel(skillIter->second->ReqLearn_Skill2, skillIter->second->ReqLearn_SkillLevel2);

                                if (SkillLearnCondition1 && SkillLearnCondition2) {
                                    if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.SkillIsLearned_MAYBE(SkillLearnCondition1->ID) && g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.SkillIsLearned_MAYBE(SkillLearnCondition2->ID)) {
                                        return skillIter->second->ID;
                                    }
                                }
                            } else if (skillIter->second->ReqLearn_Skill2 > 0) {
                                CSkillData *SkillLearnCondition2 = g_CGlobalDataManager->GetSkillbyGroupIDandBasicLevel(skillIter->second->ReqLearn_Skill2, skillIter->second->ReqLearn_SkillLevel2);
                                if (SkillLearnCondition2) {
                                    if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.SkillIsLearned_MAYBE(SkillLearnCondition2->ID)) {
                                        return skillIter->second->ID;
                                    }
                                }

                            } else if (skillIter->second->ReqLearn_Skill1 > 0) {
                                CSkillData *SkillLearnCondition1 = g_CGlobalDataManager->GetSkillbyGroupIDandBasicLevel(skillIter->second->ReqLearn_Skill1, skillIter->second->ReqLearn_SkillLevel1);
                                if (SkillLearnCondition1) {
                                    if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.SkillIsLearned_MAYBE(SkillLearnCondition1->ID)) {
                                        return skillIter->second->ID;
                                    }
                                }
                            } else {
                                return skillIter->second->ID;
                            }
                        }
                    }
                }
            }
        }
    }
}


void CIFSkillBoard::OnTimerIMPL(int timerId) {
    if (timerId == SKILLTIMER) {
        this->KillTimer(SKILLTIMER);
        UpdateMastery(m_Player->Selected_AutoMasteryId, SelectedMasteryLevel);
    }
    else if (timerId == AUTOSKILLTIMER) {
        this->KillTimer(AUTOSKILLTIMER);
        UpdateAutoSkill(m_Player->Selected_AutoMasteryId);
    }
    reinterpret_cast<void *(__thiscall *) (CObjChild *, int)>(0x006548e0)(this, timerId);
}

void CIFSkillBoard::UpdateMastery(int MasteryID, int MasteryLevel) {
    if(this-> m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->GetCheckedState_MAYBE())
    {
        CMsgStreamBuffer buf(0x70A2);
        buf << MasteryID << (byte) 1;
        SendMsg(buf);
        if (MasteryLevel >= 0 && MasteryLevel + 1 < m_Settings->ServerMaxLevel) {
            if (g_pMyPlayerObj->IsChinese()) {
                if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.GetCurrentMasteryCount() < m_Settings->MaxMasteryLevel) {
                    if (MasteryLevel < g_pMyPlayerObj->GetCurrentLevel()) {
                        unsigned int nReqExp = g_CGlobalDataManager->GetLevelData(MasteryLevel + 1).m_expM;
                        if (g_pMyPlayerObj->m_nSkillPoint >= nReqExp) {

                            StartTimer(SKILLTIMER, 130);
                            return;
                        }
                    }
                }
            }
            else if (g_pMyPlayerObj->IsEurope()) {
                if (g_pCGInterface->GetMainPopup()->GetSkill()->sSkillMaps.GetCurrentMasteryCount() < (m_Settings->ServerMaxLevel * 2)) {
                    if (MasteryLevel < g_pMyPlayerObj->GetCurrentLevel()) {
                        unsigned int nReqExp = g_CGlobalDataManager->GetLevelData(MasteryLevel + 1).m_expM;
                        if (g_pMyPlayerObj->m_nSkillPoint >= nReqExp) {

                            StartTimer(SKILLTIMER, 130);
                            return;
                        }
                    }
                }
            }
        }
    }
}
int CIFSkillBoard::OnCloseWndIMPL()
{
    m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_MASTERY_CHECKBOX, 1)->SetCheckBoxState(false);
    m_IRM.GetResObj<CIFCheckBox>(GDR_MAX_SKILL_CHECKBOX, 1)->SetCheckBoxState(false);
    m_Player->Selected_AutoMasteryId = 0;
    m_Player->Enabled_AutoMastery = false;

    m_Player->Enabled_AutoSkill = false;
    m_Player->Selected_AutoSkillId = 0;
    return 0;
}

