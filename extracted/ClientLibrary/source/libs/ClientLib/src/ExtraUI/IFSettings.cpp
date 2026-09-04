//
// Created by YUMBUL on 6.01.2025.
//

#include "IFSettings.h"
#include "IFMSFPS.h"

#include <Game.h>
#include <BSLib/Debug.h>
#include <GInterface.h>
#include <BSLib/multibyte.h>
#include <CustomData/CustomSettingManager.h>
#include <IFRenderStatic.h>
#include <ICUser.h>
#include <GlobalItemLinking/MemoryHelper.h>
#include <support/hook.h>
#include <Menu/IFGrantName.h>



GFX_IMPLEMENT_DYNCREATE(CIFSettings, CIFMainFrame)

GFX_BEGIN_MESSAGE_MAP(CIFSettings, CIFMainFrame)
                     ONG_COMMAND(11, &ClickSaveButton)
                     ONG_COMMAND(12, &ClickCancel)
                     //ONG_COMMAND(33, &ClickFPSCheckBox)
//                    ONG_COMMAND(1000, &On_BtnClick)

GFX_END_MESSAGE_MAP()

CIFSettings::CIFSettings(void){
    HideCharInfoSetting = false;
    RememberPCSetting = false;
}
CIFSettings::~CIFSettings(void){

}
bool CIFSettings::OnCreate(long ln)
{

    // Populate inherited members
    CIFMainFrame::OnCreate(ln);

    m_IRM.LoadFromFile("clientlibrary\\resinfo\\ifsettings.txt");
    m_IRM.CreateInterfaceSection("Create", this);

    this->SetText(L"Settings");
    this->m_IRM.GetResObj<CIFButton>(11, 1)->SetText(L"Save");
    this->m_IRM.GetResObj<CIFButton>(12, 1)->SetText(L"Cancel");
    this->m_IRM.GetResObj(32, 1)->SetText(L"Show Fps/Ping Window");
    this->m_IRM.GetResObj(35, 1)->SetText(L"Enable Infinity Zoom");
    this->m_IRM.GetResObj(38, 1)->SetText(L"Always Active Window");
    this->m_IRM.GetResObj(41, 1)->SetText(L"Hide item informations");

    std::n_wstring Text = L"Secondary Password (Remember PC)";
    std::n_wstring newText = KisaYazi(Text, 24);
    this->m_IRM.GetResObj(44, 1)->SetText(newText.c_str());
    this->m_IRM.GetResObj(44, 1)->SetTooltip(Text);
    this->m_IRM.GetResObj(44, 1)->SetStyleThingy(TOOLTIP);


    EnableShowFps = m_IRM.GetResObj<CIFCheckBox>(33, 1);
    EnableShowFps->SetCheckBoxState(false);
    EnableZoom = m_IRM.GetResObj<CIFCheckBox>(36, 1);
    EnableZoom->SetCheckBoxState(false);

    EnableActiveWnd = m_IRM.GetResObj<CIFCheckBox>(39, 1);
    EnableActiveWnd->SetCheckBoxState(false);

    HideCharInfo = m_IRM.GetResObj<CIFCheckBox>(42, 1);
    HideCharInfo->SetCheckBoxState(false);

    RememberPC = m_IRM.GetResObj<CIFCheckBox>(45, 1);
    RememberPC->SetCheckBoxState(false);


    UpdateMenuSize();
    this->ShowGWnd(false);
    return true;
}
#define GDR_RESULT_BUTTONTIMER 13134
void CIFSettings::OpenButton(int timeoutSeconds) {
    this->m_IRM.GetResObj<CIFButton>(11, 1)->SetEnabledState(0);
    this->StartTimer(GDR_RESULT_BUTTONTIMER, timeoutSeconds);
}

void CIFSettings::OnTimer(int timerId) {
    if (timerId == GDR_RESULT_BUTTONTIMER) {
        this->KillTimer(GDR_RESULT_BUTTONTIMER);
        this->m_IRM.GetResObj<CIFButton>(11, 1)->SetEnabledState(1);
    }
}
void CIFSettings::UpdateMenuSize()
{
    HideCharInfo->SetCheckBoxState(HideCharInfoSetting);
    RememberPC->SetCheckBoxState(RememberPCSetting);

    int PosX = 0, PosY = 0;
    PosY = (g_CGame->GetRes().res->height/2) - (this->GetSize().height/2);
    PosX = (g_CGame->GetRes().res->width/2) - (this->GetSize().width/2);
    this->MoveGWnd(PosX, PosY);
    BringToFront();
}
void FreezeOFF()
{
    g_MemoryHelper->UnProtect(reinterpret_cast<void*>(0x00ba6fc8), 10);
    *(BYTE*)(0x00BA6FC8) = 0xc7;
    *(BYTE*)(0x00BA6FC8 + 1) = 0x05;
    *(BYTE*)(0x00BA6FC8 + 2) = 0xb4;
    *(BYTE*)(0x00BA6FC8 + 3) = 0x7c;
    *(BYTE*)(0x00BA6FC8 + 4) = 0xed;
    *(BYTE*)(0x00BA6FC8 + 5) = 0x00;
    *(BYTE*)(0x00BA6FC8 + 6) = 0x00;
    *(BYTE*)(0x00BA6FC8 + 7) = 0x00;
    *(BYTE*)(0x00BA6FC8 + 8) = 0x00;
    *(BYTE*)(0x00BA6FC8 + 9) = 0x00;
    g_MemoryHelper->ReProtect();
}
void FreezeON()
{

    g_MemoryHelper->UnProtect(reinterpret_cast<void*>(0x00ba6fc8), 10);
    RenderNop((void*)0x00BA6FC8, 10);
    g_MemoryHelper->ReProtect();
}
void ZoomHackON()
{
    g_MemoryHelper->UnProtect(reinterpret_cast<void*>(0x0077B4F6), 2);
    *(BYTE*)(0x0077B4F6) = 0xEB;
    *(BYTE*)(0x0077B4F6 + 1) = 0x08;
    g_MemoryHelper->ReProtect();
}

void ZoomHackOFF()
{
    g_MemoryHelper->UnProtect(reinterpret_cast<void*>(0x0077B4F6), 2);
    *(BYTE*)(0x0077B4F6) = 0x7a;
    *(BYTE*)(0x0077B4F6 + 1) = 0x08;
    g_MemoryHelper->ReProtect();
}
void BackgroundLimit(int addr, float value)
{
    DWORD dwProtect;

    if (!VirtualProtect((LPVOID)addr, sizeof(float), PAGE_EXECUTE_READWRITE, &dwProtect)) {
        perror("Failed to unprotect memory\n");
        return;
    }
    *((float*)addr) = value;

    DWORD otherProtect;
    if (!VirtualProtect((LPVOID)addr, sizeof(float), dwProtect, &otherProtect)) {
        perror("Failed to restore protection on memory");
    }
}
void CIFSettings::ClickFPSCheckBox()
{
    if(EnableShowFps->GetCheckedState_MAYBE())
    {
        //EnableShowFps->SetCheckBoxState(false);
        if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1) != NULL)
        {
            if(!g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->IsVisible())
            {
                g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->UpdateMenuSize();
                g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->ShowGWnd(true);

            }
        }
    }
    else
    {
      //  EnableShowFps->SetCheckBoxState(true);
            if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1) != NULL)
            {
                if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->IsVisible())
                {
                    g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->ShowGWnd(false);
                }
            }

    }
    if(EnableZoom->GetCheckedState_MAYBE())
    {
        ZoomHackON();
        wchar_t message[] = L"Infinity Zoom : ON";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    else
    {
        ZoomHackOFF();
        wchar_t message[] = L"Infinity Zoom : OFF";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }

    if(EnableActiveWnd->GetCheckedState_MAYBE())
    {
        FreezeON();
        wchar_t message[] = L"Always Active Window : ON";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    else
    {
        FreezeOFF();
        wchar_t message[] = L"Always Active Window : OFF";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }


   /* if(BgLimit->GetCheckedState_MAYBE())
    {
        BackgroundLimit(0x00de34c0, 10000);
        wchar_t message[] = L"Extend Background Limit : ON";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    else
    {
        BackgroundLimit(0x00de34c0, 2500);
        wchar_t message[] = L"Extend Background Limit : OFF";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }*/
}
void CIFSettings::OnUpdate() {

}
void CIFSettings::ClickCancel()
{
    this->OnCloseWnd();
}
int CIFSettings::OnMouseMove(int a1, int x, int y)
{

        if(g_CurrentIfUnderCursor->IsSame(GFX_RUNTIME_CLASS(CIFStatic)))
        {
            CIFStatic* statica = (CIFStatic*)g_CurrentIfUnderCursor->GetRuntimeClass();
            if(statica != NULL)
            {
                if(statica->UniqueID() == 44)
                {
                    if(!g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
                    {
                        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(true);
                    }
                }
                else
                {
                    if(g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
                    {
                        g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
                    }
                }
            }
            else
            {
                if(g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
                {
                    g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
                }
            }
        }
        else
        {
            if(g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->IsVisible())
            {
                g_pCGInterface->m_IRM.GetResObj<CIFGrantName>(GrantNameID, 1)->ShowGWnd(false);
            }
        }
return 0;
}
void CIFSettings::ClickSaveButton()
{
    OpenButton(5000);
    if(EnableShowFps->GetCheckedState_MAYBE())
    {
        //EnableShowFps->SetCheckBoxState(false);
        if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1) != NULL)
        {
            if(!g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->IsVisible())
            {
                g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->UpdateMenuSize();
                g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->ShowGWnd(true);
            }
        }
    }
    else
    {
        //  EnableShowFps->SetCheckBoxState(true);
        if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1) != NULL)
        {
            if(g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->IsVisible())
            {
                g_pCGInterface->m_IRM.GetResObj<CIFMSFPS>(1952, 1)->ShowGWnd(false);
            }
        }

    }
    if(EnableZoom->GetCheckedState_MAYBE())
    {
        ZoomHackON();
        wchar_t message[] = L"Infinity Zoom : ON";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    else
    {
        ZoomHackOFF();
        wchar_t message[] = L"Infinity Zoom : OFF";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    if(EnableActiveWnd->GetCheckedState_MAYBE())
    {
        FreezeON();
        wchar_t message[] = L"Always Active Window : ON";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    else
    {
        FreezeOFF();
        wchar_t message[] = L"Always Active Window : OFF";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    if(HideCharInfo->GetCheckedState_MAYBE())
    {
        wchar_t message[] = L"Other players cannot show your items anymore!";
        CIFSystemMessage* systemmessage = (CIFSystemMessage*)(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);

    }
    else
    {

        wchar_t message[] = L"Other players can show your items!";
        CIFSystemMessage* systemmessage = reinterpret_cast<CIFSystemMessage*>(g_pCGInterface->m_IRM.GetResObj(68, 1));
        int color = D3DCOLOR_ARGB(255, 255, 255, 255);
        systemmessage->WriteMessage(0xFF, color, message, 0, 1);
    }
    HideCharInfoSetting = HideCharInfo->GetCheckedState_MAYBE();
    RememberPCSetting = RememberPC->GetCheckedState_MAYBE();
    CMsgStreamBuffer buf(0x169A);
    buf << byte(24);
    buf << HideCharInfo->GetCheckedState_MAYBE();
    buf << RememberPC->GetCheckedState_MAYBE();
    SendMsg(buf);

    char buffer3[0x200];
    sprintf(buffer3, "%s\\Setting\\Client_Extra.txt", theApp.GetWorkingDir());

// Dosyayı yazma modunda aç
    FILE *file3 = fopen(buffer3, "w");
    if (file3 != NULL) {
        // Veri kontrolü ve dosyaya yazma
        fprintf(file3, "Fps Window: %d\n", EnableShowFps->GetCheckedState_MAYBE());
        fprintf(file3, "Infinity Zoom: %d\n", EnableZoom->GetCheckedState_MAYBE());
        fprintf(file3, "Always Active Window: %d\n", EnableActiveWnd->GetCheckedState_MAYBE());

        fclose(file3);
    }
}