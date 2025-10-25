#include "ColorSetting.hpp"

#include "../Utils.hpp"

bool ColorSettingNode::init(std::shared_ptr<ColorSettingValue> setting, float width) {
    if (!SettingValueNodeV3::init(setting, width))
        return false;

    m_sprite = CCSprite::createWithTexture(createTextureFromColors(setting->getValue()));
    m_sprite->setID("trail-color-preview");
    m_sprite->setRotation(90);
    m_sprite->setPosition(getButtonMenu()->getPosition() - CCPoint{getButtonMenu()->getContentWidth() / 2 - 3.5f, 0});
    this->addChild(m_sprite);

    auto btn = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("Edit"), [](auto) {

    });
    this->getButtonMenu()->addChild(btn);
    this->getButtonMenu()->setContentWidth(40);
    this->getButtonMenu()->setLayout(RowLayout::create());

    this->updateState(nullptr);

    return true;
}

void ColorSettingNode::updateState(CCNode* invoker) {
    SettingValueNodeV3::updateState(invoker);
    auto colors = getSetting()->getValue();
    m_sprite->setTexture(createTextureFromColors(colors));
    auto scale = CCPoint{20.f, 20.f} / m_sprite->getContentSize();
    m_sprite->setScaleX(scale.x);
    m_sprite->setScaleY(scale.y);
    getButtonMenu()->updateLayout();
}

ColorSettingNode* ColorSettingNode::create(std::shared_ptr<ColorSettingValue> setting, float width)  {
    auto ret = new ColorSettingNode();
    if (ret->init(setting, width)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

Result<std::shared_ptr<SettingV3>> ColorSettingValue::parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
    auto res = std::make_shared<ColorSettingValue>();
    auto root = checkJson(json, "ColorSetting");
    res->parseBaseProperties(key, modID, root);
    root.has("colors").into(res->m_colors);
    root.checkUnknownKeys();
    return root.ok(std::static_pointer_cast<SettingV3>(res));
}

SettingNodeV3* ColorSettingValue::createNode(float width) {
    return ColorSettingNode::create(std::static_pointer_cast<ColorSettingValue>(shared_from_this()), width);
}

$execute {
    (void)Mod::get()->registerCustomSettingType("color-setting", &ColorSettingValue::parse);
}