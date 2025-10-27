#include "ColorListPopup.hpp"

bool ColorListPopup::setup(ColorSettingNode* setting) {
    setID("ColorListPopup");
    m_mainLayer->setID("main-layer");
    m_bgSprite->setID("background");
    m_buttonMenu->setID("button-menu");
    m_closeBtn->setID("close-button");

    m_node = setting;
    m_colors = setting->getValue();

    return true;
}

ColorListPopup* ColorListPopup::create(ColorSettingNode* setting) {
    auto ret = new ColorListPopup();
    if (!ret->initAnchored(320.f, 260.f, setting, "geode.loader/GE_square01-uhd.png")) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}