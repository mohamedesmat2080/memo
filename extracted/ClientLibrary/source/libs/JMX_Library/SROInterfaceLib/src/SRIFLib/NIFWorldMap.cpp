//
// Created by YUMBUL on 30.06.2024.
//

#include <map>
#include <GFX3DFunction/RStateMgr.h>
#include <GFX3DFunction/RTLoading.h>
#include <CustomData/CustomDataManager.h>
#include <SimpleViewer/VBDynamic.h>
#include <Menu/IFUniqueHistory.h>
#include <Menu/IFMenu.h>
#include <ctime>
#include <GFXMainFrame/Controler.h>
#include <CharacterDependentData.h>
#include <NavMesh/LocationInfo.h>
#include <BSLib/multibyte.h>
#include <iostream>
#include "NIFWorldMap.h"
#include "ICPlayer.h"
#include "GInterface.h"

GFX_MSGMAP* CNIFWorldMap::MessageMap(){
    static const GFX_MSGMAP_ENTRY skillBoardMessageEntries[] =
            {
                    /* {GFX_WM_COMMAND, 0, 14, 14, BSSig_u12, 0,
                             (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CIFSkillBoard::OnBtnClick))},
 */
                    {GFX_WM_COMMAND, 0, 13402, 13402, BSSig_u12, 0,
                            (GFX_PMSG)(static_cast< void (GFX_MSG_CALL CGWndBase::*)() >(&CNIFWorldMap::OnClickPingButton))},
                    // Diğer özel mesaj girişleri buraya eklenebilir
            };

    static GFX_MSGMAP newmap =
            {
                    reinterpret_cast<const GFX_MSGMAP *>(0x00d99074), skillBoardMessageEntries,
            };
    return &newmap;
}
void CNIFWorldMap::OnClickPingButton()
{
    const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
    if(partyData.bIsPartyMaster && partyData.bInParty)
    {
        CIFMenu *menu = g_pCGInterface->m_IRM.GetResObj<CIFMenu>(MainMenuID, 1);
        if(menu->CanSendPing)
        {
            menu->CanSendPing = false;
            g_Controler->SetCustomCursor(149);

        }
        else if(!menu->CanSendPing)
        {
            menu->CanSendPing = true;
            g_Controler->SetCustomCursor(166);
        }
    }
    else
    {
        g_pCGInterface->ShowMessage_Warning(L"Only party master can be use ping feature.");
    }
}

bool CNIFWorldMap::OnWMCreate(long ln) {
    bool x = reinterpret_cast<bool (__thiscall *)(CNIFWorldMap *,long)>(0x0060F340)(this,ln);

    wnd_rect sz;
    sz.size.width = 36;
    sz.size.height = 36;
    CIFButton* button = (CIFButton *) CIFWnd::CreateInstance(this, GFX_RUNTIME_CLASS(CIFButton), sz, 13402, 0);
    button->TB_Func_13("interface\\worldmap\\wmap_button_location.ddj", 1, 1);
    button->ShowGWnd(true);
    //todo interface\targetwindow\tw_icon_unique.ddj
    return x;
}
void CNIFWorldMap::Fun_0061a620()
{
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*)>(0x0061a620)(this);
}
int CNIFWorldMap::GetRegionTypeMaybe(unsigned short region,float x,float y,float z)
{
    int aa = reinterpret_cast<int(__thiscall*)(CNIFWorldMap*,short , float,float,float)>(0x00610d30)(this,region,x,y,z);
  //  printf("FUN_00610d30 %d %d %f %f %f\n", aa, region, x, y, z);
    return aa;
}
void CNIFWorldMap::FUN_006256e0(int p1)
{
    // printf("FUN_006256e0 %d \n", p1);
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, int)>(0x0060fc10)(this, p1);
}
void CNIFWorldMap::FUN_00622b60(int p1, int p2)
{
    printf("%d %d \n", p1, p2);
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, int, int)>(0x00622b60)(this, p1, p1);
}
int CNIFWorldMap::FUN_00614170(undefined4 p1, short p2, float p3, float p4, float p5, undefined4 p6)
{
    //printf("FUN_00614170 %d %d %f %f %f %d\n",p1, p2, p3, p4, p5,p6);
    return reinterpret_cast<int(__thiscall*)(CNIFWorldMap*, undefined4, short, float, float, float, undefined4)>(0x00614170)(this, p1, p2, p3, p4, p5, p6);
}

/// 0061a620 0 byte
/// 0060fc10 1 byte


void CNIFWorldMap::Fun_00622f00(undefined4 p1, undefined4 p2,undefined4  p3, undefined4  p4,undefined4  p5,undefined4 p6,undefined4 p7,undefined4 p8,
                                undefined4 p9, undefined4 p10, undefined4 p11){
    //printf("Fun_00622f00 %d %d %d %d %d %d %d %d %d %d %d\n",p1, p2, p3, p4, p5,p6, p7, p8, p9, p10, p11);
    reinterpret_cast<int(__thiscall*)(CNIFWorldMap*,undefined4, undefined4, undefined4,
                                      undefined4, undefined4, undefined4,undefined4, undefined4, undefined4, undefined4, undefined4)>
    (0x00622f00)(this, p1, p2, p3, p4, p5, p6, p7, p8, p9,p10, p11);
}
void CNIFWorldMap::FUN_00621c60(float p1, float p2)
{
    //printf("FUN_00621c60 %f %f\n", p1, p2);
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, float, float)>(0x00621c60)(this, p1, p1);
}
void CNIFWorldMap::FUN_00620550(int p1, int p2)
{
    //printf("FUN_00620550 %d %d\n", p1, p2);
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, int, int)>(0x00620550)(this, p1, p1);
}
void CNIFWorldMap::SetMapMode()
{
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*)>(0x0061ee80)(this);
}

void CNIFWorldMap::FUN_00624f40(CWorldMapGuideData* p1, undefined4 p2,undefined4 p3)
{
    //printf("FUN_00624f40 %p %d %d \n", p1, p2, p3);
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, CWorldMapGuideData*, undefined4,undefined4)>(0x00624f40)(this, p1, p2, p3);
}
void CNIFWorldMap::TEST(int p1)
{
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*, int)>(0x00610980)(this, p1);
}

void CNIFWorldMap::FUN_0061c5a0(int p1,int  p2)
{
    printf("%d %d \n", p1, p2);
    reinterpret_cast<void(__thiscall *) (CNIFWorldMap*, int, int)>(0x0061c5a0)(this, p1, p2);
}
void CNIFWorldMap::FUN_006217e0(int p1)
{
    printf("FUN_006217e0 %d \n", p1);
    reinterpret_cast<void(__thiscall *) (CNIFWorldMap*, int)>(0x006217e0)(this, p1);
}
int currentFrame = 0;
const int frameCount = 432 / 36; // 36x36 bölümlerine ayırdığımız için 12 kare
bool timerrunning = false;

void UpdateAnimation() {
    currentFrame = (currentFrame + 1) % frameCount;
}
void CNIFWorldMap::UpdateButtonPosition()
{
    const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
    if(partyData.bIsPartyMaster && partyData.bInParty)
    {
        if(!this->GetGuiFromList<CNIFButton>(13402)->IsVisible())
        {
            this->GetGuiFromList<CIFButton>(13402)->ShowGWnd(true);
        }
    }
    else
    {
        if(this->GetGuiFromList<CNIFButton>(13402)->IsVisible())
        {
            this->GetGuiFromList<CIFButton>(13402)->ShowGWnd(false);
        }
    }
    wnd_pos x = this->GetGuiFromList<CNIFButton>(20)->GetPos();
    if(this->GetGuiFromList<CNIFButton>(6)->IsVisible())
    {
        if(this->GetGuiFromList<CIFButton>(13402) != NULL)
        {
            this->GetGuiFromList<CIFButton>(13402)->MoveGWnd(x.x - 76, x.y);
        }
    }
    else
    {
        if(this->GetGuiFromList<CIFButton>(13402) != NULL)
        {
            this->GetGuiFromList<CIFButton>(13402)->MoveGWnd(x.x - 38, x.y);
        }
    }

}
void CNIFWorldMap::OnWMRenderMySelf() {
    reinterpret_cast<void(__thiscall*)(CNIFWorldMap*)>(0x0061BAC0)(this);
    UpdateButtonPosition();
    if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1) != NULL)
    {
        if (g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedRegionID != 0)
        {
            int REGIONID = g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedRegionID;
            int X = g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedX;

            int Z = g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedZ;
            //            int Y = g_pCGInterface->m_IRM.GetResObj<CIFUniqueHistory>(UniqueHistoryID, 1)->SelectedY;
            if (m_currentPos != NULL && m_currentPos != 0x0)
            {
                int currentTile_MAYBE = reinterpret_cast<int(__thiscall*) (int*)>(0x0096f4e0)(m_currentPos);

                float renderPosX = m_currentPosX +
                                   ((X / 10.0 + (float)(((REGIONID & 0xff) - 0x87) * 0xc0)) - (float)m_stateX) /
                                   m_stateY;
                float renderPosY = m_currentPosY +
                                   ((float)*(int*)(currentTile_MAYBE + 0x54) -
                                    ((Z / 10.0 + (float)(((REGIONID >> 8) - 0x5c) * 0xc0)) - (float)m_viewWidth) /
                                    m_viewHeight);

                D3DVECTOR dataOut[9];

                dataOut[0].x = renderPosX - 5;
                dataOut[0].y = renderPosY - 5;
                dataOut[0].z = 1.0;

                dataOut[2].x = dataOut[0].x + 16.0f;
                dataOut[2].y = dataOut[0].y;
                dataOut[2].z = 1.0;

                dataOut[4].x = dataOut[0].x + 16.0f;
                dataOut[4].y = renderPosY - 5 + 16.0f;
                dataOut[4].z = 1.0;

                dataOut[6].x = dataOut[0].x;
                dataOut[6].y = dataOut[4].y;
                dataOut[6].z = 1.0;

                dataOut[8].z = 0.0;

                dataOut[1].z = 0.0;
                dataOut[3].z = 0.0;
                dataOut[5].z = 1.0;
                dataOut[7].z = 1.0;

                dataOut[1].x = 0.1;
                dataOut[3].x = 0.1;
                dataOut[5].x = 0.1;
                dataOut[7].x = 0.1;

                dataOut[1].y = 0.0;
                dataOut[3].y = 1.0;
                dataOut[5].y = 1.0;
                dataOut[7].y = 0.0;


                g_RStateMgr.m_pDevice->SetFVF(260);
                g_RStateMgr.m_pDevice->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_CustomDataManager->MapIcon));
                g_RStateMgr.FUN_004700a0();
                g_RStateMgr.m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, &dataOut, 0x18);
            }

        }
    }

    if (g_pCGInterface->m_IRM.GetResObj<CIFMenu>(MainMenuID, 1) != NULL)
    {
        CIFMenu* menu = g_pCGInterface->m_IRM.GetResObj<CIFMenu>(MainMenuID, 1);
        if (menu->PingedRegionID)
        {
            if (!timerrunning) {
                timerrunning = true;
                this->StartTimer(1, 100); // Timer'ı başlat
            }

            // Harita verileri
            int REGIONID = menu->PingedRegionID;
            int X = menu->PingedPosX;

            int Z = menu->PingedPosZ;

            if(menu->MapType == 2)
            {
                if (m_currentPos != 0x0)
                {
                    int currentTile_MAYBE = reinterpret_cast<int(__thiscall*) (int*)>(0x0096f4e0)(m_currentPos);
                    if (currentTile_MAYBE == 0 || IsBadReadPtr((void*)currentTile_MAYBE, sizeof(int))) {
                        return;
                    }

                    float renderPosX = m_currentPosX - ((float)*(int*)(currentTile_MAYBE + 0x58)) * 0x20 + X;
                    float renderPosY = m_currentPosY + ((float)*(int*)(currentTile_MAYBE + 0x5c)) *  0x20 + Z;

                    D3DVECTOR dataOut[9];

                    dataOut[0].x = renderPosX - 15;
                    dataOut[0].y = renderPosY - 15;
                    dataOut[0].z = 1.0;

                    dataOut[2].x = dataOut[0].x + 36.0f;
                    dataOut[2].y = dataOut[0].y;
                    dataOut[2].z = 1.0;

                    dataOut[4].x = dataOut[0].x + 36.0f;
                    dataOut[4].y = renderPosY - 15 + 36.0f;
                    dataOut[4].z = 1.0;

                    dataOut[6].x = dataOut[0].x;
                    dataOut[6].y = dataOut[4].y;
                    dataOut[6].z = 1.0;

                    dataOut[8].z = 0.0;

                    dataOut[1].z = 0.0;
                    dataOut[3].z = 0.0;
                    dataOut[5].z = 1.0;
                    dataOut[7].z = 1.0;

                    dataOut[1].x = 0.1;
                    dataOut[3].x = 0.1;
                    dataOut[5].x = 0.1;
                    dataOut[7].x = 0.1;

                    dataOut[1].y = 0.0;
                    dataOut[3].y = 1.0;
                    dataOut[5].y = 1.0;
                    dataOut[7].y = 0.0;

                    // 432x36 boyutundaki dokunun 36x36 parçasını animasyonlu olarak render etme
                    const int textureWidth = 432;
                    const int textureHeight = 36;
                    const int subTextureWidth = 36;
                    const int subTextureHeight = 36;
                    const int numSubTextures = textureWidth / subTextureWidth; // 12 kare

                    // Kaynak dikdörtgeni (source rectangle) ayarla
                    RECT srcRect;
                    srcRect.left = currentFrame * subTextureWidth;
                    srcRect.right = srcRect.left + subTextureWidth;
                    srcRect.top = 0;
                    srcRect.bottom = subTextureHeight;

                    // Kaynak koordinatlarını ayarla
                    float u1 = static_cast<float>(srcRect.left) / textureWidth;
                    float v1 = static_cast<float>(srcRect.top) / textureHeight;
                    float u2 = static_cast<float>(srcRect.right) / textureWidth;
                    float v2 = static_cast<float>(srcRect.bottom) / textureHeight;

                    // Vertex yapısı tanımı
                    struct Vertex
                    {
                        float x, y, z, rhw; // Pozisyon verileri
                        float u, v;         // Doku koordinatları (texture coordinates)
                    };

                    Vertex vertices[4] =
                            {
                                    { dataOut[0].x, dataOut[0].y, 0, 1, u1, v1 },
                                    { dataOut[2].x, dataOut[2].y, 0, 1, u2, v1 },
                                    { dataOut[4].x, dataOut[4].y, 0, 1, u2, v2 },
                                    { dataOut[6].x, dataOut[6].y, 0, 1, u1, v2 }
                            };

                    // FVF ve doku ayarlarını yap
                    g_RStateMgr.m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
                    g_RStateMgr.m_pDevice->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_CustomDataManager->PingIcon));

                    // Özel işlev çağrısı (sizin kodunuzda: FUN_004700a0)
                    g_RStateMgr.FUN_004700a0();

                    // Primitif çizim komutu
                    g_RStateMgr.m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
                }
            }
            else
            {
                if (m_currentPos != 0x0)
                {
                    int currentTile_MAYBE = reinterpret_cast<int(__thiscall*) (int*)>(0x0096f4e0)(m_currentPos);
                    if (currentTile_MAYBE == 0 || IsBadReadPtr((void*)currentTile_MAYBE, sizeof(int))) {
                        return;
                    }

                    float renderPosX = m_currentPosX + ((X / 10.0 + (float)(((REGIONID & 0xff) - 0x87) * 0xc0)) - (float)m_stateX) / m_stateY;

                    float renderPosY = m_currentPosY + ((float)*(int*)(currentTile_MAYBE + 0x54) - ((Z / 10.0 + (float)(((REGIONID >> 8) - 0x5c) * 0xc0))
                                                                                                    - (float)m_viewWidth) / m_viewHeight);

                    D3DVECTOR dataOut[9];

                    dataOut[0].x = renderPosX - 15;
                    dataOut[0].y = renderPosY - 15;
                    dataOut[0].z = 1.0;

                    dataOut[2].x = dataOut[0].x + 36.0f;
                    dataOut[2].y = dataOut[0].y;
                    dataOut[2].z = 1.0;

                    dataOut[4].x = dataOut[0].x + 36.0f;
                    dataOut[4].y = renderPosY - 15 + 36.0f;
                    dataOut[4].z = 1.0;

                    dataOut[6].x = dataOut[0].x;
                    dataOut[6].y = dataOut[4].y;
                    dataOut[6].z = 1.0;

                    dataOut[8].z = 0.0;

                    dataOut[1].z = 0.0;
                    dataOut[3].z = 0.0;
                    dataOut[5].z = 1.0;
                    dataOut[7].z = 1.0;

                    dataOut[1].x = 0.1;
                    dataOut[3].x = 0.1;
                    dataOut[5].x = 0.1;
                    dataOut[7].x = 0.1;

                    dataOut[1].y = 0.0;
                    dataOut[3].y = 1.0;
                    dataOut[5].y = 1.0;
                    dataOut[7].y = 0.0;

                    // 432x36 boyutundaki dokunun 36x36 parçasını animasyonlu olarak render etme
                    const int textureWidth = 432;
                    const int textureHeight = 36;
                    const int subTextureWidth = 36;
                    const int subTextureHeight = 36;
                    const int numSubTextures = textureWidth / subTextureWidth; // 12 kare

                    // Kaynak dikdörtgeni (source rectangle) ayarla
                    RECT srcRect;
                    srcRect.left = currentFrame * subTextureWidth;
                    srcRect.right = srcRect.left + subTextureWidth;
                    srcRect.top = 0;
                    srcRect.bottom = subTextureHeight;

                    // Kaynak koordinatlarını ayarla
                    float u1 = static_cast<float>(srcRect.left) / textureWidth;
                    float v1 = static_cast<float>(srcRect.top) / textureHeight;
                    float u2 = static_cast<float>(srcRect.right) / textureWidth;
                    float v2 = static_cast<float>(srcRect.bottom) / textureHeight;

                    // Vertex yapısı tanımı
                    struct Vertex
                    {
                        float x, y, z, rhw; // Pozisyon verileri
                        float u, v;         // Doku koordinatları (texture coordinates)
                    };

                    Vertex vertices[4] =
                            {
                                    { dataOut[0].x, dataOut[0].y, 0, 1, u1, v1 },
                                    { dataOut[2].x, dataOut[2].y, 0, 1, u2, v1 },
                                    { dataOut[4].x, dataOut[4].y, 0, 1, u2, v2 },
                                    { dataOut[6].x, dataOut[6].y, 0, 1, u1, v2 }
                            };

                    // FVF ve doku ayarlarını yap
                    g_RStateMgr.m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
                    g_RStateMgr.m_pDevice->SetTexture(0, static_cast<IDirect3DBaseTexture9*>(m_CustomDataManager->PingIcon));

                    // Özel işlev çağrısı (sizin kodunuzda: FUN_004700a0)
                    g_RStateMgr.FUN_004700a0();

                    // Primitif çizim komutu
                    g_RStateMgr.m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
                }
            }

        }
    }
}
void CNIFWorldMap::OnTimerIMPL(int timerId) {
    if (timerrunning) {
        this->KillTimer(timerId);
        timerrunning = false;
        UpdateAnimation(); // Animasyonu güncelle
    }
    reinterpret_cast<void *(__thiscall *)(CNIFWorldMap *, int)>(0x006191d0)(this, timerId);
}

#include <cmath> // cmath kütüphanesini ekle

bool CNIFWorldMap::OnMouseActions(Event3D* mouseData) {
    bool result = reinterpret_cast<bool(__thiscall *)(CNIFWorldMap *, Event3D*)>(0x00623220)(this, mouseData);
    if (mouseData->Msg == WM_LBUTTONUP) {
        g_Controler->SetCustomCursor(149);
        CIFMenu *menu = g_pCGInterface->m_IRM.GetResObj<CIFMenu>(MainMenuID, 1);
        if(menu->CanSendPing)
        {
            menu->CanSendPing = false;
            if (m_currentPos != 0x0 && m_currentPos != NULL) {
                int currentTile_MAYBE = reinterpret_cast<int (__thiscall *)(int *)>(0x0096f4e0)(m_currentPos);
                if (currentTile_MAYBE == 0 || IsBadReadPtr((void *)currentTile_MAYBE, sizeof(int))) {
                    return false;
                }

                // Mouse tıklama pozisyonlarını al
                float clickPosX = mouseData->lParam;
                float clickPosY = mouseData->wParam;
                //std::cout << "Mouse Position: " << clickPosX << ", " << clickPosY << std::endl;

                // Harita pozisyonlarını hesapla
                float mapPosX = ((clickPosX - m_currentPosX) * m_stateY) + static_cast<float>(m_stateX);
                float mapPosY = ((-clickPosY + m_currentPosY + static_cast<float>(*(int *)(currentTile_MAYBE + 0x54))) * m_viewHeight) + static_cast<float>(m_viewWidth);
                //std::cout << "Calculated mapPosX: " << mapPosX << ", mapPosY: " << mapPosY << std::endl;

                // X ve Y sektörlerini hesapla
                int xSector = static_cast<int>(std::floor((mapPosX / 192.0) + 135.0));
                int ySector = static_cast<int>(std::floor((mapPosY / 192.0) + 92.0));

                // X ve Y ofsetlerini hesapla
                int xOffset = static_cast<int>(std::floor((((mapPosX / 192.0) - (xSector - 135.0)) * 192.0) * 10.0));
                int yOffset = static_cast<int>(std::floor((((mapPosY / 192.0) - (ySector - 92.0)) * 192.0) * 10.0));

                int pingedRegionID = (ySector << 8) | xSector;

                bool isDungeon = (pingedRegionID > SHRT_MAX);

             /*   menu->PingedRegionID = pingedRegionID;
                menu->PingedPosX = xOffset;
                menu->PingedPosY = yOffset;
                menu->PingedPosZ = yOffset;
*/
                const SPartyData& partyData = g_CCharacterDependentData.GetPartyData();
                CMsgStreamBuffer buf(0x169A);
                buf << (BYTE)19;
                buf << pingedRegionID;
                buf << xOffset;
                buf << yOffset;
                buf << yOffset;
                buf << Is0OutSide2IsInside;
                buf << Normal0Town1Dungeon2;

                buf << (BYTE)(partyData.NumberOfMembers);
                for (int i = 0; i < partyData.NumberOfMembers; ++i)
                {
                    const SPartyMemberData& memberData = g_CCharacterDependentData.GetPartyMemberData(i);
                    buf << std::n_string(TO_NSTRING(memberData.m_charactername));
                }
                SendMsg(buf);

            }
        }
    }
    return result;
}





undefined1 CNIFWorldMap::OnCloseWndIMPL(){
    CIFMenu *menu = g_pCGInterface->m_IRM.GetResObj<CIFMenu>(MainMenuID, 1);
    if(menu->CanSendPing)
    {
        menu->CanSendPing = false;
        g_Controler->SetCustomCursor(149);
    }
    return reinterpret_cast<undefined1 (__thiscall *)(CNIFWorldMap *)>(0x0046d7c0)(this);
}
