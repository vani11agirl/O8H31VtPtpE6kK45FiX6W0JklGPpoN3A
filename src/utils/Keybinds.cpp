#include "Keybinds.hpp"

KeybindManager* KeybindManager::m_instance = nullptr;

void KeybindManager::init() {
    
}

void KeybindManager::registerBind(std::string id, enumKeyCodes key, std::function<void ()> callback) {
  if (m_binds.find(id) != m_binds.end()) return log::error("Keybind with id {} already exists", id);
  m_binds[id] = {key, callback};
}

void KeybindManager::unregisterBind(std::string id) {
  if (m_binds.find(id) == m_binds.end()) return log::error("Keybind with id {} does not exist", id);
  m_binds.erase(id);
}

void KeybindManager::handleKey(enumKeyCodes key) {
  for (const auto& [id, bind] : m_binds) {
    if (bind.first == key) {
      if (bind.second == nullptr) log::error("Keybind with id {} has no callback", id);
      else bind.second();
    }
  }
}

enumKeyCodes KeybindManager::getBind(std::string id) {
  if (m_binds.find(id) == m_binds.end()) return enumKeyCodes::KEY_Unknown;
  return m_binds[id].first;
}

KeybindManager* KeybindManager::get() {
    if (!m_instance) {
        m_instance = new KeybindManager();
        m_instance->init();
    }
    return m_instance;
}

#include <Geode/modify/CCKeyboardDispatcher.hpp>
class $modify(GTKeyboardDispatcher, CCKeyboardDispatcher) {
  bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
    if (isKeyDown && !isKeyRepeat) KeybindManager::get()->handleKey(key);
    return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat);
  }
};