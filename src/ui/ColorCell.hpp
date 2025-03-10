//
// Created by transgendercat on 3/5/2025.
//

#ifndef COLORCELL_H
#define COLORCELL_H

#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/binding/CCMenuItemToggler.hpp>
#include <ui/ColorPickPopup.hpp>
#include "../colorTypes.hpp"

using namespace geode::prelude;

class ColorPopup;

class ColorCell : public CCNode, public ColorPickPopupDelegate
{
protected:
    CCScale9Sprite* m_background;
    ColorPopup* m_parentPopup;
    ColorEntry* m_entry;
    int m_index;
    std::string m_originalHex;
    std::vector<CCNode*> m_cells;

    bool init(ColorPopup* parentPopup, ColorEntry* entry, CCSize const& size, int index);

public:
    static ColorCell* create(ColorPopup* parentPopup, ColorEntry* entry, CCSize const& size, int index);
    void onDelete(CCObject*);
    void onColorPicker(CCObject*);
    void updateColor(ccColor4B const& color) override;

    ColorEntry *getEntry();
    int getIndex();
    std::string getOriginalHex();
};

#endif //COLORCELL_H
