//
// Created by transgendercat on 3/5/2025.
//

#include "ColorCell.hpp"
#include "../utils/color.hpp"

#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>

#include "ColorPopup.hpp"
#include "GrabNodeLayer.hpp" // Thanks TheSillyDoggo, once again.

using namespace geode::prelude;
using namespace custom::utils::color;

bool ColorCell::init(ColorPopup *parent_popup, ColorEntry *entry, const CCSize &size, const int index)
{
    if (!CCNode::init()) return false;

    static constexpr float HORIZONTAL_PADDING = 2.6f;

    m_parentPopup = parent_popup;
    m_entry = entry;
    m_index = index;
    m_originalHex = m_entry->m_hex;

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

    auto btnsMenu = CCMenu::create();
    btnsMenu->setAnchorPoint(ccp(1.f, .5f));
    btnsMenu->setContentSize(CCSize(50.f, 30.f));

    /* * * * * * * * *
     * Delete Button *
     * * * * * * * * */

    auto deleteIcon = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
    deleteIcon->setScale(.7f);

    auto deleteBtn = CCMenuItemSpriteExtra::create(
        deleteIcon,
        this,
        menu_selector(ColorCell::onDelete)
        );

    deleteBtn->setID("delete-btn"_spr);
    btnsMenu->addChild(deleteBtn);

    m_buttonSize += deleteBtn->getContentWidth();

    /* * * * * * * * * * * *
     * Color Picker Button *
     * * * * * * * * * * * */
    auto paintSpr = CCSprite::createWithSpriteFrameName("GJ_paintBtn_001.png");
    paintSpr->setScale(.7f);
    auto colorPickerBtn = CCMenuItemSpriteExtra::create(
        paintSpr,
        this,
        menu_selector(ColorCell::onColorPicker)
        );
    
    colorPickerBtn->setID("color-picker"_spr);
    btnsMenu->addChild(colorPickerBtn);
    
    m_buttonSize += colorPickerBtn->getContentWidth();

    // } endif

    /* * * * * * * * * * *
     * Reordering Colors *
     * * * * * * * * * * */
    auto grabNodeLayer = GrabNodeLayer::create();
    grabNodeLayer->setNodeToGrab(this);
    grabNodeLayer->setLockedAxis(LockedAxis::Vertical);
    grabNodeLayer->setOnStartDrag([this, bg, entryColor] {
        this->m_initialDragY = this->getPositionY();
        this->runAction(CCEaseInOut::create(CCScaleTo::create(0.2f, 0.95f), 2));
        auto darkenColor = [](GLubyte color) -> GLubyte {
            return static_cast<GLubyte>(std::max(0, color - 50));
        };
        bg->runAction(CCTintTo::create(0.35f, darkenColor(entryColor.r), darkenColor(entryColor.g), darkenColor(entryColor.b)));
        this->setZOrder(CCScene::get()->getHighestChildZ());
    });

    // FIXME: Game may or may not explode
    grabNodeLayer->setOnEndDrag([this, bg, entryColor] {
        // Prevent multiple calls
        // if (this->m_eow)
        //     return;
        // this->m_eow = true;

        this->runAction(CCEaseBackOut::create(CCScaleTo::create(0.35f, 1.0f)));
        bg->runAction(CCTintTo::create(0.35f, entryColor.r, entryColor.g, entryColor.b));

        float deltaY = m_initialDragY - this->getPositionY();
        float blockHeight = 35.0f + 2.5f; // cell height + gap

        if (fabs(deltaY) < 5.0f) {
            m_parentPopup->createList();
            // this->m_eow = false;
            return;
        }

        int deltaIndex = static_cast<int>(round(deltaY / blockHeight));
        int count = static_cast<int>(m_parentPopup->m_colorCells.size());
        int origIndex = m_index;
        int newIndex = origIndex + deltaIndex;

        newIndex = std::clamp(newIndex, 0, count - 1);

        log::debug("Dragged cell moved from index {} to index {}", origIndex, newIndex);

        if (newIndex == origIndex) {
            m_parentPopup->createList();
            // this->m_eow = false;
            return;
        }

        auto& cells = m_parentPopup->m_colorCells;
        auto& colors = m_parentPopup->m_colors;

        if (origIndex < 0 || origIndex >= count || newIndex < 0 || newIndex >= count) {
            log::error("Invalid index? origIndex: {}, newIndex: {}, count: {}", origIndex, newIndex, count);
            m_parentPopup->createList();
            // this->m_eow = false;
            return;
        }

        ColorCell* draggedCell = cells[origIndex];
        ColorEntry draggedColor = colors[origIndex];

        cells.erase(cells.begin() + origIndex);
        colors.erase(colors.begin() + origIndex);

        cells.insert(cells.begin() + newIndex, draggedCell);
        colors.insert(colors.begin() + newIndex, draggedColor);

        for (int i = 0; i < count; i++) {
            cells[i]->setIndex(i);
        }

        m_parentPopup->createList();

        // this->m_eow = false;
    });


    grabNodeLayer->setContentSize(ccp(this->getContentHeight() - 2, this->getContentHeight() - 2));
    grabNodeLayer->setScale(.5f, 1.f);
    grabNodeLayer->setLayoutOptions(AnchorLayoutOptions::create()->setAnchor(Anchor::Right)->setOffset(ccp(-10, 0)));
    grabNodeLayer->setID("node-grabber"_spr);

    auto grabSpr = CCSprite::create("draggable.png"_spr);
    grabSpr->setPosition(grabNodeLayer->getPosition());
    grabSpr->setScale(.6f);
    grabSpr->setOpacity(200);
    btnsMenu->addChild(grabSpr);

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
        // @geode-ignore(unknown-resource)
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
                if (auto bgSprite = typeinfo_cast<CCScale9Sprite*>(this->getChildByID("bg"_spr)))
                {
                    bgSprite->setColor(newColor);
                }
            }
        }
    );

    auto menu = CCMenu::create();
    menu->addChild(input_node);
    menu->addChild(btnsMenu);
    this->addChild(grabNodeLayer);

    menu->setLayout(RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Between)
    );
    menu->setID("menu"_spr);
    menu->setAnchorPoint(ccp(.5f, .5f));
    menu->setContentSize(CCSize{size.width - HORIZONTAL_PADDING * 2, size.height});
    menu->updateLayout();


    this->addChildAtPosition(menu, Anchor::Center, ccp(.0f, .0f));
    grabNodeLayer->getParent()->updateLayout();

    m_cells.push_back(this);

    return true;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ColorCell::onDelete(CCObject*) {
    if (m_parentPopup->m_colors.size() == 1) return FLAlertLayer::create("Hello...?", "Y-you can't have <cr>no colors</c> at all~", "Okay")->show();

    auto originalHex = m_originalHex;
    auto it = std::ranges::find_if(m_parentPopup->m_colors, [originalHex](const ColorEntry& e) {
        return e.m_hex == originalHex;
    });
    if (it != m_parentPopup->m_colors.end())
    {
        m_parentPopup->m_colors.erase(it);
    }
    else
    {
        log::error("Oops...");
    }
    m_parentPopup->createList();
    m_parentPopup->updateAddButtonState(); // Update the add button state after deletion
}

void ColorCell::onColorPicker(CCObject*)
{
    auto p = ColorPickPopup::create(ColorUtils::hexToColor3B(m_originalHex));
    p->setDelegate(this);
    p->setID("colorPicker"_spr);
    p->show();
}

void ColorCell::updateColor(ccColor4B const& color)
{
    ccColor3B newColor = { color.r, color.g, color.b };
    if (m_index < m_parentPopup->m_colors.size()) {
        m_parentPopup->m_colors[m_index].m_hex = ColorUtils::color3BToHex(newColor);
    } else {
    }
    m_parentPopup->createList();
}

ColorCell* ColorCell::create(ColorPopup* parent_popup, ColorEntry* entry, CCSize const& size, const int index)
{
    auto ret = new ColorCell();
    if (ret->init(parent_popup, entry, size, index))
    {
        ret->autorelease();
        return ret;
    }

    log::error("what the sigma");
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ColorEntry* ColorCell::getEntry() const
{
    return m_entry;
}

int ColorCell::getIndex() const
{
    return m_index;
}

std::string ColorCell::getOriginalHex() {
    return m_originalHex;
}

// super hacky
void ColorCell::setIndex(const int index) {
    m_index = index;
}
