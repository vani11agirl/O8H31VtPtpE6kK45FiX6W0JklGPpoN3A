//
// Created by transgendercat on 3/5/2025.
//

// Use log::<severity> to log. for example, "log::debug(\"hello world\")"
#include "colorCell.h"
#include "../utils/color.h"

#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <utility>

#include "colorPopup.h"

using namespace geode::prelude;
using namespace custom::utils::color;

bool colorCell::init(
    colorPopup *parent_popup,
    ColorEntry *entry,
    std::function<void()> onDelete,
    std::function<void()> onColorPicker,
    std::function<void(ccColor4B const& color)> updateColor, // I'm gonna eat someone
    const CCSize &size
    )
{
    log::debug("Initializing colorCell");
    if (!CCNode::init()) return false;

    static constexpr float HORIZONTAL_PADDING = 2.6f;

    m_parentPopup = parent_popup;
    m_entry = entry;
    m_onDelete = std::move(onDelete);
    m_onColorPicker = std::move(onColorPicker);
    m_updateColor = std::move(updateColor);

    log::debug("Parsing color from hex: {}", m_entry->m_hex);
    auto entryColor = ColorUtils::hexToColor3B(m_entry->m_hex);

    this->setContentSize(size);
    this->setAnchorPoint(ccp(.5f, .5f));
    this->setID("color_cell"_spr);

    auto bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({entryColor.r, entryColor.g, entryColor.b});
    bg->setOpacity(100);
    bg->setScale(.3f);
    bg->setContentSize(size / bg->getScale());
    bg->setID("bg"_spr);
    this->addChildAtPosition(bg, Anchor::Center);

    // log::debug("Creating toggle button");
    float m_buttonSize = 0.f;

    // m_toggler = CCMenuItemToggler::createWithStandardSprites(
    //     this,
    //     menu_selector(colorCell::onToggle),
    //     .6f
    //     );
    // m_toggler->setAnchorPoint(ccp(.5f, .5f));
    //
    // m_buttonSize += m_toggler->getContentWidth();

    auto btnsMenu = CCMenu::create();
    // btnsMenu->addChild(m_toggler);
    btnsMenu->setAnchorPoint(ccp(1.f, .5f));
    btnsMenu->setContentSize(CCSize(50.f, 30.f));

    // Always true
    // if (m_entry->m_userAdded) {
    log::debug("Adding delete button");
    auto deleteIcon = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    deleteIcon->setScale(.7f);

    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteIcon,
        this,
        menu_selector(colorCell::onDelete)
        );

    deleteBtn->setID("delete-btn"_spr);
    btnsMenu->addChild(deleteBtn);

    m_buttonSize += deleteBtn->getContentWidth();

    log::debug("Here goes the color picker");
    auto paintBtn = CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png");
    paintBtn->setScale(.7f);

    auto colorPickerBtn = CCMenuItemSpriteExtra::create(
        paintBtn,
        this,
        menu_selector(colorCell::onColorPicker)
        );

    colorPickerBtn->setID("color-picker"_spr);
    btnsMenu->addChild(colorPickerBtn);

    m_buttonSize += colorPickerBtn->getContentWidth();

    // }

    btnsMenu->setLayout(
        RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Even)
        );

    btnsMenu->updateLayout();
    btnsMenu->setID("button_menu"_spr);

    log::debug("Creating text input field");
    log::debug("Before calling TextInput::create()");
    auto input_node = TextInput::create(
        size.width - HORIZONTAL_PADDING * 2 - m_buttonSize,
        "Color Hex Code",
        "JetBrainsMono.fnt"_spr
    );
    log::debug("After calling TextInput::create()");
    input_node->setScale(1.f);
    log::debug("Set scale on text input");
    input_node->setFilter("#1234567890ABCDEF");
    log::debug("Set filter on text input");
    input_node->setMaxCharCount(7);
    log::debug("Set max char count on text input");
    input_node->setString(m_entry->m_hex, false);
    log::debug("Set string on text input: {}", m_entry->m_hex);
    input_node->setTextAlign(TextInputAlign::Left);
    log::debug("Set text alignment on text input");
    input_node->setCallback(
        [this](std::string const &str)
        {
            log::debug("Hex input changed to: {}", str);
            m_entry->m_hex = str;
            if (str.length() == 7)
            {
                log::debug("Hex code reached 7 characters, updating background color");
                ccColor3B newColor = ColorUtils::hexToColor3B(str);
                if (auto bgSprite = dynamic_cast<CCScale9Sprite*>(this->getChildByID("bg"_spr)))
                {
                    bgSprite->setColor(newColor);
                    log::debug("Background color updated to: ({}, {}, {})", newColor.r, newColor.g, newColor.b);
                }
                else
                {
                    log::error("Background sprite not found");
                }
            }
        }
    );
    log::debug("Finished setting callback on text input");

    auto menu = CCMenu::create();
    log::debug("Created CCMenu for text input field");
    menu->addChild(input_node);
    log::debug("Added text input to menu");
    menu->addChild(btnsMenu);
    log::debug("Added button menu to menu");

    menu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Between)
    );
    log::debug("Set layout on menu");
    menu->setID("menu"_spr);
    menu->setAnchorPoint(ccp(.5f, .5f));
    menu->setContentSize(CCSize{size.width - HORIZONTAL_PADDING * 2, size.height});
    log::debug("Set content size on menu");
    menu->updateLayout();
    log::debug("Updated layout on menu");

    this->addChildAtPosition(menu, Anchor::Center, ccp(.0f, .0f));
    log::debug("Added menu to colorCell");

    // this->updateUI();
    // log::debug("Called updateUI on colorCell");

    log::debug("colorCell initialized successfully");
    return true;
}
//
// void colorCell::updateUI() const
// {
//     log::debug("Updating UI: toggler state is {}", m_entry->m_enabled);
//     m_toggler->toggle(m_entry->m_enabled);
// }
//
// void colorCell::onToggle(CCObject*) const
// {
//     log::debug("Toggling entry from {} to {}", m_entry->m_enabled, !m_entry->m_enabled);
//     m_entry->m_enabled = !m_entry->m_enabled;
//     this->updateUI();
//     m_toggler->toggle(!m_toggler->isToggled());
// }

void colorCell::onDelete(CCObject*)
{
    log::debug("onDelete called");
    m_onDelete();
}

void colorCell::onColorPicker(CCObject*)
{
    log::debug("onColorPicker called");
    m_onColorPicker();
}

void colorCell::updateColor(ccColor4B const& color)
{
    log::debug("!");
    m_updateColor(color);
}


colorCell* colorCell::create(colorPopup* parent_popup, ColorEntry* entry, std::function<void()> onDelete, std::function<void()> onColorPicker, std::function<void(ccColor4B const& color)> updateColor, CCSize const& size)
{
    log::debug("Creating new colorCell instance");
    auto ret = new colorCell();
    if (ret->init(parent_popup, entry, std::move(onDelete), std::move(onColorPicker), std::move(updateColor), size))
    {
        log::debug("colorCell instance created successfully");
        ret->autorelease();
        return ret;
    }

    log::error("Failed to initialize colorCell, deleting instance");
    CC_SAFE_DELETE(ret);
    return nullptr;
}
