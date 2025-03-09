//
// Created by transgendercat on 3/5/2025.
//

#ifndef COLORCELL_H
#define COLORCELL_H

#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <ui/ColorPickPopup.hpp>
#include "../colorTypes.h"

using namespace geode::prelude;

class colorPopup;
class colorCell : public CCNode, public ColorPickPopupDelegate
{
protected:
    // your mom
    colorPopup* m_parentPopup;
    ColorEntry* m_entry;
    std::function<void()> m_onDelete;
    // recently added
    std::function<void()> m_onColorPicker;
    std::function<void(ccColor4B const& color)> m_updateColor;

    // CCMenuItemToggler* m_toggler;
    CCScale9Sprite* m_background;

    // Added onColorPicker
    bool init(colorPopup* parent_popup, ColorEntry* entry, std::function<void()> onDelete, std::function<void()> onColorPicker, std::function<void(ccColor4B const& color)> updateColor, CCSize const& size);
    // void updateUI() const;


public:
    // Added onColorPicker once again
    static colorCell* create(colorPopup* parent_popup, ColorEntry* entry, std::function<void()> onDelete, std::function<void()> onColorPicker, std::function<void(ccColor4B const& color)> updateColor, CCSize const& size);
    void onToggle(CCObject*); // TODO: Remove this without breaking the entire thing
    void onDelete(CCObject*);
    void onColorPicker(CCObject*);
    void updateColor(ccColor4B const& color) override;
};

#endif //COLORCELL_H
