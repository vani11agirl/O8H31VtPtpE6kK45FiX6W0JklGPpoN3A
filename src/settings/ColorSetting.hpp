#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class ColorSettingValue : public SettingBaseValueV3<std::vector<std::string>> {
protected:
    std::vector<std::string> m_colors;
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json);

    SettingNodeV3* createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<std::vector<std::string>> {
    using SettingType = ColorSettingValue;
};

class ColorSettingNode : public SettingValueNodeV3<ColorSettingValue> {
    bool init(std::shared_ptr<ColorSettingValue> setting, float width);
    void updateState(CCNode* invoker) override;

protected:
    CCSprite* m_sprite = nullptr;

public:
    static ColorSettingNode* create(std::shared_ptr<ColorSettingValue> setting, float width);
};