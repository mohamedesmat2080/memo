#include "IFChest.h"
#include <Game.h>
#include <ICPlayer.h>
#include <BSLib/multibyte.h>
#include "GInterface.h"
#include <CharacterDependentData.h>
#include <TextStringManager.h>
#include <GlobalDataManager.h>
#include <CustomData/CustomDataManager.h>


#define GDR_TAKE_ALL 23


#define GDR_NEXT 902
#define GDR_PREV 901
#define GDR_SPIN_TEXT_PAGE 900


GFX_IMPLEMENT_DYNCREATE(CIFChest, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFChest, CIFMainFrame)
                    ONG_COMMAND(100, &CIFChest::OnUnknownStuff)
                    ONG_COMMAND(GDR_TAKE_ALL, &CIFChest::On_BtnClick)
                    ONG_COMMAND(GDR_PREV, &On_PrevBtn)
                    ONG_COMMAND(GDR_NEXT, &On_NextBtn)
GFX_END_MESSAGE_MAP()

CIFChest::CIFChest(void){
    m_CurrentIndex = 1;
    m_MaxIndex = 1;
    TakeAllWorking = false;
    my_Chest = std::map<int, CharChest>();
}
CIFChest::~CIFChest(void){
    if (m_pTabs) {
        free(m_pTabs);
        m_pTabs = 0;
    }
}
int CIFChest::Func_4(int a2) {
    int v1 = 0;
    while (a2 != v1 + 100) {
        if (++v1 >= numberOfTabs)
            return -1;
    }

    return 100;
}

void CIFChest::OnUnknownStuff() {
    int id = GetCurrentEventMsgCtrlId();
    int i = 0;

    for (int i = 0; i < numberOfTabs; ++i) {
        if (id == m_pTabs[i]->UniqueID()) {
            ActivateTabPage(i);
            return;
        }
    }
}
void CIFChest::ActivateTabPage(BYTE page) {

}
bool CIFChest::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);
    wnd_rect sz;

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifchest.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    m_pTabs = new CIFSelectableArea *[numberOfTabs];

    for (int i = 0; i < numberOfTabs; i++) {

        RECT mrect;
        mrect.top = 5;
        mrect.left = 6;
        mrect.right = 0;
        mrect.bottom = 4;

        RECT selectable_area_size;
        selectable_area_size.top = 35;
        selectable_area_size.left = tabMarginLeft + tabWidth * i;
        selectable_area_size.right = tabWidth - 4;
        selectable_area_size.bottom = tabHeight;

        m_pTabs[i] = (CIFSelectableArea *) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSelectableArea),
                                                                 selectable_area_size, tabFirstId + i, 0);

        m_pTabs[i]->SetSomeRect(mrect);
        m_pTabs[i]->SetFont(this->N00009C2F);

        m_pTabs[i]->sub_64CE30("interface\\mall\\mall_sub_tab_on.ddj",
                               "interface\\mall\\mall_sub_tab_off.ddj", "interface\\mall\\mall_sub_tab_off.ddj");

        if (i) {
            switch (i) {
                case 1:
                    m_pTabs[1]->SetText(L"Log");
                    break;

            }
            m_pTabs[i]->sub_64CC30(0);
        } else {
            m_pTabs[0]->SetText(L"Items");
            m_pTabs[i]->sub_64CC30(1);
        }
    }
    m_pTabs[1]->ShowGWnd(false);
    m_pTabs[1]->SetClickable(false);


    this->SetText(L"Item Chest");
    this->m_IRM.GetResObj(23, 1)->SetText(L"Take All");
    this->m_IRM.GetResObj(9, 1)->SetText(L"Take");
    this->m_IRM.GetResObj(8, 1)->SetText(L"Type");

    this->m_IRM.GetResObj(7, 1)->SetText(L"Date");
    this->m_IRM.GetResObj(6, 1)->SetText(L"Qty.");
    this->m_IRM.GetResObj(5, 1)->SetText(L"Item Name");
    this->m_IRM.GetResObj(10, 1)->SetText(L"No.");
    this->m_IRM.GetResObj<CIFStatic>(GDR_SPIN_TEXT_PAGE, 1)->SetText(L"1");



    std::n_wstring msg = L"Inventory Remaining";
    std::n_wstring categoryshort = msg.substr(0, 12) + L"...";
    m_IRM.GetResObj(25, 1)->SetText(categoryshort.c_str());
    m_IRM.GetResObj(25, 1)->SetTooltip(msg);
    m_IRM.GetResObj(25, 1)->SetStyleThingy(TOOLTIP);

    for(int i = 0; i < 11; i++)
    {
        m_IRM.GetResObj<CIFChestSlot>(50, 1)->ShowGWnd(true);
    }

    this->ShowGWnd(false);
    return true;
}

void CIFChest::UpdateMenuSize()
{

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    m_IRM.GetResObj<CIFButton>(GDR_TAKE_ALL, 1)->SetEnabledState(true);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void CIFChest::Clear()
{
    for (int i = 0; i < 11; ++i)
    {
        m_IRM.GetResObj<CIFChestSlot>(50 + i, 1)->Clear();
    }
    // UpdateSelfRank(L"empty", 0, -1);
}

void CIFChest::UpdateRanks() {
    Clear();

    m_CurrentIndex = 1;  // Başlangıç sayfası
    m_MaxIndex = (my_Chest.size() + 10) / 11;  // Toplam sayfa sayısı, 7'ye tam bölünmeyenler için +1

    // İlk sayfayı yükle
    LoadPage(m_CurrentIndex);
    UpdateText(m_CurrentIndex);
}
void CIFChest::ReloadRanks() {
    Clear();

    // İlk sayfayı yükle
    LoadPage(m_CurrentIndex);
    UpdateText(m_CurrentIndex);
}

void CIFChest::LoadPage(int pageNumber) {
    int startIndex = (pageNumber - 1) * 11;  // Sayfada gösterilecek ilk öğenin indeksi
    int endIndex = startIndex + 11;          // Sayfada gösterilecek son öğenin indeksi

    Clear();  // Önceki verileri temizle

    // Eğer liste sınırlarını aşıyorsa, son indeksi liste boyutuyla sınırla
    if (endIndex > my_Chest.size()) {
        endIndex = my_Chest.size();
    }
    // Yeni öğeleri yükle
    std::map<int, CharChest>::iterator it = my_Chest.begin();
    std::advance(it, startIndex); // Move the iterator to the start index

    int slotIndex = 0;
    for (int i = startIndex; i < endIndex; ++i, ++it) {
        if (it == my_Chest.end()) break; // Safety check for bounds

        int slotNumber = i - startIndex + 1; // Slot numarası 1'den başlamalı (sayfadaki sıralama)


        m_IRM.GetResObj<CIFChestSlot>(49 + slotNumber, 1)->
                SetName(i + 1, it->second.DbID, it->second.ItemID, it->second.Quantity, it->second.Date.c_str(),
                        it->second.Type.c_str(), it->second.Plus);
        m_IRM.GetResObj<CIFChestSlot>(49 + slotNumber, 1)->ShowGWnd(true);
        ++slotIndex;
    }
}


void CIFChest::UpdateText(int Number)
{
    wchar_t buffer1[255];
    swprintf_s(buffer1, L"%d", Number);
    this->m_IRM.GetResObj<CIFStatic>(GDR_SPIN_TEXT_PAGE, 1)->SetText(buffer1);
}
void CIFChest::On_NextBtn() {
    int totalPages = (my_Chest.size() + 10) / 11;  // Toplam sayfa sayısı
    if (m_CurrentIndex < totalPages) {
        m_CurrentIndex++;
        LoadPage(m_CurrentIndex);  // Yeni sayfayı yükle
        UpdateText(m_CurrentIndex);
    }
}

void CIFChest::On_PrevBtn() {
    if (m_CurrentIndex > 1) {
        m_CurrentIndex--;
        LoadPage(m_CurrentIndex);  // Önceki sayfayı yükle
        UpdateText(m_CurrentIndex);
    }
}
void CIFChest::OnUpdate()
{
    unsigned __int8 test = g_CGlobalDataManager->GetEmptyInventorySlots();
    wchar_t Point[33];
    swprintf_s(Point, L"%d  ", test);
    m_IRM.GetResObj(24, 1)->SetText(Point);
}

#define TakeAllTimer 1345
void CIFChest::On_BtnClick() {
    if(!g_pCGInterface->IsInteractionBlocked(13))
    {
        this->StartTimer(TakeAllTimer, 1000);
        m_IRM.GetResObj<CIFButton>(GDR_TAKE_ALL, 1)->SetEnabledState(false);
        TakeAllWorking = true;
    }

}

void CIFChest::OnTimer(int timerId) {
    if (timerId == TakeAllTimer)
    {
        this->KillTimer(TakeAllTimer);
        int i = 0;

        if(!my_Chest.empty() > 0 && g_CGlobalDataManager->GetEmptyInventorySlots() > 0)
        {
            for (std::map<int, CharChest>::iterator it = my_Chest.begin(); it != my_Chest.end(); ++it)
            {
                i++;
                unsigned __int8 InventorySlot2 = g_CGlobalDataManager->GetEmptyInventorySlots();
                if (InventorySlot2 > 0)
                {
                    CMsgStreamBuffer buf(0x169A);
                    buf << BYTE(0x7) << it->second.DbID;
                    SendMsg(buf);
                    my_Chest.erase(it->second.DbID);
                    Clear();
                    UpdateRanks();
                    break;
                }
            }
            this->StartTimer(TakeAllTimer, 500);
        }
        else
        {
            m_IRM.GetResObj<CIFButton>(GDR_TAKE_ALL, 1)->SetEnabledState(true);
            TakeAllWorking = false;
        }
    }
}


undefined1 CIFChest::OnCloseWnd() {
    this->KillTimer(TakeAllTimer);
    m_IRM.GetResObj<CIFButton>(GDR_TAKE_ALL, 1)->SetEnabledState(true);
    TakeAllWorking = false;
    return CIFWnd::OnCloseWnd();
}