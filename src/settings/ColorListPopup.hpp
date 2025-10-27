#pragma once

#include "ColorSetting.hpp"

using namespace geode::prelude;

class ColorListPopup : public Popup<ColorSettingNode*> {
protected:
    bool setup(ColorSettingNode* setting) override;

    ColorSettingNode* m_node = nullptr;
    std::vector<std::string> m_colors;
public:
    static ColorListPopup* create(ColorSettingNode* setting);
};