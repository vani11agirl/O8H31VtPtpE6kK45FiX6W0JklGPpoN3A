#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include "utils/customSettings.hpp"

// where's my estrogen pills 
// i still couldn't find them
// sorry km7

using namespace geode::prelude;

$execute {
    // custom setting
    (void) Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);

    // setting validation (old preset system subject to removal)
    listenForSettingChanges("preset", [](std::string value) { if (value == "Custom") { Mod::get()->setSavedValue<bool>("enable-custom", true); } else { Mod::get()->setSavedValue<bool>("enable-custom", false); }});
}