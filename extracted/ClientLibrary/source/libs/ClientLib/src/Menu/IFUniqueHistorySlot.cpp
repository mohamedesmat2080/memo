#include <TextStringManager.h>
#include <time.h>
#include <ctime>
#include <GInterface.h>
#include "IFUniqueHistory.h"


GFX_IMPLEMENT_DYNCREATE(CIFUniqueHistorySlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFUniqueHistorySlot, CIFWnd)
GFX_END_MESSAGE_MAP()
CIFUniqueHistorySlot::CIFUniqueHistorySlot(void)
{
    times = 0;
    KilledRegID = 0;
    X = 0;
    Z = 0;
}
CIFUniqueHistorySlot::~CIFUniqueHistorySlot(void)
{
}
bool CIFUniqueHistorySlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifuniquehistoryslot.txt");
    m_IRM.CreateInterfaceSection("Create", this);


    return true;
}
void CIFUniqueHistorySlot::OnUpdate() {

    if(times != 0)
    {
        std::time_t currentTime = std::time(NULL);
        long elapsedTime = currentTime - times;
        long days = elapsedTime / (60 * 60 * 24);
        long hours = (elapsedTime % (60 * 60 * 24)) / (60 * 60);
        long minutes = (elapsedTime % (60 * 60)) / 60;

        wchar_t buffer12[255];
        swprintf_s(buffer12, 255, L"%ldd %ldh %ldm ago", days, hours, minutes);

        this->m_IRM.GetResObj<CIFStatic>(5, 1)->SetText(buffer12);

    }

}


void CIFUniqueHistorySlot::SetName(int Num, const wchar_t* uniquename, byte state, long time, const wchar_t* killer,  int RegionID, int KilledX, int KilledY, int KilledZ) {

    //printf("%ls %d %ld %ls %d %d %d %d\n", uniquename, state, time, killer, RegionID, KilledX, KilledY, KilledZ);

    this->m_IRM.GetResObj<CIFStatic>(3, 1)->SetText(uniquename);

    if(state == 0)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->SetText(L"<Killed>");
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->m_FontTexture.SetColor(0xFF0000);
    }
    else if(state == 1)
    {
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->SetText(L"<Alive>");
        this->m_IRM.GetResObj<CIFStatic>(4, 1)->m_FontTexture.SetColor(0x00FF00);
    }

    times = time;

    std::time_t currentTime = std::time(NULL);
    long elapsedTime = currentTime - time;

    long days = elapsedTime / (60 * 60 * 24);
    long hours = (elapsedTime % (60 * 60 * 24)) / (60 * 60);
    long minutes = (elapsedTime % (60 * 60)) / 60;

    wchar_t buffer12[255];
    swprintf_s(buffer12, 255, L"%ldd %ldh %ldm ago", days, hours, minutes);

    this->m_IRM.GetResObj<CIFStatic>(5, 1)->SetText(buffer12);

    this->m_IRM.GetResObj<CIFStatic>(6, 1)->SetText(killer);

    KilledRegID = RegionID;
    X = KilledX;
    Z = KilledZ;
    Y = KilledY;
}

int CIFUniqueHistorySlot::OnMouseLeftUp(int a1, int x, int y) {

    g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->ClearDDJ();
    if (KilledRegID != 0)
    {

        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedRegionID = KilledRegID;
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedX = X;
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedZ = Z;
        g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedY = Y;
        int RegionType = g_pCGInterface->GetCNIFWorldMap()->GetRegionTypeMaybe(KilledRegID, X , Y , Z);
        if(!g_pCGInterface->GetCNIFWorldMap()->IsVisible())
        {
            g_pCGInterface->GetCNIFWorldMap()->ShowGWnd(true);
        }

        wnd_pos x = g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->GetPos();
        if(g_pCGInterface->GetCNIFWorldMap()->MapType1IsSmall0IsBig == 0)
        {
            g_pCGInterface->GetCNIFWorldMap()->SetMapMode();
        }

        g_pCGInterface->GetCNIFWorldMap()->MoveGWnd(x.x-270, x.y);

      //  printf("%d %d %d %d \n", KilledRegID, X, Y, Z);
         bool isDungeon = (KilledRegID > SHRT_MAX);
        if(!isDungeon)
        {
            int local_34 = (unsigned int)(KilledRegID >> 8);
            int uVar10 = KilledRegID & 0xff;
            int PosX = (int)X >> 0x20;
            int PosZ = (int)Z >> 0x20;
            g_pCGInterface->GetCNIFWorldMap()->field_229ad = 1;
            g_pCGInterface->GetCNIFWorldMap()->Fun_0061a620();
            g_pCGInterface->GetCNIFWorldMap()->FUN_006256e0(0);

            g_pCGInterface->GetCNIFWorldMap()->Fun_00622f00(1073741824, 6881382, 7077989, 100, (undefined4)g_pCGInterface->GetCNIFWorldMap(), 5, 7, uVar10, local_34, PosX, PosZ);

            this->m_IRM.GetResObj<CIFBarWnd>(7, 1)->TB_Func_13("interface\\ifcommon\\com_bar01select_", 1, 1);
        }
    }

    return true;
}

void CIFUniqueHistorySlot::ClearDDJ()
{
    this->m_IRM.GetResObj<CIFBarWnd>(7, 1)->TB_Func_13("interface\\ifcommon\\com_bar01_", 0, 0);
}
void CIFUniqueHistorySlot::Clear()
{
    this->m_IRM.GetResObj(3, 1)->SetText(L"");
    this->m_IRM.GetResObj(4, 1)->SetText(L"");
    this->m_IRM.GetResObj(5, 1)->SetText(L"");
    this->m_IRM.GetResObj(6, 1)->SetText(L"");

    KilledRegID = 0;
    X = 0;
    Z = 0;
    Y = 0;
    times = 0;
}
