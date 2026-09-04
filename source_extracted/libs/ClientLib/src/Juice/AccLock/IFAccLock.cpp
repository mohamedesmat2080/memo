#include "IFAccLock.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GameDataExt.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>
#include <ctime>
#include <sstream>

GFX_IMPLEMENT_DYNCREATE(CIFAccLock, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFAccLock, CIFMainFrame)
            ONG_COMMAND(2007, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2008, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2009, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2010, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2011, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2012, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2013, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2014, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2015, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2016, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2018, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2019, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2023, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2025, &CIFAccLock::OnBtnsClick)
            ONG_COMMAND(2026, &CIFAccLock::OnBtnsClick)
GFX_END_MESSAGE_MAP()
CIFAccLock::CIFAccLock(void)
{
}
CIFAccLock::~CIFAccLock(void)
{
}
bool CIFAccLock::OnCreate(long ln)
{
    CIFMainFrame::OnCreate(ln);

    RECT m_tileRect = { 32,44,428,342 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 2000, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_tile2Rect = { 270,36,214,284 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 2001, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frame2Rect = { 270,36,214,284 };
    m_frame2 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame2Rect, 2002, 0);
    m_frame2->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_tile3Rect = { 7,36,263,284 };
    m_tile3 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile3Rect, 2003, 0);
    m_tile3->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frame3Rect = { 7,36,263,284 };
    m_frame3 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame3Rect, 2004, 0);
    m_frame3->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_tile4Rect = { 7,320,475,76 };
    m_tile4 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile4Rect, 2005, 0);
    m_tile4->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frame4Rect = { 7,320,477,76 };
    m_frame4 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame4Rect, 2006, 0);
    m_frame4->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_btnsRect = {277,43,60,60};
    m_btns = new CIFButton*[10];
    for(int i=0;i<10;i++)
    {
        if(i ==9)
            m_btnsRect.left += 70;

        m_btns[i] = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btnsRect, 2007+i, 0);

        m_btnsRect.left += 70;

        if((i+1)%3==0)
        {
            m_btnsRect.left = 277;
            m_btnsRect.top+=70;
        }

        std::ostringstream temp;
        temp << i;
        std::string numbDDJ = "juicer\\lock\\sec_num_0" + temp.str() + ".ddj";

        m_btns[i]->TB_Func_13(numbDDJ.c_str(),0,0);
    }

    RECT m_xRect = {417,253,60,60};
    m_x = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_xRect, 2018, 0);
    m_x->TB_Func_13("juicer\\lock\\sec_num_x.ddj",0,0);

    RECT m_cRect = {277,253,60,60};
    m_c = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_cRect, 2019, 0);
    m_c->TB_Func_13("juicer\\lock\\sec_num_c.ddj",0,0);

    RECT m_edit1Rect = {18,45,143,20};
    m_edit1 = (CIFEdit*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFEdit), m_edit1Rect, 2020, 0);
    m_edit1->TB_Func_13("interface\\mall\\mall_box_02.ddj", 0, 0);
    m_edit1->SetMaxLength(8);
    m_edit1->AddValue_404(0x80);
    m_edit1->SetStyleThingy(PASSWORD_MASKED);

    RECT m_edit2Rect = {18,85,143,20};
    m_edit2 = (CIFEdit*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFEdit), m_edit2Rect, 2021, 0);
    m_edit2->TB_Func_13("interface\\mall\\mall_box_02.ddj", 0, 0);
    m_edit2->SetMaxLength(8);
    m_edit2->AddValue_404(0x80);
    m_edit2->SetStyleThingy(PASSWORD_MASKED);

    RECT m_edit3Rect = {18,125,143,20};
    m_edit3 = (CIFEdit*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFEdit), m_edit3Rect, 2022, 0);
    m_edit3->TB_Func_13("interface\\mall\\mall_box_02.ddj", 0, 0);
    m_edit3->SetMaxLength(8);
    m_edit3->AddValue_404(0x80);
    m_edit3->SetStyleThingy(PASSWORD_MASKED);

    RECT m_btn1Rect = {168,125,98,22};
    m_btn1 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btn1Rect, 2023, 0);
    m_btn1->TB_Func_13("interface\\mall\\mall_button.ddj",0,0);
    m_btn1->SetText(L"Confirm");

    RECT m_infoRect = {18,162,236,149};
    m_info = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_infoRect, 2024, 0);
    m_info->m_FontTexture.sub_8B4400(0, 0);
    m_info->m_FontTexture.sub_8B4750(0);
    m_info->TB_Func_5(0);
    m_info->TB_Func_6(0);

    RECT m_btn2Rect = {278,349,96,24};
    m_btn2 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btn2Rect, 2025, 0);
    m_btn2->TB_Func_13("interface\\mall\\mall_button.ddj",0,0);
    m_btn2->SetText(L"Change");

    RECT m_btn3Rect = {382,349,96,24};
    m_btn3 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btn3Rect, 2026, 0);
    m_btn3->TB_Func_13("interface\\mall\\mall_button.ddj",0,0);
    m_btn3->SetText(L"Cancel");

    RECT m_cboxRect = {15,350,15,15};
    m_cbox = (CIFCheckBox*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFCheckBox), m_cboxRect , 2027, 0);

    RECT m_cboxtextRect = {36,351,216,11};
    m_cboxtext = (CIFStatic*) CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_cboxtextRect, 2028, 0);
    m_cboxtext->m_FontTexture.sub_8B4400(0, 0);
    m_cboxtext->m_FontTexture.sub_8B4750(0);
    m_cboxtext->TB_Func_5(0);
    m_cboxtext->TB_Func_6(0);

    srand((unsigned int)time(NULL));
    for(int i=0;i<10;i++)
    {
        int randomIndex = 0 + rand() % (( 9 + 1 ) - 0);
        SwapButtons(m_btns[i],m_btns[randomIndex]);
    }
    createpwMode();

    SetText(L"Secondary Passcode");
    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    return true;
}
void CIFAccLock::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
void CIFAccLock::OnUpdate() {

}
void CIFAccLock::OnBtnsClick() {

    int id = GetIDOfInterfaceUnderCursor();
    if(id == 2023)
    {
        if(m_edit3->IsVisible())
        {
            CMsgStreamBuffer buf(0x181F);
            buf << BYTE(0x01);
            buf << std::n_string(TO_NSTRING(text_holder1));
            buf << std::n_string(TO_NSTRING(text_holder2));
            buf << std::n_string(TO_NSTRING(text_holder3));
            buf << m_cbox->GetCheckedState_MAYBE();
            SendMsg(buf);
        }
        else if(m_edit2->IsVisible())
        {
            CMsgStreamBuffer buf(0x181F);
            buf << BYTE(0x00);
            buf << std::n_string(TO_NSTRING(text_holder1));
            buf << std::n_string(TO_NSTRING(text_holder2));
            SendMsg(buf);
        }
        else
        {
            CMsgStreamBuffer buf(0x181F);
            buf << BYTE(0x02);
            buf << std::n_string(TO_NSTRING(text_holder1));
            buf << m_cbox->GetCheckedState_MAYBE();
            SendMsg(buf);
        }
    }
    else if (id == 2025)
    {
        if(!m_edit2->IsVisible())
            changepwMode();
    }
    else if(id == 2026)
    {
        if(m_edit3->IsVisible())
            enterpwMode();
        else
            this->ShowGWnd(false);
    }
    else if(id == 2018)
    {
        srand((unsigned int)time(NULL));
        for(int i=0;i<10;i++)
        {
            int randomIndex = 0 + rand() % (( 9 + 1 ) - 0);
            SwapButtons(m_btns[i],m_btns[randomIndex]);
        }
        switch(focusedEditID)
        {
            case 0:
            {
                if(wcslen(text_holder1.c_str()) > 0) {
                    text_holder1 = text_holder1.erase(text_holder1.size() - 1, 1);
                    std::n_wstring text = m_edit1->GetNText();
                    text = text.erase(text.size() - 1, 1);
                    m_edit1->SetText(text.c_str());
                }
            }
            break;
            case 1:
            {
                if(wcslen(text_holder2.c_str()) > 0) {
                    text_holder2 = text_holder2.erase(text_holder2.size() - 1, 1);
                    std::n_wstring text = m_edit2->GetNText();
                    text = text.erase(text.size() - 1, 1);
                    m_edit2->SetText(text.c_str());
                }
            }
            break;
            case 2:
            {
                if(wcslen(text_holder3.c_str()) > 0) {

                    text_holder3 = text_holder3.erase(text_holder3.size() - 1, 1);
                    std::n_wstring text = m_edit3->GetNText();
                    text = text.erase(text.size() - 1, 1);
                    m_edit3->SetText(text.c_str());
                }
            }
            break;
        }
    }
    else if (id == 2019)
    {
        srand((unsigned int)time(NULL));
        for(int i=0;i<10;i++)
        {
            int randomIndex = 0 + rand() % (( 9 + 1 ) - 0);
            SwapButtons(m_btns[i],m_btns[randomIndex]);
        }
        switch(focusedEditID)
        {
            case 0:
                text_holder1 = L"";
                m_edit1->SetText(L"");
                break;
            case 1:
                text_holder2 = L"";
                m_edit2->SetText(L"");
                break;
            case 2:
                text_holder3 = L"";
                m_edit3->SetText(L"");
                break;
        }
    }
    else
    {
        srand((unsigned int)time(NULL));
        for(int i=0;i<10;i++)
        {
            int randomIndex = 0 + rand() % (( 9 + 1 ) - 0);
            SwapButtons(m_btns[i],m_btns[randomIndex]);
        }
        int value = id - 2007;
        wchar_t *str = new wchar_t[200];
        printf("%d\n\r",value);
        switch(focusedEditID)
        {
            case 0:
                if(wcslen(text_holder1.c_str()) < 8)
                {
                    wsprintfW(str, L"%s%d", text_holder1.c_str(), value);
                    text_holder1 = str;
                    wsprintfW(str, L"%s*", m_edit1->GetNText().c_str());
                    m_edit1->SetText(str);
                    m_edit1->SetFocus_MAYBE();
                }
                break;
            case 1:
                if(wcslen(text_holder2.c_str()) < 8)
                {
                    wsprintfW(str, L"%s%d", text_holder2.c_str(), value);
                    text_holder2 = str;
                    wsprintfW(str, L"%s*", m_edit2->GetNText().c_str());
                    m_edit2->SetText(str);
                    m_edit2->SetFocus_MAYBE();
                }
                break;
            case 2:
                if(wcslen(text_holder3.c_str()) < 8)
                {
                    wsprintfW(str, L"%s%d", text_holder3.c_str(), value);
                    text_holder3 = str;
                    wsprintfW(str, L"%s*", m_edit3->GetNText().c_str());
                    m_edit3->SetText(str);
                    m_edit3->SetFocus_MAYBE();
                }
                break;
        }
    }
}
void CIFAccLock::SwapButtons(CIFButton* btn1, CIFButton* btn2)
{
    int PosX1,PosX2;
    int PosY1,PosY2;

    PosX1 = btn1->GetPos().x;
    PosX2 = btn2->GetPos().x;

    PosY1 = btn1->GetPos().y;
    PosY2 = btn2->GetPos().y;

    btn1->MoveGWnd(PosX2,PosY2);
    btn2->MoveGWnd(PosX1,PosY1);
}
void CIFAccLock::changepwMode()
{
    m_edit2->ShowGWnd(true);
    m_edit3->ShowGWnd(true);
    m_btn1->MoveGWnd(this->GetPos().x + 168,this->GetPos().y +125);
    m_info->MoveGWnd(this->GetPos().x + 18,this->GetPos().y + 162);
    m_cboxtext->SetText(L"Reset all associated hwids");
    m_info->SetText(L"*Write the old passcode at the first box\n*Write the new passcode at the second box\n*Confirm the new password at the third\n*Choose whether to reset all hwids or not\n\n*Passcode must be between 6 to 8 \n characters at most.");
}
void CIFAccLock::createpwMode()
{
    m_edit3->ShowGWnd(false);
    m_btn1->MoveGWnd(this->GetPos().x + 168,this->GetPos().y +85);
    m_info->MoveGWnd(this->GetPos().x + 18,this->GetPos().y + 122);
    m_cboxtext->SetText(L"Don't ask again from this pc");
    m_info->SetText(L"*Write the passcode at the first box\nConfirm the passcode at the second one\n*Choose whether to save this pc or not\n\n*Passcode must be between 6 to 8\n characters at most.");
}
void CIFAccLock::enterpwMode()
{
    m_edit2->ShowGWnd(false);
    m_edit3->ShowGWnd(false);
    m_btn1->MoveGWnd(this->GetPos().x + 168,this->GetPos().y +45);
    m_info->MoveGWnd(this->GetPos().x + 18,this->GetPos().y + 82);
    m_info->SetText(L"*Enter the passcode you created\n*Passcode has to be between 6 to 8 digits\n*Choose whether to save this pc or not");

}