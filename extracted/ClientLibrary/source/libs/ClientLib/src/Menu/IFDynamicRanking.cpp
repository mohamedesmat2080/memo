#include "IFDynamicRanking.h"
#include <Game.h>
#include <ICPlayer.h>
#include <BSLib/multibyte.h>
#include "GInterface.h"
#include "IFDynamicRankingSlot.h"
#include <CharacterDependentData.h>
#include <TextStringManager.h>
#include <GlobalDataManager.h>



#define GDR_NEXT 902
#define GDR_PREV 901
#define GDR_SPIN_TEXT_PAGE 900
#define GDR_RESULT_BUTTON 23

GFX_IMPLEMENT_DYNCREATE(CIFDynamicRanking, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFDynamicRanking, CIFMainFrame)
                    ONG_COMMAND(GDR_RESULT_BUTTON, &On_BtnClick)
                    ONG_COMMAND(GDR_PREV, &On_PrevBtn)
                    ONG_COMMAND(GDR_NEXT, &On_NextBtn)
GFX_END_MESSAGE_MAP()

CIFDynamicRanking::CIFDynamicRanking(void){
    m_popup = 0;
    RankCategorys = std::map<std::n_wstring, int>();
    RankList = std::vector<RankStruct>();
    m_CurrentIndex = 1;
    m_MaxIndex = 1;
}
CIFDynamicRanking::~CIFDynamicRanking(void){
}

bool CIFDynamicRanking::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);


    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifdynamicranking.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->SetText(L"Dynamic Ranking");

    wnd_rect sz;

    this->m_IRM.GetResObj(21, 1)->SetText(L"Guild");
    this->m_IRM.GetResObj(28, 1)->SetText(L"Name");
    this->m_IRM.GetResObj(9, 1)->SetText(L"No.");
    this->m_IRM.GetResObj(22, 1)->SetText(L"Points");
    this->m_IRM.GetResObj(23, 1)->SetText(L"Result");

    this->m_IRM.GetResObj(3, 1)->SetText(L"Rank Type :");

    this->m_IRM.GetResObj<CIFStatic>(GDR_SPIN_TEXT_PAGE, 1)->SetText(L"1");
    this->m_IRM.GetResObj<CIFStatic>(31, 1)->m_FontTexture.SetColor(0x00FF00);
    this->m_IRM.GetResObj<CIFStatic>(32, 1)->m_FontTexture.SetColor(0x00FF00);
    this->m_IRM.GetResObj<CIFStatic>(33, 1)->m_FontTexture.SetColor(0x00FF00);
    this->m_IRM.GetResObj<CIFStatic>(34, 1)->m_FontTexture.SetColor(0x00FF00);
    this->m_IRM.GetResObj<CIFStatic>(31, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(32, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(33, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(34, 1)->ShowGWnd(false);

    sz.pos.x = 151;
    sz.pos.y = 51;
    sz.size.width = 200;
    sz.size.height = 22;
    m_popup = (CIFPopupList*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFPopupList), sz, 30, 0);

    for (int i = 0; i < 10; ++i)
    {
        m_IRM.GetResObj<CIFDynamicRankingSlot>(50 + i, 1)->ShowGWnd(true);
    }
    this->ShowGWnd(false);
    return true;
}

void CIFDynamicRanking::UpdateMenuSize()
{

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}


void CIFDynamicRanking::OnUpdate()
{

}
void CIFDynamicRanking::Hide()
{
    this->m_IRM.GetResObj<CIFStatic>(31, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(32, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(33, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFStatic>(34, 1)->ShowGWnd(false);
}
void CIFDynamicRanking::Clear()
{
    for (int i = 0; i < 10; ++i)
    {
        m_IRM.GetResObj<CIFDynamicRankingSlot>(50 + i, 1)->Clear();
    }
    UpdateSelfRank(L"empty", 0, -1);
}
void CIFDynamicRanking::UpdateRanks() {
    Clear();

    m_CurrentIndex = 1;  // Başlangıç sayfası
    m_MaxIndex = (RankList.size() + 9) / 10;  // Toplam sayfa sayısı, 7'ye tam bölünmeyenler için +1

    // İlk sayfayı yükle
    LoadPage(m_CurrentIndex);
    UpdateText(m_CurrentIndex);
}
void CIFDynamicRanking::LoadPage(int pageNumber) {
    int startIndex = (pageNumber - 1) * 10;  // Sayfada gösterilecek ilk öğenin indeksi
    int endIndex = startIndex + 10;          // Sayfada gösterilecek son öğenin indeksi

    Clear();  // Önceki verileri temizle

    // Eğer liste sınırlarını aşıyorsa, son indeksi liste boyutuyla sınırla
    if (endIndex > RankList.size()) {
        endIndex = RankList.size();
    }
    // Yeni öğeleri yükle
    for (int i = startIndex; i < endIndex; ++i) {
        // Burada slotIndex yerine i'yi doğrudan kullanıyoruz ki doğru sıradan başlasın.
        int slotNumber = i - startIndex + 1; // Slot numarası 1'den başlamalı (sayfadaki sıralama)

        m_IRM.GetResObj<CIFDynamicRankingSlot>(49 + slotNumber, 1)->
                SetName(i + 1, RankList[i].Charname.c_str(), RankList[i].Guild.c_str(), RankList[i].Points.c_str()); // Sıralamaya göre numara
        m_IRM.GetResObj<CIFDynamicRankingSlot>(49 + slotNumber, 1)->ShowGWnd(true);

        // Eğer karakter oyuncunun karakteriyse işaretle
        if (RankList[i].Charname == g_pMyPlayerObj->GetCharName().c_str()) {
            m_IRM.GetResObj<CIFDynamicRankingSlot>(49 + slotNumber, 1)->NewUpdate();
        }
    }
}


void CIFDynamicRanking::UpdateText(int Number)
{
    wchar_t buffer1[255];
    swprintf_s(buffer1, L"%d", Number);
    this->m_IRM.GetResObj<CIFStatic>(GDR_SPIN_TEXT_PAGE, 1)->SetText(buffer1);
}
void CIFDynamicRanking::On_NextBtn() {
    int totalPages = (RankList.size() + 9) / 10;  // Toplam sayfa sayısı
    if (m_CurrentIndex < totalPages) {
        m_CurrentIndex++;
        LoadPage(m_CurrentIndex);  // Yeni sayfayı yükle
        UpdateText(m_CurrentIndex);
    }
}

void CIFDynamicRanking::On_PrevBtn() {
    if (m_CurrentIndex > 1) {
        m_CurrentIndex--;
        LoadPage(m_CurrentIndex);  // Önceki sayfayı yükle
        UpdateText(m_CurrentIndex);
    }
}


void CIFDynamicRanking::UpdateSelfRank(std::n_wstring Name, int No, int Point)
{
    this->m_IRM.GetResObj<CIFStatic>(31, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFStatic>(32, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFStatic>(33, 1)->ShowGWnd(true);
    this->m_IRM.GetResObj<CIFStatic>(34, 1)->ShowGWnd(true);

    if(No != 0)
    {
        wchar_t buffer1[255];
        swprintf_s(buffer1, L"%d", No);

        this->m_IRM.GetResObj<CIFStatic>(31, 1)->SetText(buffer1);
    }
    else
    {
        this->m_IRM.GetResObj<CIFStatic>(31, 1)->SetText(L"0");
    }

    if(Name != L"empty")
    {
        this->m_IRM.GetResObj<CIFStatic>(32, 1)->SetText(Name.c_str());
    }
    else
    {
        this->m_IRM.GetResObj<CIFStatic>(32, 1)->SetText(g_pMyPlayerObj->GetCharName().c_str());
    }

    if(g_pMyPlayerObj->GetGuildName().empty())
    {
        this->m_IRM.GetResObj<CIFStatic>(33, 1)->SetText(L"<No Guild>");
    }
    else
    {
        this->m_IRM.GetResObj<CIFStatic>(33, 1)->SetText(g_pMyPlayerObj->GetGuildName().c_str());
    }

    if(Point != -1)
    {
        std::wstring MyPoints = Insert(Point);
        this->m_IRM.GetResObj<CIFStatic>(34, 1)->SetText(MyPoints.c_str());
    }
    else
    {
        this->m_IRM.GetResObj<CIFStatic>(34, 1)->SetText(L"<None>");
    }

}
void CIFDynamicRanking::On_BtnClick() {
    int RankID = RankCategorys[m_popup->m_text->GetNText()];
    if(RankID == 1)
    {
        Clear();

        byte customrank1 = RankID;
        SendPacket(customrank1);

    }
    else if(RankID == 2)
    {
        Clear();
        byte customrank2 = RankID;
        SendPacket(customrank2);
    }
    else if(RankID == 3)
    {
        Clear();
        byte customrank3 = RankID;
        SendPacket(customrank3);
    }
    else if(RankID == 4)
    {
        Clear();
        byte customrank4 = RankID;
        SendPacket(customrank4);
    }
    else if(RankID == 5)
    {
        Clear();
        byte customrank5 = RankID;
        SendPacket(customrank5);
    }
    else if(RankID == 6)
    {
        Clear();
        byte customrank6 = RankID;
        SendPacket(customrank6);
    }
    else if(RankID == 7)
    {
        Clear();
        byte customrank7 = RankID;
        SendPacket(customrank7);
    }
    else if(RankID == 8)
    {
        Clear();
        byte customrank8 = RankID;
        SendPacket(customrank8);
    }
    else if(RankID == 9)
    {
        Clear();
        byte customrank9 = RankID;
        SendPacket(customrank9);
    }
    else if(RankID == 10)
    {
        Clear();
        byte uniqrank = RankID;
        SendPacket(uniqrank);
    }
    else if(RankID == 11)
    {
        Clear();
        byte thiefrank = RankID;
        SendPacket(thiefrank);
    }
    else if(RankID == 12)
    {
        Clear();
        byte traderrank = RankID;
        SendPacket(traderrank);
    }
    else if(RankID == 13)
    {
        Clear();
        byte hunterrank = RankID;
        SendPacket(hunterrank);
    }
    OpenButton(5000);

}
#define GDR_RESULT_BUTTONTIMER 13134
void CIFDynamicRanking::OpenButton(int timeoutSeconds) {
    this->m_IRM.GetResObj<CIFButton>(GDR_RESULT_BUTTON, 1)->SetEnabledState(0);
    this->StartTimer(GDR_RESULT_BUTTONTIMER, timeoutSeconds);
}

void CIFDynamicRanking::OnTimer(int timerId) {
    if (timerId == GDR_RESULT_BUTTONTIMER) {
        this->KillTimer(GDR_RESULT_BUTTONTIMER);
        this->m_IRM.GetResObj<CIFButton>(GDR_RESULT_BUTTON, 1)->SetEnabledState(1);
    }
}
void CIFDynamicRanking::SendPacket(byte type)
{
    CMsgStreamBuffer buf(0x180A);
    buf << type;
    SendMsg(buf);
}
