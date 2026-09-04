#pragma once

class ColorPicker {
public:

    ColorPicker();

    void Render();
private:
    float m_color[3];
    bool interface_draw_outline;
    bool interface_draw_marker;
};
