#include <utility>
#include "ColorPopup.hpp"
#include "ColorCell.hpp"
#include "PresetPopup.hpp"
#include "../utils/customSettings.hpp"

using namespace geode::prelude;

bool ColorPopup::setup(std::vector<ColorEntry> colors, ColorListSettingNode *setting)
{
    m_setting = setting;
    m_colors = colors;
    this->createList();
    CCSprite* plusBtn = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    plusBtn->setScale(.7f);
    m_addBtn = CCMenuItemSpriteExtra::create(
        plusBtn,
        this,
        menu_selector(ColorPopup::onAdd)
    );
    m_addBtn->setAnchorPoint({0.5, 0.5});
    m_addBtn->setID("addBtn"_spr);
    // auto menu = CCMenu::create();
    // menu->addChild(m_addBtn);
    // menu->setZOrder(1);
    // menu->setID("menu"_spr);
    m_buttonMenu->addChildAtPosition(
        m_addBtn,
        Anchor::BottomRight,
        {-5.f - m_addBtn->getContentWidth() / 2.f, 8.f + m_addBtn->getContentHeight() / 2.f}
    );
    this->updateAddButtonState();

    // [Presets] with transgender flag background
    // auto tPresetSprite = ButtonSprite::create("Presets", "bigFont.fnt", "geode.loader/GE_button_04.png");
    // tPresetSprite->setScale(.7f);
    // m_tPresetBtn = CCMenuItemSpriteExtra::create(
    //     tPresetSprite,
    //     this,
    //     nullptr
    //     );
    // m_tPresetBtn->setID("colon-three");
    // m_buttonMenu->addChildAtPosition(
    //     m_tPresetBtn,
    //     Anchor::BottomLeft,
    //     {5.f + m_tPresetBtn->getContentWidth() / 2.f, 8.f + m_tPresetBtn->getContentHeight() / 2.f}
    // );

    // [*] button real! I still can't decide...
    auto presetSprite = CCSprite::createWithSpriteFrameName("GJ_starBtn_001.png");
    presetSprite->setScale(.7f);
    m_presetBtn = CCMenuItemSpriteExtra::create(
        presetSprite,
        this,
        menu_selector(ColorPopup::onPresets)
        );
    m_presetBtn->setAnchorPoint({0.5f, 0.5f});
    m_presetBtn->setID("preset-button");
    m_buttonMenu->addChildAtPosition(
        m_presetBtn,
        Anchor::BottomLeft,
{5.f + m_presetBtn->getContentWidth() / 2.f, 8.f + m_presetBtn->getContentHeight() / 2.f}
        );

    return true;
}

void ColorPopup::createList()
{
    // avoid blowing up the game, hopefully~
    m_colorCells.clear();

    auto contentSize = m_mainLayer->getContentSize();
    constexpr float HORIZONTAL_PADDING = 5.f;
    if (m_scrollLayer)
    {
        m_scrollLayer->removeFromParent();
    }
    float contentSizeX = contentSize.width - HORIZONTAL_PADDING * 2;
    float contentSizeY = contentSize.height - HORIZONTAL_PADDING * 2 - 4.f;
    m_scrollLayer = ScrollLayer::create({contentSizeX, contentSizeY});
    m_scrollLayer->m_contentLayer->setLayout(
        ColumnLayout::create()
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(contentSizeY)
        ->setGap(HORIZONTAL_PADDING / 2)
    );
    m_scrollLayer->setPosition({HORIZONTAL_PADDING, HORIZONTAL_PADDING + 2.f});
    m_scrollLayer->setID("scroll-layer"_spr);

    for (size_t i = 0; i < m_colors.size(); i++)
    {
        size_t index = i;
        auto cell = ColorCell::create(
            this,
            &m_colors[index],
            CCSize{320.f - HORIZONTAL_PADDING * 2, 35.f},
            index
        );
        cell->setAnchorPoint({0, 0});
        std::string cellId = std::string(GEODE_MOD_ID) + "/color-cell-" + std::to_string(i);
        cell->setID(cellId.c_str());
        m_scrollLayer->m_contentLayer->addChild(cell);
        m_colorCells.push_back(cell);
    }
    m_scrollLayer->m_contentLayer->updateLayout();
    m_scrollLayer->m_contentLayer->setID("content-layer"_spr);
    this->m_mainLayer->addChild(m_scrollLayer);
    handleTouchPriority(this);
}


CCSize ColorPopup::getScrollLayerContentSize() const
{
    return m_scrollLayer->m_contentLayer->getContentSize();
}
void ColorPopup::onClose(CCObject *sender)
{
    m_setting->setValue(ColorList{m_colors}, m_setting);
    Popup::onClose(sender);
}

void ColorPopup::onAdd(CCObject*)
{
    m_colors.emplace_back("#FFFFFF", true, true);
    this->createList();
    this->updateAddButtonState();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
void ColorPopup::onPresets(CCObject*)
{
    // im killing myself
    auto presetPopup = PresetPopup::create();
    presetPopup->show();
}

ColorPopup* ColorPopup::create(std::vector<ColorEntry> colors, ColorListSettingNode *setting)
{
    auto ret = new ColorPopup();
    auto size = CCSize{320.f, 240.f};
    if (ret->initAnchored(
        size.width,
        size.height,
        std::move(colors),
        setting,
        // @geode-ignore(unknown-resource)
        "geode.loader/GE_square01.png"
    ))
    {
        ret->autorelease();
        return ret;
    }
    log::error("ehhh...");
    CC_SAFE_DELETE(ret);
    return ret;
}

void ColorPopup::updateAddButtonState() const
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