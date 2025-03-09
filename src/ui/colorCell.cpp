//
// Created by transgendercat on 3/5/2025.
//

#include "colorCell.h"
#include "../utils/color.h"

#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <utility>

#include "colorPopup.h"
#include "GrabNodeLayer.h" // Thanks TheSillyDoggo, once again.

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
    if (!CCNode::init()) return false;

    static constexpr float HORIZONTAL_PADDING = 2.6f;

    m_parentPopup = parent_popup;
    m_entry = entry;
    m_onDelete = std::move(onDelete);

    // TODO: Properly figure this out, updateColor() never got called
    m_onColorPicker = std::move(onColorPicker);
    m_updateColor = std::move(updateColor);

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

    /* * * * * * * * *
     * Delete Button *
     * * * * * * * * */

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

    /* * * * * * * * * * * *
     * Color Picker Button *
     * * * * * * * * * * * */
    // TODO: Have a crack at trying to fix it if you want, I give up trying to.
    // auto paintBtn = CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png");
    // paintBtn->setScale(.7f);

    // auto colorPickerBtn = CCMenuItemSpriteExtra::create(
    //     paintBtn,
    //     this,
    //     menu_selector(colorCell::onColorPicker)
    //     );
    //
    // colorPickerBtn->setID("color-picker"_spr);
    // btnsMenu->addChild(colorPickerBtn);
    //
    // m_buttonSize += colorPickerBtn->getContentWidth();

    // } endif

    /* * * * * * * * * * *
     * Reordering Colors *
     * * * * * * * * * * */
    auto grabNodeLayer = GrabNodeLayer::create();
    grabNodeLayer->setNodeToGrab(this);
    grabNodeLayer->setLockedAxis(LockedAxis::Vertical);
    grabNodeLayer->setOnStartDrag([this, bg, entryColor]
    {
        this->runAction(CCEaseInOut::create(CCScaleTo::create(.2f, .95f), 2));
        bg->runAction(CCTintTo::create(.35f, entryColor.r - 100, entryColor.g - 100, entryColor.b - 100)); // idk
        this->setZOrder(CCScene::get()->getHighestChildZ());
    });
    // TODO: idk
    // grabNodeLayer->setOnEndDrag()

    btnsMenu->setLayout(
        RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Even)
        );

    btnsMenu->updateLayout();
    btnsMenu->setID("button_menu"_spr);

    auto input_node = TextInput::create(
        size.width - HORIZONTAL_PADDING * 2 - m_buttonSize,
        "Color Hex Code",
        "JetBrainsMono.fnt"_spr
    );
    input_node->setScale(1.f);
    input_node->setFilter("#1234567890ABCDEFabcdef"); // As long as you don't have the character filter bypass hack...
    input_node->setMaxCharCount(7); // As long as you don't have the character length bypass hack...
    input_node->setString(m_entry->m_hex, false); // Initiate the string
    input_node->setTextAlign(TextInputAlign::Left);
    input_node->setCallback(
        [this](std::string const &str)
        {
            m_entry->m_hex = str;
            if (str.length() == 7)
            {
                ccColor3B newColor = ColorUtils::hexToColor3B(str);
                if (auto bgSprite = dynamic_cast<CCScale9Sprite*>(this->getChildByID("bg"_spr)))
                {
                    bgSprite->setColor(newColor);
                }
                else
                {
                }
            }
        }
    );

    auto menu = CCMenu::create();
    menu->addChild(input_node);
    menu->addChild(btnsMenu);

    menu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Between)
    );
    menu->setID("menu"_spr);
    menu->setAnchorPoint(ccp(.5f, .5f));
    menu->setContentSize(CCSize{size.width - HORIZONTAL_PADDING * 2, size.height});
    menu->updateLayout();

    this->addChildAtPosition(menu, Anchor::Center, ccp(.0f, .0f));

    return true;
}

void colorCell::onDelete(CCObject*)
{
    m_onDelete();
}

void colorCell::onColorPicker(CCObject*)
{
    m_onColorPicker();
}

void colorCell::updateColor(ccColor4B const& color)
{
    log::debug("I would hug you if you could make this line of log appear");
    m_updateColor(color);
}


colorCell* colorCell::create(colorPopup* parent_popup, ColorEntry* entry, std::function<void()> onDelete, std::function<void()> onColorPicker, std::function<void(ccColor4B const& color)> updateColor, CCSize const& size)
{
    auto ret = new colorCell();
    if (ret->init(parent_popup, entry, std::move(onDelete), std::move(onColorPicker), std::move(updateColor), size))
    {
        ret->autorelease();
        return ret;
    }

    log::error("what the sigma");
    CC_SAFE_DELETE(ret);
    return nullptr;
}
