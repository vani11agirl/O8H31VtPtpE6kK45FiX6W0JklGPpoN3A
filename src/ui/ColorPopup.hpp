//
// Created by transgendercat on 3/5/2025.
//

#ifndef COLORPOPUP_H
#define COLORPOPUP_H

#include "../colorTypes.hpp"
#include <Geode/Geode.hpp>
#include <ui/ColorPickPopup.hpp>
#include "ColorCell.hpp"
#include <ui/ScrollLayer.hpp>

using namespace geode::prelude;

class ColorListSettingNode;

class ColorPopup : public Popup<std::vector<ColorEntry>, ColorListSettingNode*> {
    protected:
        int m_index;
        ScrollLayer *m_scrollLayer;
        CCMenuItemSpriteExtra* m_addBtn;
        ColorListSettingNode *m_setting;

        bool setup(std::vector<ColorEntry> colors, ColorListSettingNode *setting) override;

        void onAdd(CCObject *);
        void onClose(CCObject *sender) override;

    public:
        void createList();
        void updateAddButtonState() const;

        std::vector<ColorEntry> m_colors;
        std::vector<ColorCell *> m_colorCells;

        static ColorPopup *create(std::vector<ColorEntry> colors, ColorListSettingNode *setting);
};



#endif //COLORPOPUP_H
