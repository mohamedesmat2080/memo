#include "IFItemChestSlot.h"
#include "../../../../../DevKit_DLL/src/Util.h"
#include "IFItemChest.h"
#include <GInterface.h>
#include <GlobalDataManager.h>
#include <TextStringManager.h>

GFX_IMPLEMENT_DYNCREATE(CIFItemChestSlot, CIFWnd)
GFX_BEGIN_MESSAGE_MAP(CIFItemChestSlot, CIFWnd)
            ONG_COMMAND(7, &CIFItemChestSlot::OnTake)
GFX_END_MESSAGE_MAP()
CIFItemChestSlot::CIFItemChestSlot(void)
{
}
CIFItemChestSlot::~CIFItemChestSlot(void)
{
}
bool CIFItemChestSlot::OnCreate(long ln)
{
    // Populate inherited members
    CIFWnd::OnCreate(ln);
    RECT m_itemTextRect = { 30,9,220,13 };
    m_itemText = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_itemTextRect, 3, 0);

    RECT m_fromTextRect = { 220,9,140,17 };
    m_fromText = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_fromTextRect, 4, 0);

    RECT m_countTextRect = { 360,9,76,13 };
    m_countText = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_countTextRect, 5, 0);

    RECT m_dateTextRect = { 437,9,150,13 };
    m_dateText = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), m_dateTextRect, 6, 0);

    RECT m_takeBtnRect = { 593,5,65,20 };
    m_takeBtn = (CIFButton*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), m_takeBtnRect, 7, 0);
    m_takeBtn->SetText(L"Take");
    m_takeBtn->TB_Func_13("interface\\mall\\mall_button.ddj", 0, 0);
    m_takeBtn->ShowGWnd(false);

    RECT m_itemInfoRect = { 18,2,25,25 };
    m_itemInfo = (CIFSlotWithHelp*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFSlotWithHelp), m_itemInfoRect, 8, 0);
    m_itemInfo->ShowGWnd(false);


    RECT r_edgeRect = { this->GetSize().width,0,4,this->GetSize().height };
    r_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), r_edgeRect, 1, 0);
    r_edge->TB_Func_13("interface\\ifcommon\\com_bar01_right.ddj",0,0);

    RECT l_edgeRect = { 0,0,4,this->GetSize().height };
    l_edge = (CIFStatic*)CGWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFStatic), l_edgeRect, 2, 0);
    l_edge->TB_Func_13("interface\\ifcommon\\com_bar01_left.ddj",0,0);
    TB_Func_13("interface\\ifcommon\\com_bar01_mid.ddj",0,0);
    return true;
}
void CIFItemChestSlot::OnUpdate() {
}
void CIFItemChestSlot::FillTheSlot(int itemID,int itemCount)
{
    const CItemData::SData *data = &g_CGlobalDataManager->GetItemData(itemID);
    if(data != NULL)
    {
        m_itemInfo->TB_Func_13(data->m_assocFileIcon128.c_str(),0,0);
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
            m_itemInfo->ItemInfo = tempItemInfo;
            m_itemInfo->ItemInfo->m_quantity = itemCount;
            m_itemInfo->SetType(70);
            m_itemInfo->ShowGWnd(true);
        }
    }
}

void CIFItemChestSlot::OnTake()
{
    CIFItemChest* chestWnd = (CIFItemChest *) g_pCGInterface->GetWindowByUniqueID(13395);

    CMsgStreamBuffer buf(0x180F);
    buf << byte(0x01) << byte(chestWnd->m_spin->index) << (int)this->m_takeBtn->getTag();
    SendMsg(buf);
}