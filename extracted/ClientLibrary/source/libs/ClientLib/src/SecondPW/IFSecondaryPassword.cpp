#include "IFSecondaryPassword.h"
#include <Game.h>
#include <ICPlayer.h>
#include <BSLib/multibyte.h>
#include "GInterface.h"
#include <CharacterDependentData.h>
#include <TextStringManager.h>
#include <GlobalDataManager.h>
#include <IFMagicStateBoard.h>
#include <Macro/IFMacroMenu.h>
#include <sstream>
#include <CustomData/CustomDataManager.h>
#include <CustomData/CustomCICPlayer.h>
#include <LockItems/IFNewMsgBox.h>

#define GDR_BLACKSQUARE_DESC 6
#define GDR_TILE_DESC 7
#define GDR_DESC 18

#define GDR_NUMBER_1 20
#define GDR_NUMBER_2 21
#define GDR_NUMBER_3 22
#define GDR_NUMBER_4 23
#define GDR_NUMBER_5 24
#define GDR_NUMBER_6 25
#define GDR_NUMBER_7 26
#define GDR_NUMBER_8 27
#define GDR_NUMBER_9 28
#define GDR_NUMBER_0 29
#define GDR_NUMBER_CLEAN 30
#define GDR_NUMBER_REMOVE 31
#define GDR_CHANGE_PW 10

#define GDR_CREATE_PASSWORD_BUTTON 500

#define GDR_CIFEDIT_PASSWORD 13333
#define GDR_CIFEDIT_PASSWORD_2 13334

#define GDR_CIFEDIT_PASSWORD_3_FOR_CHANGE_PW 13335

GFX_IMPLEMENT_DYNCREATE(CIFSecondaryPassword, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFSecondaryPassword, CIFMainFrame)
                    ONG_COMMAND(GDR_CHANGE_PW, &CIFSecondaryPassword::RemovePw)
                    ONG_COMMAND(5, &CIFSecondaryPassword::Confirm)
                    ONG_COMMAND(GDR_NUMBER_1, &CIFSecondaryPassword::Number1)
                    ONG_COMMAND(GDR_NUMBER_2, &CIFSecondaryPassword::Number2)
                    ONG_COMMAND(GDR_NUMBER_3, &CIFSecondaryPassword::Number3)
                    ONG_COMMAND(GDR_NUMBER_4, &CIFSecondaryPassword::Number4)
                    ONG_COMMAND(GDR_NUMBER_5, &CIFSecondaryPassword::Number5)
                    ONG_COMMAND(GDR_NUMBER_6, &CIFSecondaryPassword::Number6)
                    ONG_COMMAND(GDR_NUMBER_7, &CIFSecondaryPassword::Number7)
                    ONG_COMMAND(GDR_NUMBER_8, &CIFSecondaryPassword::Number8)
                    ONG_COMMAND(GDR_NUMBER_9, &CIFSecondaryPassword::Number9)
                    ONG_COMMAND(GDR_NUMBER_0, &CIFSecondaryPassword::Number0)
                    ONG_COMMAND(GDR_NUMBER_CLEAN, &CIFSecondaryPassword::NumberClean)
                    ONG_COMMAND(GDR_NUMBER_REMOVE, &CIFSecondaryPassword::NumberRemove)

                    ONG_COMMAND(GDR_CREATE_PASSWORD_BUTTON, &CIFSecondaryPassword::CreatePassword)

                    ONG_COMMAND(9, &CIFSecondaryPassword::OnClickCancelBtn)
GFX_END_MESSAGE_MAP()

CIFSecondaryPassword::CIFSecondaryPassword(void){
    Mode = -1;
    Pass1 =  std::n_wstring();
    Pass2 =  std::n_wstring();
    Pass3 =  std::n_wstring();
}
CIFSecondaryPassword::~CIFSecondaryPassword(void){

}

bool CIFSecondaryPassword::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifsecondarypassword.txt");
    m_IRM.CreateInterfaceSection("Create", this);
    wnd_rect sz;
    sz.pos.x= 264;
    sz.pos.y = 53;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers1 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_1, 0);
    numbers1->TB_Func_13("clientlibrary\\2secret\\sec_num_01.ddj", 1, 0);


    sz.pos.x= 314;
    sz.pos.y = 53;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers2 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_2, 0);
    numbers2->TB_Func_13("clientlibrary\\2secret\\sec_num_02.ddj", 1, 0);

    sz.pos.x= 364;
    sz.pos.y = 53;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers3 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_3, 0);
    numbers3->TB_Func_13("clientlibrary\\2secret\\sec_num_03.ddj", 1, 0);


    sz.pos.x= 264;
    sz.pos.y = 103;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers4 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_4, 0);
    numbers4->TB_Func_13("clientlibrary\\2secret\\sec_num_04.ddj", 1, 0);


    sz.pos.x= 314;
    sz.pos.y = 103;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers5 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_5, 0);
    numbers5->TB_Func_13("clientlibrary\\2secret\\sec_num_05.ddj", 1, 0);

    sz.pos.x= 364;
    sz.pos.y = 103;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers6 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_6, 0);
    numbers6->TB_Func_13("clientlibrary\\2secret\\sec_num_06.ddj", 1, 0);


    sz.pos.x= 264;
    sz.pos.y = 153;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers7 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_7, 0);
    numbers7->TB_Func_13("clientlibrary\\2secret\\sec_num_07.ddj", 1, 0);


    sz.pos.x= 314;
    sz.pos.y = 153;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers8 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_8, 0);
    numbers8->TB_Func_13("clientlibrary\\2secret\\sec_num_08.ddj", 1, 0);


    sz.pos.x= 364;
    sz.pos.y = 153;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers9 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_9, 0);
    numbers9->TB_Func_13("clientlibrary\\2secret\\sec_num_09.ddj", 1, 0);


    sz.pos.x= 314;
    sz.pos.y = 203;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbers0 = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_0, 0);
    numbers0->TB_Func_13("clientlibrary\\2secret\\sec_num_00.ddj", 1, 0);


    sz.pos.x= 264;
    sz.pos.y = 203;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbersclean = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_CLEAN, 0);
    numbersclean->TB_Func_13("clientlibrary\\2secret\\sec_num_c.ddj", 1, 0);



    sz.pos.x= 364;
    sz.pos.y = 203;
    sz.size.width = 44;
    sz.size.height = 44;
    CIFButton*numbersremove = (CIFButton*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, GDR_NUMBER_REMOVE, 0);
    numbersremove->TB_Func_13("clientlibrary\\2secret\\sec_num_x.ddj", 1, 0);



    mySecondPasswordBox = this->m_IRM.GetResObj<CIFEdit>(GDR_CIFEDIT_PASSWORD_2, 1);
    mySecondPasswordBox->ShowGWnd(false);
    mySecondPasswordBox->SetStyleThingy(PASSWORD_MASKED);
    mySecondPasswordBox->SetMaxLength(8);

    myFirstPasswordBox = this->m_IRM.GetResObj<CIFEdit>(GDR_CIFEDIT_PASSWORD, 1);
    myFirstPasswordBox->SetStyleThingy(PASSWORD_MASKED);
    myFirstPasswordBox->SetMaxLength(8);
  //  myFirstPasswordBox->SetValue_404(0x80);

    myThirdPasswordBox = this->m_IRM.GetResObj<CIFEdit>(GDR_CIFEDIT_PASSWORD_3_FOR_CHANGE_PW, 1);
    myThirdPasswordBox->SetStyleThingy(PASSWORD_MASKED);
    myThirdPasswordBox->SetMaxLength(8);


    this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->SetCheckBoxState(false);



    this->m_IRM.GetResObj(5, 1)->SetText(L"Confirm");

    myCancelButton = this->m_IRM.GetResObj<CIFButton>(9, 1);
    myCancelButton->SetText(L"Cancel");
//    this->m_IRM.GetResObj(9, 1)->ShowGWnd(false);

    myChangeButton = this->m_IRM.GetResObj<CIFButton>(10, 1);
    myChangeButton->SetText(L"Change");
    myChangeButton->ShowGWnd(false);


    this->m_IRM.GetResObj(511, 1)->ShowGWnd(false);

    this->m_IRM.GetResObj(511, 1)->SetText(L"Remember this PC.");

    this->ShuffleButtons();

    DescTextBox = m_IRM.GetResObj<CIFTextBox>(GDR_DESC, 1);
    DescTextBox->SetLineHeight(15);

    this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->SetText(L"Confirm");
    this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->ShowGWnd(false);
    this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->SetType(1);
    this->SetMode(0);
    this->ShowGWnd(false);
    return true;
}
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <PSTitle.h>
#include <iostream>


void CIFSecondaryPassword::ShuffleButtons() {
    std::vector<std::pair<int, int>> positions;
    positions.push_back(std::make_pair(264, 53));
    positions.push_back(std::make_pair(314, 53));
    positions.push_back(std::make_pair(364, 53));
    positions.push_back(std::make_pair(264, 103));
    positions.push_back(std::make_pair(314, 103));
    positions.push_back(std::make_pair(364, 103));
    positions.push_back(std::make_pair(264, 153));
    positions.push_back(std::make_pair(314, 153));
    positions.push_back(std::make_pair(364, 153));
    positions.push_back(std::make_pair(314, 203));

    // Rastgeleliği başlat
    std::srand(unsigned(std::time(0)));

    // Pozisyonları karıştır
    std::random_shuffle(positions.begin(), positions.end());

    // Karışık butonların pozisyonlarını ayarla
    for (int i = 0; i < positions.size(); i++) {
        int x = positions[i].first;
        int y = positions[i].second;

        // GDR_NUMBER_0 - 1 (GDR_NUMBER_1, GDR_NUMBER_2, ..., GDR_NUMBER_0) butonlarını taşımak için
        this->GetGuiFromList<CIFButton>(GDR_NUMBER_1 + i)->MoveGWnd(x, y);
    }

    // Sabit butonların pozisyonlarını ayarla (örnek olarak, GDR_NUMBER_30 ve sonrası)
    // Sabit butonlar burada eklenebilir
    // this->m_IRM.GetResObj(GDR_NUMBER_30, 1)->MoveGWnd(sabit_x, sabit_y);
}


void CIFSecondaryPassword::Confirm()
{
    if(Mode == 1)
    {
        CMsgStreamBuffer buf(0x1211);
        buf << (byte)2;
        std::wstringstream pw;
        pw << Pass1; // c_str() kullanmaya gerek yok
        int firstpw;
        pw >> firstpw;
        buf << firstpw;
        buf << (byte)this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->GetCheckedState_MAYBE();
        SendMsg(buf);
    }
}
void CIFSecondaryPassword::RemovePw()
{
    this->SetMode(2);
}
void CIFSecondaryPassword::Number1(){
    WriteNumber(L"1");
}
void CIFSecondaryPassword::Number2(){
    WriteNumber(L"2");
}
void CIFSecondaryPassword::Number3(){
    WriteNumber(L"3");
}
void CIFSecondaryPassword::Number4(){
    WriteNumber(L"4");
}
void CIFSecondaryPassword::Number5(){
    WriteNumber(L"5");
}
void CIFSecondaryPassword::Number6(){
    WriteNumber(L"6");
}
void CIFSecondaryPassword::Number7(){
    WriteNumber(L"7");
}
void CIFSecondaryPassword::Number8(){
    WriteNumber(L"8");
}
void CIFSecondaryPassword::Number9(){
    WriteNumber(L"9");
}
void CIFSecondaryPassword::Number0(){
    WriteNumber(L"0");
}
void CIFSecondaryPassword::NumberClean(){
    if(m_Player->FocusedId == 1)
    {
        myFirstPasswordBox->SetText(L"");
        Pass1 = L"";
    }
    else if(m_Player->FocusedId == 2)
    {
        mySecondPasswordBox->SetText(L"");
        Pass2 = L"";
    }
    else if(m_Player->FocusedId == 3)
    {
        myThirdPasswordBox->SetText(L"");
        Pass3 = L"";
    }
}
void CIFSecondaryPassword::ClearAll()
{
    myFirstPasswordBox->SetText(L"");
    mySecondPasswordBox->SetText(L"");
    myThirdPasswordBox->SetText(L"");
    Pass1 = L"";
    Pass2 = L"";
    Pass3 = L"";
}

void CIFSecondaryPassword::NumberRemove()
{
    if (m_Player->FocusedId == 1)
    {
        std::n_wstring input = myFirstPasswordBox->GetNText();
        if (!input.empty())
        {
            Pass1 = Pass1.substr(0, Pass1.size() - 1);
            input = input.substr(0, input.size() - 1);
            myFirstPasswordBox->SetText(input.c_str());

        }
    }
    else if (m_Player->FocusedId == 2)
    {
        std::n_wstring input = mySecondPasswordBox->GetNText();
        if (!input.empty())
        {
            Pass2 = Pass2.substr(0, Pass2.size() - 1);
            input = input.substr(0, input.size() - 1);
            mySecondPasswordBox->SetText(input.c_str());
        }
    }
    else if (m_Player->FocusedId == 3)
    {
        std::n_wstring input = myThirdPasswordBox->GetNText();
        if (!input.empty())
        {
            Pass3 = Pass3.substr(0, Pass3.size() - 1);
            input = input.substr(0, input.size() - 1);
            myThirdPasswordBox->SetText(input.c_str());
        }
    }
}






void CIFSecondaryPassword::WriteNumber(std::n_wstring number)
{
    if (m_Player->FocusedId == 1)
    {
        std::n_wstring input = myFirstPasswordBox->GetNText();

        if (input.length() < 8)  // Maximum length 8
        {
            for (size_t i = 0; i < number.length(); ++i)
            {
                wchar_t ch = number[i];
                WPARAM wParam = (WPARAM)ch;

                // WM_KEYDOWN mesajı için lParam hesapla
                LPARAM lParamKeyDown = 1 | (MapVirtualKey((UINT)wParam, 0) << 16);

                // WM_CHAR mesajı için lParam (genellikle basit bir değer)
                LPARAM lParamChar = MAKELPARAM(1, 1);  // Daha doğru bir lParam değeri

                // WM_KEYUP mesajı için lParam hesapla
                LPARAM lParamKeyUp = lParamKeyDown | (1 << 31);
                myFirstPasswordBox->SetFocus_MAYBE();
                // WM_KEYDOWN mesajı gönder
                myFirstPasswordBox->OnCharIMPLSelf(myFirstPasswordBox->m_hEditBoxWnd, WM_KEYDOWN, lParamKeyDown, wParam);

                // WM_CHAR mesajı gönder
                myFirstPasswordBox->OnCharIMPLSelf(myFirstPasswordBox->m_hEditBoxWnd, WM_CHAR, lParamChar, wParam);

                // WM_KEYUP mesajı gönder
                myFirstPasswordBox->OnCharIMPLSelf(myFirstPasswordBox->m_hEditBoxWnd, WM_KEYUP, lParamKeyUp, wParam);

                // Maskelenmemiş metni sakla
                Pass1 += ch;
            }
        }
    }
    else if (m_Player->FocusedId == 2)
    {
        std::n_wstring input = mySecondPasswordBox->GetNText();

        if (input.length() < 8)  // Maximum length 8
        {
            for (size_t i = 0; i < number.length(); ++i)
            {
                wchar_t ch = number[i];
                WPARAM wParam = (WPARAM)ch;
                mySecondPasswordBox->SetFocus_MAYBE();
                // WM_KEYDOWN mesajı için lParam hesapla
                LPARAM lParamKeyDown = 1 | (MapVirtualKey((UINT)wParam, 0) << 16);

                // WM_CHAR mesajı için lParam (genellikle basit bir değer)
                LPARAM lParamChar = MAKELPARAM(1, 1);  // Daha doğru bir lParam değeri

                // WM_KEYUP mesajı için lParam hesapla
                LPARAM lParamKeyUp = lParamKeyDown | (1 << 31);

                // WM_KEYDOWN mesajı gönder
                mySecondPasswordBox->OnCharIMPLSelf(mySecondPasswordBox->m_hEditBoxWnd, WM_KEYDOWN, lParamKeyDown, wParam);

                // WM_CHAR mesajı gönder
                mySecondPasswordBox->OnCharIMPLSelf(mySecondPasswordBox->m_hEditBoxWnd, WM_CHAR, lParamChar, wParam);

                // WM_KEYUP mesajı gönder
                mySecondPasswordBox->OnCharIMPLSelf(mySecondPasswordBox->m_hEditBoxWnd, WM_KEYUP, lParamKeyUp, wParam);

                // Maskelenmemiş metni sakla
                Pass2 += ch;
            }
        }
    }
    else if (m_Player->FocusedId == 3)
    {

        std::n_wstring input = myThirdPasswordBox->GetNText();

        if (input.length() < 8)  // Maximum length 8
        {
            for (size_t i = 0; i < number.length(); ++i)
            {
                wchar_t ch = number[i];
                WPARAM wParam = (WPARAM)ch;
                myThirdPasswordBox->SetFocus_MAYBE();
                // WM_KEYDOWN mesajı için lParam hesapla
                LPARAM lParamKeyDown = 1 | (MapVirtualKey((UINT)wParam, 0) << 16);

                // WM_CHAR mesajı için lParam (genellikle basit bir değer)
                LPARAM lParamChar = MAKELPARAM(1, 1);  // Daha doğru bir lParam değeri

                // WM_KEYUP mesajı için lParam hesapla
                LPARAM lParamKeyUp = lParamKeyDown | (1 << 31);

                // WM_KEYDOWN mesajı gönder
                myThirdPasswordBox->OnCharIMPLSelf(myThirdPasswordBox->m_hEditBoxWnd, WM_KEYDOWN, lParamKeyDown, wParam);

                // WM_CHAR mesajı gönder
                myThirdPasswordBox->OnCharIMPLSelf(myThirdPasswordBox->m_hEditBoxWnd, WM_CHAR, lParamChar, wParam);

                // WM_KEYUP mesajı gönder
                myThirdPasswordBox->OnCharIMPLSelf(myThirdPasswordBox->m_hEditBoxWnd, WM_KEYUP, lParamKeyUp, wParam);

                // Maskelenmemiş metni sakla
                Pass3 += ch;
            }
        }
    }
}

void CIFSecondaryPassword::UpdateMenuSize()
{

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}


void CIFSecondaryPassword::OnUpdate()
{

}
void CIFSecondaryPassword::Clear()
{

}
void CIFSecondaryPassword::On_BtnClick() {

}

void CIFSecondaryPassword::SetMode(byte type)
{
    Mode = type;
    if(type == 0) /// TODO CREATE PASS
    {
        ClearAll();
        myChangeButton->ShowGWnd(false);
        mySecondPasswordBox->ShowGWnd(true);
        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->MoveGWnd(this->GetPos().x +13, this->GetPos().y +107);
        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->SetGWndSize(227, 148);

        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->MoveGWnd(this->GetPos().x +17, this->GetPos().y + 110);
        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->SetGWndSize(219, 143);

        DescTextBox->MoveGWnd(this->GetPos().x + 20, this->GetPos().y +120);
        this->SetText(L"Create Secondary Passcode");

        std::n_wstring Text1 = L"Click the keypad to the right to create a passcode.";

        std::n_wstring Text2 = L"The passcode must be between 6-8 digits.\n\n";

        std::n_wstring Text3 = L"You are recommended to select a passcode that is unrelated to your account password.";
        std::n_wstring Text4 = Text1 + L"\n" + Text2 + Text3;

        DescTextBox->SetText(Text4.c_str());



        this->m_IRM.GetResObj(16, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(17, 1)->ShowGWnd(true);


        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(5, 1)->ShowGWnd(false);

        this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(511, 1)->ShowGWnd(false);

        wnd_pos x = this->GetPos();
        myCancelButton->MoveGWnd(x.x + 164, x.y + 266);

        this->m_IRM.GetResObj(199, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(200, 1)->ShowGWnd(false);
        myThirdPasswordBox->ShowGWnd(false);

        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->MoveGWnd(x.x + 164, x.y + 46);

    }
    else if(type == 1) /// TODO ENTER PASS
    {
        ClearAll();
        myChangeButton->ShowGWnd(true);
        mySecondPasswordBox->ShowGWnd(false);
        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->MoveGWnd(this->GetPos().x +13, this->GetPos().y +75);
        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->SetGWndSize(227, 180);

        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->MoveGWnd(this->GetPos().x +17, this->GetPos().y +78);
        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->SetGWndSize(219, 175);
        DescTextBox->MoveGWnd(this->GetPos().x +20, this->GetPos().y +88);
        DescTextBox->SetText(L"Click the keypad to the right to create a passcode.");

        this->m_IRM.GetResObj(16, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(17, 1)->ShowGWnd(false);



        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(5, 1)->ShowGWnd(true);

        this->SetText(L"Enter Secondary Passcode");
        this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(511, 1)->ShowGWnd(true);

        wnd_pos x = this->GetPos();
        myCancelButton->MoveGWnd(x.x + 345, x.y + 266);

        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->MoveGWnd(x.x + 164, x.y + 46);

        this->m_IRM.GetResObj(199, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(200, 1)->ShowGWnd(false);
        myThirdPasswordBox->ShowGWnd(false);
    }
    else if(type == 2) /// TODO CHANGE PASS
    {
        ClearAll();
        myChangeButton->ShowGWnd(true);
        mySecondPasswordBox->ShowGWnd(true);

        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->MoveGWnd(this->GetPos().x +13, this->GetPos().y +139);
        this->m_IRM.GetResObj<CIFStretchWnd>(GDR_BLACKSQUARE_DESC, 1)->SetGWndSize(227, 116);

        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->MoveGWnd(this->GetPos().x +17, this->GetPos().y + 141);
        this->m_IRM.GetResObj<CIFNormalTile>(GDR_TILE_DESC, 1)->SetGWndSize(219, 111);

        DescTextBox->MoveGWnd(this->GetPos().x + 20, this->GetPos().y + 144);
        this->SetText(L"Change Secondary Passcode");

        std::n_wstring Text1 = L"Enter your current passcode and enter the new passcode twice.";

        std::n_wstring Text4 = Text1;

        DescTextBox->SetText(Text4.c_str());

        this->m_IRM.GetResObj(16, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(17, 1)->ShowGWnd(true);


        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(5, 1)->ShowGWnd(false);

        this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->ShowGWnd(false);
        this->m_IRM.GetResObj(511, 1)->ShowGWnd(false);

        wnd_pos x = this->GetPos();
    //    myCancelButton->MoveGWnd(x.x + 164, x.y + 266);

        this->m_IRM.GetResObj(GDR_CREATE_PASSWORD_BUTTON, 1)->MoveGWnd(x.x + 164, x.y + 110);

        this->m_IRM.GetResObj(199, 1)->ShowGWnd(true);
        this->m_IRM.GetResObj(200, 1)->ShowGWnd(true);
        myThirdPasswordBox->ShowGWnd(true);
    }
}
undefined1 CIFSecondaryPassword::OnCloseWnd(){
    return 0;
}
void CIFSecondaryPassword::OnClickCancelBtn()
{
    if(Mode == 0)
    {
        this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->UpdateMenuSize();
        this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->ShowGWnd(true);
    }
    else if(Mode == 1)
    {
        this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->UpdateMenuSize();
        this->m_IRM.GetResObj<CIFNewMsgBox>(610, 1)->ShowGWnd(true);
    }
    else if(Mode == 2)
    {
        this->SetMode(1);
    }
}
void CIFSecondaryPassword::CreatePassword(){
    if(Mode == 0)
    {
        if(Pass1.length() > 5)
        {
            if(Pass1 == Pass2)
            {
                CMsgStreamBuffer buf(0x1211);
                buf << (byte)0;
                std::wstringstream pw;
                pw << Pass1; // c_str() kullanmaya gerek yok
                int firstpw;
                pw >> firstpw;
                buf << firstpw;
                SendMsg(buf);
            }
            else
            {
                if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CPSTitle)))
                {
                    CPSTitle * titlewnd = (CPSTitle*)this->GetParentControl();
                    titlewnd->ShowMessage(L"Passwords doesnt match.", 0xFFFF671D);
                }
            }
        }
        else
        {
            if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CPSTitle)))
            {
                CPSTitle * titlewnd = (CPSTitle*)this->GetParentControl();
                titlewnd->ShowMessage(L"The passcode must be between 6-8 digits.", 0xFFFF671D);
            }
        }
    }
    else if(Mode == 2)
    {
        if(Pass1.length() < 5)
        {
            if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CPSTitle)))
            {
                CPSTitle * titlewnd = (CPSTitle*)this->GetParentControl();
                titlewnd->ShowMessage(L"The passcode must be between 6-8 digits.", 0xFFFF671D);
            }
        }
        else
        {
            if(Pass3 == Pass2)
            {
                CMsgStreamBuffer buf(0x1211);
                buf << (byte)1; /// CHANGE PW
                std::wstringstream pw;
                pw << Pass1; // c_str() kullanmaya gerek yok
                int firstpw;
                pw >> firstpw;

                buf << firstpw;

                std::wstringstream pw2;
                pw2 << Pass3; // c_str() kullanmaya gerek yok
                int secondpw;
                pw2 >> secondpw;

                buf << secondpw;
                SendMsg(buf);
            }
            else
            {
                if(this->GetParentControl()->IsSame(GFX_RUNTIME_CLASS(CPSTitle)))
                {
                    CPSTitle * titlewnd = (CPSTitle*)this->GetParentControl();
                    titlewnd->ShowMessage(L"Passwords doesnt match.", 0xFFFF671D);
                }
            }
        }
    }
}


void CIFSecondaryPassword::SetRememberPC(bool a)
{
    this->m_IRM.GetResObj<CIFCheckBox>(510, 1)->SetCheckBoxState(a);

}