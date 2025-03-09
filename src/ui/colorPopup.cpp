#include <utility>
#include "colorPopup.h"
#include "colorCell.h"
#include <algorithm>
#include <functional>

#include "../utils/color.h"

using namespace geode::prelude;

bool colorPopup::setup(std::vector<ColorEntry> colors, std::function<void(std::vector<ColorEntry>)> callback)
{
    log::debug("Setting up colorPopup with {} colors", colors.size());
    m_colors = colors;
    m_onColorChanged = callback;
    this->createList();
    log::debug("Creating add button");
    CCSprite* plusBtn = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    plusBtn->setScale(.7f);
    m_addBtn = CCMenuItemSpriteExtra::create(
        plusBtn,
        this,
        menu_selector(colorPopup::onAdd)
    );
    m_addBtn->setAnchorPoint({0.5, 0.5});
    m_addBtn->setID("addBtn"_spr);
    auto menu = CCMenu::create();
    menu->addChild(m_addBtn);
    menu->setZOrder(1);
    menu->setID("menu"_spr);
    log::debug("Adding menu to main layer");
    m_mainLayer->addChildAtPosition(
        menu,
        Anchor::BottomRight,
        {-5.f - m_addBtn->getContentWidth() / 2.f, 8.f + m_addBtn->getContentHeight() / 2.f}
    );
    this->updateAddButtonState(); // Update the add button state initially
    return true;
}

void colorPopup::createList()
{
    log::debug("Creating color list");
    auto contentSize = m_mainLayer->getContentSize();
    log::debug("Content size returned: width = {}, height = {}", contentSize.width, contentSize.height);
    constexpr float HORIZONTAL_PADDING = 5.f;

    if (m_scrollLayer)
    {
        log::debug("Removing existing scroll layer");
        m_scrollLayer->removeFromParent();
    }
    float contentSizeX = contentSize.width - HORIZONTAL_PADDING * 2;
    float contentSizeY = contentSize.height - HORIZONTAL_PADDING * 2 - 4.f;
    log::debug("Computed ScrollLayer Size: width = {}, height = {}", contentSizeX, contentSizeY);
    m_scrollLayer = ScrollLayer::create({contentSizeX, contentSizeY});
    m_scrollLayer->setContentSize({contentSizeX, contentSizeY});
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(contentSizeY)
        ->setGap(HORIZONTAL_PADDING / 2)
    );
    m_scrollLayer->setPosition({HORIZONTAL_PADDING, HORIZONTAL_PADDING + 2.f});
    m_scrollLayer->setID("scroll-layer"_spr);
    log::debug("Adding {} colors to the list", m_colors.size());
    for (size_t i = 0; i < m_colors.size(); i++)
    {
        size_t index = i;
        auto originalHex = m_colors[index].m_hex;
        auto colorCell = colorCell::create(
            this,
            &m_colors[index],
            [this, index, originalHex]()
            {
                log::debug("Deleting colorCell for entry with hex: {}", originalHex);
                if (index < m_colors.size() && m_colors[index].m_hex == originalHex)
                {
                    m_colors.erase(m_colors.begin() + index);
                }
                else
                {
                    auto it = std::ranges::find_if(m_colors, [originalHex](const ColorEntry& e) {
                        return e.m_hex == originalHex;
                    });
                    if (it != m_colors.end())
                    {
                        m_colors.erase(it);
                    }
                    else
                    {
                        log::error("Failed to find the color entry during deletion callback");
                    }
                }
                this->createList();
                this->updateAddButtonState(); // Update the add button state after deletion
            },
            [this, index, originalHex]()
            {
                log::debug("Attempting to bring up the color picker for entry with hex {} for index {}.", originalHex, index);
                m_index = index;
                auto p = ColorPickPopup::create(custom::utils::color::ColorUtils::hexToColor3B(originalHex));
                p->setDelegate(this);
                p->setID("colorPicker"_spr);
                p->show();
            },
            [this, index](ccColor4B const& color)
            {
                log::debug("updateColor fired in colorPopup with: R={}, G={}, B={}, A={}",
                           color.r, color.g, color.b, color.a);
                ccColor3B newColor = { color.r, color.g, color.b };
                if (m_index < m_colors.size()) {
                    m_colors[index].m_hex = custom::utils::color::ColorUtils::color3BToHex(newColor);
                    log::debug("Updated color at index {} to: {}", index, m_colors[index].m_hex);
                } else {
                    log::error("updateColor failed: invalid index {}", index);
                }
                this->createList();
            },
            CCSize{320.f - HORIZONTAL_PADDING * 2, 35.f}
        );
        colorCell->setAnchorPoint({0, 0});
        std::string cellId = std::string(GEODE_MOD_ID) + "/color-cell-" + std::to_string(i);
        colorCell->setID(cellId.c_str());
        m_scrollLayer->m_contentLayer->addChild(colorCell);
    }
    auto menu2 = CCMenu::create();
    menu2->setContentSize({0.f, 36.f});
    m_scrollLayer->m_contentLayer->addChild(menu2);
    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->m_contentLayer->setID("content-layer"_spr);
    this->m_mainLayer->addChild(m_scrollLayer);
    handleTouchPriority(this);
}

void colorPopup::onClose(CCObject *sender)
{
    log::debug("Closing colorPopup");
    m_onColorChanged(m_colors);
    Popup::onClose(sender);
}

void colorPopup::onAdd(CCObject*)
{
    log::debug("Adding new color entry");
    m_colors.emplace_back("#FFFFFF", true, true);
    this->createList();
    this->updateAddButtonState(); // Update the add button state after adding
}


colorPopup* colorPopup::create(std::vector<ColorEntry> colors, const std::function<void(std::vector<ColorEntry>)>& onColorChanged)
{
    log::debug("Creating new colorPopup instance");
    auto ret = new colorPopup();
    auto size = CCSize{320.f, 240.f};
    if (ret->initAnchored(
        size.width,
        size.height,
        std::move(colors),
        onColorChanged,
        "geode.loader/GE_square01.png"
    ))
    {
        log::debug("colorPopup instance created successfully");
        ret->autorelease();
        return ret;
    }
    log::error("Failed to initialize colorPopup, deleting instance");
    CC_SAFE_DELETE(ret);
    return ret;
}

void colorPopup::updateAddButtonState() const
{
    if (m_addBtn)
    {
        if (m_colors.size() >= 7) {
            static_cast<CCSprite*>(m_addBtn->getNormalImage())->setColor(ccColor3B{166, 166, 166});
            m_addBtn->setEnabled(false);
        } else {
            static_cast<CCSprite*>(m_addBtn->getNormalImage())->setColor(ccColor3B{255, 255, 255});
            m_addBtn->setEnabled(true);
        }
    }
}
