#include <ClientNet\MsgStreamBuffer.h>
#include "IFAchievementsSlot.h"
#include "IFAchievements.h"
#include <GInterface.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>
#include <sstream>
#include <CustomData/CustomDataManager.h>
#include <sstream> // Gerekli kütüphane
#include <CustomData/CustomCICPlayer.h>



GFX_IMPLEMENT_DYNCREATE(CIFAchievementsSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFAchievementsSlot, CIFWnd)
GFX_END_MESSAGE_MAP()

CIFAchievementsSlot::CIFAchievementsSlot(void){
    Description = L"";
}
CIFAchievementsSlot::~CIFAchievementsSlot(void){

}


bool CIFAchievementsSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifachievementsslot.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    Description = L"";
    DatabaseID = 0;
    this->ShowGWnd(false);
    return true;
}

int CIFAchievementsSlot::OnMouseLeftUp(int a1, int x, int y) {

    g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->ClearDDJ();
    if(DatabaseID != 0)
    {
        g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->SetDescBoxText(Description);
        if(m_Player->m_Achievements.find(DatabaseID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[DatabaseID] == 1)
            {
                g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->SelectedItemID = DatabaseID;
                g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->SetUseButtonState(true);
            }
            else
            {
                g_pCGInterface->m_IRM.GetResObj<CIFAchievements>(AchievementsID, 1)->SetUseButtonState(false);
            }
        }

        this->m_IRM.GetResObj<CIFBarWnd>(3, 1)->TB_Func_13("interface\\ifcommon\\com_bar01select_", 1, 1);
    }


    return true;
}
void CIFAchievementsSlot::ClearDDJ()
{
    m_IRM.GetResObj<CIFBarWnd>(3, 1)->TB_Func_13("interface\\ifcommon\\com_bar01_", 0, 0);
}

void CIFAchievementsSlot::OnUpdate() {

}
void CIFAchievementsSlot::WriteLine(int DbID, byte Category, std::wstring Name){
    DatabaseID = DbID;
    UpdateDefaultIcons(Category, DbID);
    this->m_IRM.GetResObj<CIFStatic>(5, 1)->SetText(Name.c_str());
    UpdateDefaultDescs(DbID);
}


void CIFAchievementsSlot::UpdateDefaultDescs(int AchievementID)
{
    std::vector<int> foundedIndexes;

    // Eşleşen indeksleri bul
    for (size_t i = 0; i < m_CustomDataManager->m_RefAchievementCondition.size(); ++i)
    {
        if (m_CustomDataManager->m_RefAchievementCondition[i].RefAchievementID == AchievementID)
        {
            foundedIndexes.push_back(i);
        }
    }

    std::wstringstream buffer;  // Stringstream kullanarak dinamik tampon oluştur
    std::n_wstring Name = this->m_IRM.GetResObj<CIFStatic>(5, 1)->GetNText();

    // İlk satırı ekle
    buffer << L"<" << Name << L">\n\nRequirements\n";

    // Eşleşmeler varsa devam et
    if (!foundedIndexes.empty())
    {
        for (size_t j = 0; j < foundedIndexes.size(); ++j)
        {
            int foundedIndex = foundedIndexes[j];
            if (m_CustomDataManager->m_RefAchievementCondition[foundedIndex].Type == 0)
            {
                if(m_Player->m_AchievementsCondition.find(m_CustomDataManager->m_RefAchievementCondition[foundedIndex].ID)
                   != m_Player->m_AchievementsCondition.end())
                {
                    if(m_Player->m_AchievementsCondition[m_CustomDataManager->m_RefAchievementCondition[foundedIndex].ID].AchievementID == AchievementID)
                    {
                        if(m_Player->m_AchievementsCondition[m_CustomDataManager->m_RefAchievementCondition[foundedIndex].ID].ProgressCount >=
                                m_CustomDataManager->m_RefAchievementCondition[foundedIndex].CompleteCount)
                        {
                            buffer << m_CustomDataManager->m_RefAchievementCondition[foundedIndex].Name.c_str()
                                   << L" (" <<  Insert(m_Player->m_AchievementsCondition[m_CustomDataManager->m_RefAchievementCondition[foundedIndex].ID].ProgressCount)
                                   << L"/" << Insert(m_CustomDataManager->m_RefAchievementCondition[foundedIndex].CompleteCount) << L") "
                                   << L"<Finished>\n";
                        }
                        else
                        {
                            buffer << m_CustomDataManager->m_RefAchievementCondition[foundedIndex].Name.c_str()
                                   << L" (" <<  Insert(m_Player->m_AchievementsCondition[m_CustomDataManager->m_RefAchievementCondition[foundedIndex].ID].ProgressCount)
                                   << L"/" << Insert(m_CustomDataManager->m_RefAchievementCondition[foundedIndex].CompleteCount) << L") "
                                   << L"<Not Finished>\n";
                        }

                    }
                    else
                    {
                        buffer << m_CustomDataManager->m_RefAchievementCondition[foundedIndex].Name.c_str()
                               << L" (" <<  0 << L"/" << Insert(m_CustomDataManager->m_RefAchievementCondition[foundedIndex].CompleteCount) << L") "
                               << L"<Not Finished>\n";
                    }

                }
                else
                {
                    buffer << m_CustomDataManager->m_RefAchievementCondition[foundedIndex].Name.c_str()
                           << L" (" << 0 << L"/" << Insert(m_CustomDataManager->m_RefAchievementCondition[foundedIndex].CompleteCount) << L") "
                           << L"<Not Finished>\n";
                }
            }
        }
    }
    if(m_CustomDataManager->m_RefAchievement[DatabaseID].RewardType == 0)
    {
        buffer << L"\nRewards"
               << L"\nTitle : "<< L"<"
                  << m_CustomDataManager->m_RefAchievement[DatabaseID].RewardTitleName.c_str() << L">";
    }
    else if(m_CustomDataManager->m_RefAchievement[DatabaseID].RewardType == 1)
    {
        buffer << L"\nRewards\n"
               << L"\nSkill Point : "<< L"<"
               << m_CustomDataManager->m_RefAchievement[DatabaseID].RewardSkillPoint << L">";
    }
    else if(m_CustomDataManager->m_RefAchievement[DatabaseID].RewardType == 2)
    {
        buffer << L"\nRewards\n"
               << L"\nGold : "<< L"<"
               << m_CustomDataManager->m_RefAchievement[DatabaseID].RewardGold << L">";
    }

    Description = buffer.str().c_str();  // Sonuç olarak buffer'ı al
}

void CIFAchievementsSlot::UpdateDefaultIcons(byte CateID, int AchievementID)
{    // 2 unique

    if(CateID == 0)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_all_gray.ddj", 0, 0);
    }
    else if(CateID == 1)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_quest_gray.ddj", 0, 0);
    }
    else if(CateID == 2)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_unique_gray.ddj", 0, 0);
    }
    else if(CateID == 3)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_match_gray.ddj", 0, 0);
    }
    else if(CateID == 4)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_event_gray.ddj", 0, 0);
    }


    if(CateID == 0)
    {
        if(m_Player->m_Achievements.find(AchievementID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[AchievementID] == 0)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_all_gray.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 1)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_all.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 2)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_all.ddj", 0, 0);
            }

        }
        else
        {
            this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_all_gray.ddj", 0, 0);
        }
    }
    else if(CateID == 1)
    {
        if(m_Player->m_Achievements.find(AchievementID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[AchievementID] == 0)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_quest_gray.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 1)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_quest.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 2)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_quest.ddj", 0, 0);
            }

        }
        else
        {
            this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_quest_gray.ddj", 0, 0);
        }
    }
    else if(CateID == 2)
    {
        if(m_Player->m_Achievements.find(AchievementID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[AchievementID] == 0)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_unique_gray.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 1)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_unique.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 2)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_unique.ddj", 0, 0);
            }

        }
        else
        {
            this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_unique_gray.ddj", 0, 0);
        }
    }
    else if(CateID == 3)
    {
        if(m_Player->m_Achievements.find(AchievementID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[AchievementID] == 0)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_match_gray.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 1)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_match.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 2)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_match.ddj", 0, 0);
            }

        }
        else
        {
            this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_match_gray.ddj", 0, 0);
        }
    }
    else if(CateID == 4)
    {
        if(m_Player->m_Achievements.find(AchievementID) != m_Player->m_Achievements.end())
        {
            if(m_Player->m_Achievements[AchievementID] == 0)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_event_gray.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 1)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_event.ddj", 0, 0);
            }
            else if(m_Player->m_Achievements[AchievementID] == 2)
            {
                this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_event.ddj", 0, 0);
            }

        }
        else
        {
            this->m_IRM.GetResObj<CIFStatic>(4, 1)->TB_Func_13("clientlibrary\\title\\title_icon_mini_event_gray.ddj", 0, 0);
        }
    }
}

