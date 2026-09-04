
#include <Data/ItemData.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>
#include <GInterface.h>
#include <CustomData/CustomDataManager.h>
#include "IFChest.h"


GFX_IMPLEMENT_DYNCREATE(CIFChestSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFChestSlot, CIFWnd)
                    ONG_COMMAND(10, &TakeBtn)
GFX_END_MESSAGE_MAP()
CIFChestSlot::CIFChestSlot(void)
{
    DatabaseID = 0;
}
CIFChestSlot::~CIFChestSlot(void)
{
}
bool CIFChestSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifchestslot.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->m_IRM.GetResObj<CIFButton>(10, 1)->SetText(L"Take");
    this->m_IRM.GetResObj<CIFButton>(10, 1)->ShowGWnd(false);
    return true;
}
void CIFChestSlot::OnUpdate() {

}
void CIFChestSlot::SetName(int FakeNum, int DbID, int ItemID, int Quantity, const wchar_t *Date, const wchar_t *Type, byte plus){
    DatabaseID = DbID;

    wchar_t buffer3[253];
    swprintf_s(buffer3, L"%d.", FakeNum);

    this->m_IRM.GetResObj<CIFStatic>(3, 1)->SetText(buffer3);


    std::n_wstring Name;
    const SItemData* data = &g_CGlobalDataManager->GetItemData(ItemID);

    if (data != NULL)
    {
        Name = g_CTextStringManager->GetString2(data->NameStrID.c_str())->c_str();
    }
    else
    {
        Name = L"<Unknown>";
    }
    wchar_t buffer1[255];
    swprintf_s(buffer1, L"%s (+%d)", Name.c_str(), plus);
    this->m_IRM.GetResObj<CIFStatic>(5, 1)->SetText(buffer1);

    wchar_t buffer2[255];
    swprintf_s(buffer2, L"%d", Quantity);
    this->m_IRM.GetResObj<CIFStatic>(6, 1)->SetText(buffer2);


    this->m_IRM.GetResObj<CIFStatic>(7, 1)->SetText(Date);
    this->m_IRM.GetResObj<CIFStatic>(8, 1)->SetText(Type);
    this->m_IRM.GetResObj<CIFButton>(10, 1)->ShowGWnd(true);
}
void CIFChestSlot::Clear()
{
    DatabaseID = 0;
    this->m_IRM.GetResObj<CIFStatic>(3, 1)->SetText(L"");



    this->m_IRM.GetResObj<CIFStatic>(5, 1)->SetText(L"");

    this->m_IRM.GetResObj<CIFStatic>(6, 1)->SetText(L"");


    this->m_IRM.GetResObj<CIFStatic>(7, 1)->SetText(L"");
    this->m_IRM.GetResObj<CIFStatic>(8, 1)->SetText(L"");
    this->m_IRM.GetResObj<CIFButton>(10, 1)->ShowGWnd(false);
}
void CIFChestSlot::TakeBtn()
{
    if(g_pCGInterface->m_IRM.GetResObj<CIFChest>(ChestID, 1)->TakeAllWorking)
        return;
    if(!g_pCGInterface->IsInteractionBlocked(13))
    {
        unsigned __int8 InventorySlot2 = g_CGlobalDataManager->GetEmptyInventorySlots();
        if (InventorySlot2 == 0)
        {
            g_pCGInterface->ShowMessage_Warning(L"Inventory is full.");
            return;
        }
        if(DatabaseID != 0)
        {
            if(g_pCGInterface->m_IRM.GetResObj<CIFChest>(ChestID, 1)->my_Chest.find(DatabaseID) != g_pCGInterface->m_IRM.GetResObj<CIFChest>(ChestID, 1)->my_Chest.end())
            {
                g_pCGInterface->m_IRM.GetResObj<CIFChest>(ChestID, 1)->my_Chest.erase(DatabaseID);
                CMsgStreamBuffer buf(0x169A);
                buf << BYTE(0x7) << DatabaseID;
                SendMsg(buf);
                g_pCGInterface->m_IRM.GetResObj<CIFChest>(ChestID, 1)->ReloadRanks();

            }
        }
    }
}
