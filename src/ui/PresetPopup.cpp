#include "PresetPopup.hpp"
#include <matjson.hpp>
#define GEODE_ERROR log::error
#define GEODE_INFO log::info
#define GEODE_DEBUG log::debug

/**
 * @brief Makes a preset button!
 * @param __label The button label, as a string.
 * @param __colorsArray Array of colors.
 * @param __representation Accent/representation color. Rendering the individuals that way would take way too much effort.
 *
 * @note I hate this.
 */
#define PRESET_BUTTON(__label, __colorsArray, __representation) \
CCArray* __colorsArray = new CCArray(); \
ccColor3B __representation; \
auto spr = ButtonSprite::create(__label); \
std::string nodeId = fmt::format("{}{}", static_cast<char>(std::tolower(__label[0])), __label.substr(1)); \
spr->setID(nodeId.c_str()); \
spr->setColor(__representation); \
auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(nullptr)); \
btn->setUserObject("preset", CCString::create(__label)); \
m_presetsMenu->addChild(btn);

// TODO: The rest of this.



bool PresetPopup::setup()
{

    // convenience function provided by Popup
    // for adding/setting a title to the popup
    this->setTitle("Hi mom!");

    auto presetsContainer = CCNode::create();
    presetsContainer->setContentSize(CCSizeMake(300, 100));
    presetsContainer->setAnchorPoint(ccp(.5f, .5f));

    auto presetsBackground = CCScale9Sprite::create("square02b_001.png");
    presetsBackground->setColor(ccc3(0, 0, 0));
    presetsBackground->setOpacity(75);
    presetsBackground->setScale(.3f);
    presetsBackground->setContentSize(presetsContainer->getContentSize() / presetsBackground->getScale());
    presetsContainer->addChildAtPosition(presetsBackground, Anchor::Center);

    m_presetsMenu = CCMenu::create();
    m_presetsMenu->setContentSize(presetsContainer->getContentSize() - ccp(10, 10));
    m_presetsMenu->setLayout(
        RowLayout::create()
            ->setDefaultScaleLimits(1.f, 1.f)
            ->setGrowCrossAxis(true)
            ->setCrossAxisOverflow(true)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisAlignment(AxisAlignment::Center)
        );
    presetsContainer->addChildAtPosition(m_presetsMenu, Anchor::Center);
    m_mainLayer->addChildAtPosition(presetsContainer, Anchor::Top, ccp(0, -85));

    auto okSpr = ButtonSprite::create("OK");
    okSpr->setScale(.7f);
    auto okBtn = CCMenuItemSpriteExtra::create(
        okSpr,
        this,
        menu_selector(PresetPopup::onClose)
        );

    m_buttonMenu->addChildAtPosition(okBtn, Anchor::Bottom, ccp(0, 20));

    return true;
}
//
bool PresetPopup::setupPresets()
{
    auto mod = Mod::get();

    std::filesystem::path presetsJson = mod->getResourcesDir() / "presets.json";

    // we read the file meow
    std::ifstream file(presetsJson);
    if (!file.is_open()) {
        GEODE_ERROR("Failed to open presets.json");
        return false;
    }
    std::string jsonString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    auto res = matjson::parse(jsonString);
    if (!res) {
        GEODE_ERROR("Failed to parse presets.json! Oh no!");
        return false;
    }
    matjson::Value root = res.unwrapOr("Oops...");
    if (root == "Oops..." || !root.isObject()) {
        GEODE_ERROR("Failed to parse preset root! Oh no!");
        return false;
    }

    for (auto& preset : root["presets"].asArray())
    {
        if (!preset.isObject()) {
            GEODE_ERROR("Failed to parse a preset! Oh no!");
            continue;
        }
        std::string name = preset["name"];
        std::vector<int> accent = preset["accent"];

        // Button creation code goes here
        auto spr = ButtonSprite::create(name);
        spr->setScale(.7f);
        // Slightly darken the color for readability
        ccColor3B color = {
            static_cast<GLubyte>(accent[0] * 0.8),
            static_cast<GLubyte>(accent[1] * 0.8),
            static_cast<GLubyte>(accent[2] * 0.8)
        };
        spr->setColor(color);
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            nullptr // TODO
        );
        btn->setUserObject("preset", CCString::create(name));
        m_presetsMenu->addChild(btn);
        m_presetsMenu->updateLayout();
    }

    return true;
}