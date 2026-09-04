#include "IFCustomsMarket.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include <BSLib/multibyte.h>
#include <GlobalHelpersThatHaveNoHomeYet.h>

GFX_IMPLEMENT_DYNCREATE(CIFCustomsMarket, CIFMainFrame)
GFX_BEGIN_MESSAGE_MAP(CIFCustomsMarket, CIFMainFrame)
            ONG_COMMAND(14, &CIFCustomsMarket::OnBuyTitle)
            ONG_COMMAND(15, &CIFCustomsMarket::OnBuyTitleColor)
            ONG_COMMAND(16, &CIFCustomsMarket::OnBuyNameColor)

GFX_END_MESSAGE_MAP()
CIFCustomsMarket::CIFCustomsMarket(void)
{
}
CIFCustomsMarket::~CIFCustomsMarket(void)
{
}
bool CIFCustomsMarket::OnCreate(long ln)
{
    this->showColorPicker = true; //blocking the color picker from being rendered.

    CIFMainFrame::OnCreate(ln);

    RECT m_tile2Rect = { 20,40,775,310 };
    m_tile2 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile2Rect, 2, 0);
    m_tile2->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_tileRect = { 15,42,250,300 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 21, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frameRect = { 15,42,250,300 };
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 3, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_head1Rect = { 11,42,262,24 };
    m_head1 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head1Rect, 4, 0);
    m_head1->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head1->SetText(L"Purchase Custom Title");

    RECT m_tile3Rect = { 280,42,250,300 };
    m_tile3 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile3Rect, 5, 0);
    m_tile3->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frame2Rect = { 280,42,250,300 };
    m_frame2 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame2Rect, 6, 0);
    m_frame2->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_head2Rect = { 276,42,262,24 };
    m_head2 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head2Rect, 7, 0);
    m_head2->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head2->SetText(L"Purchase Custom Title Color");

    RECT m_tile4Rect = { 545,42,250,300 };
    m_tile4 = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tile4Rect, 8, 0);
    m_tile4->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_m.ddj", 0, 0);

    RECT m_frame3Rect = { 545,42,250,300 };
    m_frame3 = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frame3Rect, 9, 0);
    m_frame3->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_head3Rect = { 541,42,262,24 };
    m_head3 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_head3Rect, 10, 0);
    m_head3->TB_Func_13("interface\\mall\\mall_subj_03.ddj", 0, 0);
    m_head3->SetText(L"Purchase Custom Name Color");

    RECT m_fee1Rect = { 77,78,107,22 };
    m_fee1 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_fee1Rect, 11, 0);
    m_fee1->SetText(L"<fee 500 silk>");
    m_fee1->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 255, 0, 0));

    RECT m_fee2Rect = { 346,78,107,22 };
    m_fee2 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_fee2Rect, 12, 0);
    m_fee2->SetText(L"<fee 500 silk>");
    m_fee2->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 255, 0, 0));

    RECT m_fee3Rect = { 615,78,107,22 };
    m_fee3 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_fee3Rect, 13, 0);
    m_fee3->SetText(L"<fee 500 silk>");
    m_fee3->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 255, 0, 0));

    RECT m_buy1Rect = { 84,310,100,24 };
    m_buy1 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_buy1Rect, 14, 0);
    m_buy1->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_buy1->SetText(L"Buy Now");

    RECT m_buy2Rect = { 357,310,100,24 };
    m_buy2 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_buy2Rect, 15, 0);
    m_buy2->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_buy2->SetText(L"Buy Now");

    RECT m_buy3Rect = { 629,310,100,24 };
    m_buy3 = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_buy3Rect, 16, 0);
    m_buy3->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_buy3->SetText(L"Buy Now");

    RECT m_info1Rect = { 20,193,100,24 };
    m_info1 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_info1Rect, 17, 0);
    m_info1->SetText(L"Title:");
    m_info1->m_FontTexture.SetColor(D3DCOLOR_RGBA(255, 249, 192, 45));

    RECT m_editRect = { 95,196,110,22 };
    m_edit = (CIFEdit*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFEdit), m_editRect, 18, 0);
    m_edit->TB_Func_13("interface\\mall\\mall_box_02.ddj", 0, 0);
    m_edit->SetMaxLength(20);

    RECT m_example1Rect = { 290,319,50,10 };
    m_example1 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_example1Rect, 19, 0);
    m_example1->SetText(L"<Example>");

    RECT m_example2Rect = { 555,319,50,10 };
    m_example2 = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_example2Rect, 20, 0);
    m_example2->SetText(L"<Example>");

    TB_Func_13("interface\\frame\\mall_ms_wnd_", 0, 0);
    this->MoveToCenter();
    this->ShowGWnd(false);
    this->showColorPicker = false;
    SetText(L"Titles Market");
    return true;
}
void CIFCustomsMarket::MoveToCenter()
{
    int PosX = 0, PosY = 0;
    PosY = (CLIENT_SCREEN_HEIGHT/2) - (this->GetSize().height/2);
    PosX = (CLIENT_SCREEN_WIDTH/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
}
void CIFCustomsMarket::OnUpdate() {

}
void CIFCustomsMarket::OnBuyTitle()
{
    m_buy1->SetEnabledState(false);

    CMsgStreamBuffer buf(0x181C);
    buf << BYTE(0x00) << std::n_string(TO_NSTRING(m_edit->GetNText()));
    SendMsg(buf);
}
void CIFCustomsMarket::OnBuyTitleColor()
{
    m_buy2->SetEnabledState(false);

    CMsgStreamBuffer buf(0x181C);
    buf << BYTE(0x01) << int(colorPicker1);
    SendMsg(buf);
}
void CIFCustomsMarket::OnBuyNameColor()
{
    m_buy3->SetEnabledState(false);

    CMsgStreamBuffer buf(0x181C);
    buf << BYTE(0x02) << int(colorPicker2);
    SendMsg(buf);
}