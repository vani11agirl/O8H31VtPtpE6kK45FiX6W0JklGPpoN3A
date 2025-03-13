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

template <>
struct matjson::Serialize<std::vector<int>>
{
    static Result<std::vector<int>> fromJson(matjson::Value const &value)
    {
        if (!value.isArray()) {
            return Err("Expected an array");
        }
        std::vector<int> result;
        for (auto const &elem : *value.asArray()) {
            if (!elem.isNumber()) {
                return Err("Expected a number");
            }
            result.push_back(elem.asInt().unwrapOrDefault());
        }
        return Ok(result);
    }

    static matjson::Value toJson(std::vector<int> const &value)
    {
        auto arr = matjson::makeObject({});
        for (auto const &elem : value) {
            arr.push(elem);
        }
        return arr;
    }
};

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
            ->setDefaultScaleLimits(.1f,.3f)
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

    setupPresets();

    return true;
}
//
bool PresetPopup::setupPresets()
{
    auto mod = Mod::get();

    std::filesystem::path presetsJson = mod->getResourcesDir() / "presets.json";

    GEODE_DEBUG("Reading presets from {}", presetsJson.string());

    // we read the file meow
    std::ifstream file(presetsJson);

    auto res = matjson::parse(file);
    if (!res) {
        GEODE_ERROR("Failed to parse presets.json! Oh no!");
        return false;
    }
    matjson::Value root = res.unwrapOr("Oops...");
    if (root == "Oops..." || !root.isObject()) {
        GEODE_ERROR("Failed to parse preset root! Oh no!");
        return false;
    }

    for (auto& [name, preset] : root)
    {
        if (!preset.isObject()) {
            GEODE_ERROR("Failed to parse a preset! Oh no!");
            continue;
        }

        std::vector<int> accent = preset["accent"].as<std::vector<int>>().unwrapOrDefault();

        // Button creation code goes here
        auto lightSpr = ButtonSprite::create(name.c_str(), "bigFont.fnt", "geode.loader/white-square.png", .6f);
        auto darkSpr = ButtonSprite::create(name.c_str(), "bigFont.fnt", "geode.loader/white-square.png", .6f);
        // spr->setScale(.7f);
        // Slightly darken the color for readability
        ccColor3B lighterColor = {
            static_cast<GLubyte>(accent[0] * 0.8),
            static_cast<GLubyte>(accent[1] * 0.8),
            static_cast<GLubyte>(accent[2] * 0.8)
        };
        ccColor3B darkerColor = {
            static_cast<GLubyte>(accent[0] * 0.5),
            static_cast<GLubyte>(accent[1] * 0.5),
            static_cast<GLubyte>(accent[2] * 0.5)
        };
        lightSpr->m_BGSprite->setColor(lighterColor);
        darkSpr->m_BGSprite->setColor(darkerColor);
        auto btn = CCMenuItemToggler::create(
            darkSpr, lightSpr,
            this,
            menu_selector(PresetPopup::onSelection)
        );
        btn->m_notClickable = true;
        btn->setUserObject("preset", CCString::create(name));
        m_presetsMenu->addChild(btn);
        m_presetsMenu->updateLayout();
        m_presetButtons.push_back(btn);
    }
    return true;
}


// ReSharper disable once CppMemberFunctionMayBeStatic
// ReSharper disable once CppMemberFunctionMayBeConst
// can you shut up for once
void PresetPopup::onSelection(CCObject* meow)
{
    const auto target = as<CCMenuItemToggler*, CCObject*>(meow);
    auto presetString = target->getUserObject("preset");
    GEODE_DEBUG("Selected preset: {}", typeinfo_cast<CCString*, CCObject*>(presetString)->getCString());

    for (const auto& btn : m_presetButtons)
    {
        if (btn->getUserObject("preset") == presetString)
        {
            btn->toggle(true);
            btn->selected();
            btn->m_notClickable = true;
            btn->setClickable(false);
            btn->updateSprite();
        } else
        {
            btn->toggle(false);
            btn->unselected();
            btn->m_notClickable = false;
            btn->setClickable(true);
            btn->updateSprite();
        }
    }
}
