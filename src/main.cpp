#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include "utils/customSettings.hpp"

// where's my estrogen pills 
// i need them now
// 

using namespace geode::prelude;

$execute {
    (void) Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);
}