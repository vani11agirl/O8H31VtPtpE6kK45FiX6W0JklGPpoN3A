#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include "utils/utils.hpp"
#include "utils/customSettings.h"

// where's my estrogen pills 
// i need them now
// 

using namespace geode::prelude;

$execute {
    (void) Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);
}