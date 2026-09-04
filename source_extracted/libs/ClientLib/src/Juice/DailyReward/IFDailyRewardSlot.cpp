#include "IFDailyRewardSlot.h"
#include <Game.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFDailyRewardSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFDailyRewardSlot, CIFWnd)
            ONG_COMMAND(5, &CIFDailyRewardSlot::OnTake)
GFX_END_MESSAGE_MAP()
CIFDailyRewardSlot::CIFDailyRewardSlot(void)
{
}
CIFDailyRewardSlot::~CIFDailyRewardSlot(void)
{
}
bool CIFDailyRewardSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_tileRect = { 0,0,229,243 };
    m_tile = (CIFNormalTile*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFNormalTile), m_tileRect, 0, 0);
    m_tile->TB_Func_13("interface\\ifcommon\\bg_tile\\com_bg_tile_p.ddj", 0, 0);

    RECT m_frameRect = {0,0,229,243};
    m_frame = (CIFFrame*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFFrame), m_frameRect, 1, 0);
    m_frame->TB_Func_13("interface\\frame\\mall_sub_wnd04_", 0, 0);

    RECT m_slotRect = {72,20,70,70};
    m_slot = (CIFSlotWithHelp*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSlotWithHelp), m_slotRect, 2, 0);
    m_slot->TB_Func_13("interface\\ifcommon\\com_kindred_china.ddj",0,0);

    RECT m_NameRect = {12,122,200,10};
    m_Name = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_NameRect, 3, 0);

    RECT m_CountRect = {62,160,100,10};
    m_Count = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_CountRect, 4, 0);

    RECT m_btnRect = {24,210,177,24};
    m_btn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_btnRect, 5, 0);
    m_btn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_btn->SetEnabledState(false);

    RECT m_dayRect = {-10,-6,77,77};
    m_day = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_dayRect, 6, 0);

    return true;
}
void CIFDailyRewardSlot::OnUpdate() {

}
void CIFDailyRewardSlot::FillTheSlot(int itemID,int itemCount)
{
    const CItemData::SData *data = &g_CGlobalDataManager->GetItemData(itemID);
    if(data != NULL)
    {
        m_slot->TB_Func_13(data->m_assocFileIcon128.c_str(),0,0);
        std::wstring NameStrID128 = g_CTextStringManager->GetStringByNameStrID(
                data->m_nameStrId128)
            ->c_str();
        m_Name->SetText(NameStrID128.c_str());

        CMsgStreamBuffer buf(0xB034);
        buf << INT32(0) << INT32(itemID);
        u_short typeID2 = data->m_typeId.getTypeID2();
        u_short typeID3 = data->m_typeId.getTypeID3();
        u_short typeID4 = data->m_typeId.getTypeID4();
        switch(typeID2)
        {
            case 1:
                buf << UINT8(0) << UINT64(0) << UINT32(1) <<UINT8(0) << UINT8(1) << UINT8(0) << UINT8(2) << UINT8(0);
                break;
            case 2:
                switch (typeID3)
                {
                    case 1:
                        buf << UINT8(0x01);
                        break;
                    case 2:
                        buf << UINT32(0x00);
                        break;
                    default:
                        if (typeID4 == 3) //Magic cube
                            buf << UINT32(0x01);
                        break;
                }
                break;
            case 3:
                buf << UINT16(0x01);
                if(typeID3 == 11)
                    if(typeID4 == 1 || typeID4 == 2)
                        buf << UINT8(0x00);
                    else if(typeID3 == 14 && typeID4 == 2)
                        buf << UINT8(0x00);
                break;
        }

        CSOItem* tempItemInfo = new CSOItem();
        tempItemInfo->ReadFromPacket(&buf,1);
        tempItemInfo->SetEnabled(true);
        if(tempItemInfo)
        {
            m_slot->ItemInfo = tempItemInfo;
            m_slot->ItemInfo->m_quantity = itemCount;
            m_slot->SetType(70);
            m_slot->ShowGWnd(true);
        }
    }
}
void CIFDailyRewardSlot::OnTake()
{
    CMsgStreamBuffer buf(0x181D);
    buf << byte(0x01) << this->index;
    SendMsg(buf);

    m_btn->SetEnabledState(false);
    m_btn->SetText(L"Taken");
}