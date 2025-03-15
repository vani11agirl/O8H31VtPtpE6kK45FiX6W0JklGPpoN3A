#pragma once

#include "cocos/robtop/keyboard_dispatcher/CCKeyboardDelegate.h"
#include <Geode/Geode.hpp>
#include <functional>

using namespace geode::prelude;

class KeybindManager {
  protected:
    static KeybindManager* m_instance;
    std::map<std::string, std::pair<enumKeyCodes, std::function<void ()>>> m_binds = {};
    void init();
  public:
    static KeybindManager* get();

    void registerBind(std::string id, enumKeyCodes key, std::function<void ()> callback);
    void unregisterBind(std::string id);
    void handleKey(enumKeyCodes key);
    enumKeyCodes getBind(std::string id);
};