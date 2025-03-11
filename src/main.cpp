#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include "utils/customSettings.hpp"

// where's my estrogen pills 
// i still couldn't find them
// sorry km7

// Who could have thought that a simple mod would take so long to make?
// I guess it was a bit more complicated than I thought.
// But hey, at least I learned a lot along the way!
// Thanks to everyone who helped me out with this mod, I really appreciate it!
// And thanks to the Geode team for making this possible!
// I hope you enjoy the mod!
// And remember, if you have any questions or suggestions, feel free to reach out!
// I'm always open to feedback and improvements!
// And if you want to support me, feel free to donate! Maybe a pair of thigh highs?
// Or a nice dress? Or maybe some estrogen pills? Just kidding!
// But seriously, thanks for everything!
// I couldn't have done it without you km7dev!
// And if you want to see more mods like this, let me know!
// I'm always looking for new ideas and inspiration!
// And if you want to see more of my work, check out my other mods!
// I'm always working on something new and exciting!

// Transgender people are valid and deserve to be treated with respect and dignity.
// We are all human beings, and we all deserve to be treated with kindness and compassion.

using namespace geode::prelude;

$execute {
    // custom setting
    (void) Mod::get()->registerCustomSettingType("gaydient-colors", ColorListSetting::parse);

    // setting validation (old preset system subject to removal)
    Mod::get()->setSavedValue<bool>("enable-custom", Mod::get()->getSettingValue<std::string>("preset") == "Custom");
    listenForSettingChanges("preset", [](std::string value) { Mod::get()->setSavedValue<bool>("enable-custom", value == "Custom"); });
}
