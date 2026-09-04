#include "ColorPicker2.h"
#include <GInterface.h>

#include "../../../source/DevKit_DLL/src/Util.h"
#include <ICPlayer.h>
#include <Juice/CustomsMarket/IFCustomsMarket.h>
#include <imgui/imgui.h>


void ColorPicker2::Render() {
    if(!g_pCICPlayer)
        return;

    CIFCustomsMarket *marketWnd = (CIFCustomsMarket *) g_pCGInterface->GetWindowByUniqueID(13397);
    if(!marketWnd->IsVisible())
        return;
    else
        marketWnd->showColorPicker = false;

    if(marketWnd->showColorPicker)
        return;

    ImGui::SetNextWindowSize(ImVec2(350,200));
    if (!ImGui::Begin("bimbum2",0,ImGuiWindowFlags_NoBackground |ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoResize| ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse ))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::ColorPicker3("Select Color2", m_color, ImGuiColorEditFlags_NoOptions  | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs);
    ImGuiStyle* style = &ImGui::GetStyle();

    style->Colors[ImGuiCol_TitleBg] = ImColor(0,0,0,130);
    style->Colors[ImGuiCol_TitleBgActive] = ImColor(0,0,0,130);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0,0,0,130);

    int red = static_cast<int>(m_color[0] * 255.0f);
    int green = static_cast<int>(m_color[1] * 255.0f);
    int blue = static_cast<int>(m_color[2] * 255.0f);

    D3DCOLOR TitleColor = D3DCOLOR_ARGB(255, red, green, blue);

    ImGui::SetWindowPos("bimbum2",ImVec2(marketWnd->GetPos().x + 550,marketWnd->GetPos().y + 103));
    marketWnd->colorPicker2 = TitleColor;
    marketWnd->m_example2->m_FontTexture.SetColor(TitleColor);

    ImGui::End();
}

ColorPicker2::ColorPicker2() :  interface_draw_marker(false), interface_draw_outline(false) {

}
