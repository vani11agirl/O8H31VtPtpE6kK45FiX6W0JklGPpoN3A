//
// Created by transgendercat on 3/5/2025.
//

#ifndef COLORPOPUP_H
#define COLORPOPUP_H

#include "../colorTypes.h"
#include <Geode/Geode.hpp>
#include <ui/ColorPickPopup.hpp>
#include <ui/ScrollLayer.hpp>

using namespace geode::prelude;

class colorPopup : public Popup<std::vector<ColorEntry>, std::function<void(std::vector<ColorEntry>)>>, public ColorPickPopupDelegate {
    protected:
        std::vector<ColorEntry> m_colors;
        std::function<void(std::vector<ColorEntry>)> m_onColorChanged;

        ScrollLayer *m_scrollLayer;
        CCMenuItemSpriteExtra* m_addBtn;
        int m_index;

        bool setup(std::vector<ColorEntry> colors, std::function<void(std::vector<ColorEntry>)>) override;

        void createList();

        void onAdd(CCObject *);
        void onClose(CCObject *sender) override;

        void updateAddButtonState() const;

        // void updateColor(ccColor4B const& color) override;

    public:
        static colorPopup *create(std::vector<ColorEntry> colors, const std::function<void(std::vector<ColorEntry>)>& onColorChanged);
};



#endif //COLORPOPUP_H
