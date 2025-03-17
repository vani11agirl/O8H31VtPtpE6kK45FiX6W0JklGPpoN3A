#include "Keybinds.hpp"

KeybindManager* KeybindManager::m_instance = nullptr;

void KeybindManager::registerBind(std::string id, std::function<void ()> callback) {
  if (m_binds.find(id) != m_binds.end()) return log::error("Keybind with id {} already exists", id);
  m_binds[id] = callback;
  m_savedBinds[id] = (geode::listenForSettingChanges<enumKeyCodes>(id, [this, id](enumKeyCodes v) {
    m_savedBinds[id] = v;
  }), Mod::get()->getSettingValue<enumKeyCodes>(id));
}

void KeybindManager::handleKey(enumKeyCodes key) {
  for (const auto& [id, bind] : m_binds) {
    if (m_savedBinds[id] == key) {
      if (bind == nullptr) log::error("Keybind with id {} has no callback", id);
      else bind();
    }
  }
}

enumKeyCodes KeybindManager::getBind(std::string id) {
  if (m_binds.find(id) == m_binds.end()) return enumKeyCodes::KEY_Unknown;
  return m_savedBinds[id];
}

KeybindManager* KeybindManager::get() {
    if (!m_instance) {
        m_instance = new KeybindManager();
    }
    return m_instance;
}

bool settingBind = false;

#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(GTKeyboardDispatcher, CCKeyboardDispatcher) {
  static void onModify(auto& self) {
    Result<> res = self.setHookPriorityPre("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", Priority::First);
    if (res.isErr()) {
      geode::log::warn("Failed to set hook priority: {}", res.unwrapErr());
    }
  }

  bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
    if (isKeyDown && !isKeyRepeat && settingBind) {
      if (key == KEY_Unknown) Notification::create("Unknown key pressed", NotificationIcon::Error, .5f)->show();
      else if (!CCScene::get()->getChildByType<BindPopup>(0) || key == KEY_Escape) settingBind = false;
      else {
        if (isKeyDown && !isKeyRepeat) {
          auto popup = CCScene::get()->getChildByType<BindPopup>(0);
          popup->setKey(key);
        }
        return true;
      }
    }
    if (isKeyDown && !isKeyRepeat) KeybindManager::get()->handleKey(key);
    return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat);
  }
};


// It crashes if the original function doesnt have it :trol:
std::string keyToString(enumKeyCodes key) {
  log::info("{}", (int) key);
  switch (key) {
    case KEY_None:      return "None";
    case KEY_C:         return "C";
    case KEY_Multiply:  return "Mul";
    case KEY_Divide:    return "Div";
    case KEY_OEMPlus:   return "Plus";
    case KEY_OEMMinus:  return "Minus";
    case CONTROLLER_LTHUMBSTICK_DOWN:   return "L_THUMBSTICK_DOWN";
    case CONTROLLER_LTHUMBSTICK_LEFT:   return "L_THUMBSTICK_LEFT";
    case CONTROLLER_LTHUMBSTICK_UP:     return "L_THUMBSTICK_UP";
    case CONTROLLER_LTHUMBSTICK_RIGHT:  return "L_THUMBSTICK_RIGHT";
    case CONTROLLER_RTHUMBSTICK_RIGHT:  return "R_THUMBSTICK_RIGHT";
    case CONTROLLER_RTHUMBSTICK_LEFT:   return "L_THUMBSTICK_RIGHT";
    case CONTROLLER_RTHUMBSTICK_DOWN:   return "L_THUMBSTICK_DOWN";
    case CONTROLLER_RTHUMBSTICK_UP:     return "L_THUMBSTICK_UP";
    case static_cast<enumKeyCodes>(-1): return "Unk";
    default: return CCKeyboardDispatcher::get()->keyToString(key);
  }
}

void BindPopup::onClose(CCObject *sender) {
  settingBind = false;
  m_setting->setValue(m_key, m_setting);
  this->removeFromParent();
}

bool BindPopup::setup(KeybindSettingNode * setting) {
  m_setting = setting;
  m_key = setting->getValue();
  settingBind = true;

  setID("bind-popup");
  m_bgSprite->setID("background");

  auto lab = CCLabelBMFont::create("Press a key to bind,\nor press escape to cancel.", "chatFont.fnt");
  lab->setAnchorPoint({.5f, .5f});
  lab->setAlignment(kCCTextAlignmentCenter);
  lab->setID("info-label");
  m_mainLayer->addChildAtPosition(lab, Anchor::Center);

  return true;
}

void BindPopup::setKey(enumKeyCodes key) {
  m_key = key;
  settingBind = false;
  onClose(nullptr);
}

BindPopup* BindPopup::create(KeybindSettingNode * setting) {
  auto ret = new BindPopup();
  if (ret && ret->initAnchored(200.f, 100.f, setting)) {
      ret->autorelease();
      return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

Result<std::shared_ptr<SettingV3>> KeybindSetting::parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
  auto res = std::make_shared<KeybindSetting>();

  auto root = checkJson(json, "keybind");

  
  res->parseBaseProperties(key, modID, root);

  root.checkUnknownKeys();

  return root.ok(std::static_pointer_cast<SettingV3>(res));
}

SettingNodeV3 *KeybindSetting::createNode(float width) {
  return KeybindSettingNode::create(
      std::static_pointer_cast<KeybindSetting>(shared_from_this()),
      width
  );
}

void KeybindSettingNode::removeBind(CCObject* sender) {
  this->setValue(KEY_None, this);
}

bool KeybindSettingNode::init(std::shared_ptr<KeybindSetting> setting, float width) {
  if (!SettingValueNodeV3::init(setting, width))
      return false;
  m_btnSpr = ButtonSprite::create(keyToString( getValue()).c_str());
  m_btnSpr->updateBGImage("GJ_button_05.png");
  
  m_btn = CCMenuItemSpriteExtra::create(
    m_btnSpr, this, menu_selector(KeybindSettingNode::onClick)
  );

  m_btnSpr->setScale(.72f);

  auto resetBtn = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png"),
    this, menu_selector(KeybindSettingNode::removeBind)
  );
  static_cast<CCSprite*>(resetBtn->getNormalImage())->setScale(.75f);
  this->getButtonMenu()->addChildAtPosition(resetBtn, Anchor::Right, {-12.f, 0});
  this->getButtonMenu()->addChildAtPosition(m_btn, Anchor::Right, {-m_btn->getContentSize().width / 2 - 21.f, 0});
  this->getButtonMenu()->setContentWidth(60);

  this->updateState(nullptr);
  
  return true;
}

void KeybindSettingNode::updateState(CCNode* invoker) {
  SettingValueNodeV3::updateState(invoker);

  m_btnSpr->setString(keyToString(getValue()).c_str());
  this->getButtonMenu()->updateLayout();

  static_cast<AnchorLayoutOptions*>(m_btn->getLayoutOptions())->setOffset({-m_btn->getContentSize().width / 2 - 21.f, 0});

  auto shouldEnable = this->getSetting()->shouldEnable();

  m_btn->setEnabled(shouldEnable);
  m_btn->setCascadeColorEnabled(true);
  m_btn->setCascadeOpacityEnabled(true);
  m_btn->setOpacity(shouldEnable ? 255 : 155);
  m_btn->setColor(shouldEnable ? ccWHITE : ccGRAY);
}

void KeybindSettingNode::onClick(CCObject* sender) {
  auto popup = BindPopup::create(this);
  popup->show();
}

KeybindSettingNode* KeybindSettingNode::create(std::shared_ptr<KeybindSetting> setting, float width)  {
  auto ret = new KeybindSettingNode();
  if (ret && ret->init(setting, width)) {
      ret->autorelease();
      return ret;
  }
  CC_SAFE_DELETE(ret);
  return nullptr;
}

$execute {
  (void)Mod::get()->registerCustomSettingType("keybind", &KeybindSetting::parse);
}