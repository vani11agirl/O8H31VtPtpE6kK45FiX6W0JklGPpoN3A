#pragma once

#include "ui/Popup.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class KeybindManager {
protected:
  static KeybindManager *m_instance;
  std::map<std::string, std::function<void()>> m_binds = {};
  std::map<std::string, enumKeyCodes> m_savedBinds = {};

public:
  static KeybindManager *get();

  void registerBind(std::string id, std::function<void()> callback);
  void handleKey(enumKeyCodes key);
  enumKeyCodes getBind(std::string id);
};

#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>

using namespace geode::prelude;

template <>
struct matjson::Serialize<enumKeyCodes> {
    static Result<enumKeyCodes> fromJson(matjson::Value const &value) {
      return Ok((enumKeyCodes) value.asInt().unwrapOr(-1));
    }

    static matjson::Value toJson(enumKeyCodes const &value) {
        return (int) value;
    }
};

class KeybindSetting : public SettingBaseValueV3<enumKeyCodes> {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json);
    
    SettingNodeV3* createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<enumKeyCodes> {
    using SettingType = KeybindSetting;
};

class KeybindSettingNode : public SettingValueNodeV3<KeybindSetting> {
protected:
    CCMenuItemSpriteExtra *m_btn = nullptr;
    ButtonSprite *m_btnSpr = nullptr;
    bool init(std::shared_ptr<KeybindSetting> setting, float width);
    
    void updateState(CCNode* invoker) override;
    void onClick(CCObject* sender);
    void removeBind(CCObject* sender);

public:
    static KeybindSettingNode* create(std::shared_ptr<KeybindSetting> setting, float width);
};

class BindPopup : public Popup<KeybindSettingNode *> {
  protected:
    KeybindSettingNode * m_setting;
    enumKeyCodes m_key;

    void onClose(CCObject *sender) override;
    bool setup(KeybindSettingNode * setting) override;
  public:
    void setKey(enumKeyCodes key);

    static BindPopup* create(KeybindSettingNode * setting);
};