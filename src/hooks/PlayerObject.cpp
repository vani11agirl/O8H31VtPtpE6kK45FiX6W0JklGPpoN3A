#include "../bettercocos/CCMultiColorMotionStreak.hpp"
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/Geode.hpp>
#include "../colorTypes.hpp"
#include "../utils/utils.hpp"
#include "../utils/color.hpp"
// This is included so we can actually get the setting, despite it thinking its unused
#include "../utils/customSettings.hpp"
#include "ccTypes.h"

using namespace geode::prelude;

class $modify(gErpaxdumjam4dumge, PlayerObject)
{
    static void onModify(ModifyBase<ModifyDerive<gErpaxdumjam4dumge, PlayerObject>>& self) {
        (void)self.setHookPriorityAfterPost("PlayerObject::setupStreak", "hiimjustin000.more_icons");
    }

    std::vector<ccColor3B> colorsForPreset(std::string preset) {
        if (preset == "Custom") {
            std::vector<ccColor3B> ret = {};
            auto colors = fastGetSetting<"stripe-colors", ColorList>().colors;
            for (auto entry : colors) {
                ret.push_back(custom::utils::color::ColorUtils::hexToColor3B(entry.m_hex));
            }
            return ret;
        }
	    if (preset == "Bisexual") return {
            ccc3(214, 2, 112),
            ccc3(214, 2, 112),
            ccc3(155, 79, 150),
            ccc3(0, 56, 168),
            ccc3(0, 56, 168),
        };
		if (preset == "Pansexual") return {
            ccc3(255, 33, 140),
            ccc3(255, 216, 0),
            ccc3(33, 177, 255),
        };
		if (preset == "Transgender") return {
            ccc3(91, 206, 250),
            ccc3(245, 169, 184),
            ccc3(255, 255, 255),
            ccc3(245, 169, 184),
            ccc3(91, 206, 250),
        };
		if (preset == "Nonbinary") return {
            ccc3(252, 244, 52),
            ccc3(255, 255, 255),
            ccc3(156, 89, 209),
            ccc3(44, 44, 44),
        };
		if (preset == "Genderfluid") return {
            ccc3(255, 118, 164),
            ccc3(255, 255, 255),
            ccc3(192, 17, 215),
            ccc3(0, 0, 0),
            ccc3(47, 60, 190),
        };
		if (preset == "Genderqueer") return {
            ccc3(181, 126, 220),
            ccc3(255, 255, 255),
            ccc3(74, 129, 35),
        };
		if (preset == "Lesbian") return {
            ccc3(213, 45, 0),
            ccc3(239, 118, 39),
            ccc3(255, 154, 86),
            ccc3(255, 255, 255),
            ccc3(209, 98, 164),
            ccc3(181, 86, 144),
            ccc3(163, 2, 98),
        };
		if (preset == "Aromantic") return {
            ccc3(61, 165, 66),
            ccc3(167, 211, 121),
            ccc3(255, 255, 255),
            ccc3(169, 169, 169),
            ccc3(0, 0, 0),
        };
		if (preset == "Asexual") return {
            ccc3(0, 0, 0),
            ccc3(163, 163, 163),
            ccc3(255, 255, 255),
            ccc3(128, 0, 128),
        };
		if (preset == "Agender") return {
            ccc3(0, 0, 0),
            ccc3(188, 196, 199),
            ccc3(255, 255, 255),
            ccc3(183, 246, 132),
            ccc3(255, 255, 255),
            ccc3(188, 196, 199),
            ccc3(0, 0, 0),

        };
		if (preset == "Demiboy") return {
            ccc3(127, 127, 127),
            ccc3(188, 196, 199),
            ccc3(255, 255, 255),
            ccc3(157, 215, 234),
            ccc3(255, 255, 255),
            ccc3(188, 196, 199),
            ccc3(127, 127, 127),
        };
		if (preset == "Demigirl") return {
            ccc3(127, 127, 127),
            ccc3(188, 196, 199),
            ccc3(253, 173, 200),
            ccc3(255, 255, 255),
            ccc3(253, 173, 200),
            ccc3(188, 196, 199),
            ccc3(127, 127, 127),
        };
		if (preset == "Polysexual") return {
            ccc3(247, 20, 186),
            ccc3(1, 214, 106),
            ccc3(21, 148, 246),
        };
		if (preset == "Abrosexual") return {
            ccc3(117, 202,145),
            ccc3(179, 228, 199),
            ccc3(255, 255, 255),
            ccc3(230, 149, 181),
            ccc3(217, 68, 108),
        };
        // just gon use pride as default
        return {
            ccc3(228, 3, 3),
            ccc3(255, 140, 0),
            ccc3(255, 237, 0),
            ccc3(0, 128, 38),
            ccc3(0, 76, 255),
            ccc3(115, 41, 130),
        };
    }

    void setupStreak() {
        PlayerObject::setupStreak();

        if (this->m_regularTrail) {
            this->m_regularTrail->removeFromParent();
            this->m_regularTrail = nullptr;
        }

        std::vector<ccColor3B> stripeColors = colorsForPreset(fastGetSetting<"preset", std::string>());

        auto texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage("streak_05_001.png", true);

        auto newTrail = CCMultiColorMotionStreak::create(fastGetSetting<"fade-time", float>(), fastGetSetting<"min-seg", float>(), fastGetSetting<"trail-width", float>(), fastGetSetting<"opacity", int>(), stripeColors, texture, fastGetSetting<"disable-blending", bool>());
        newTrail->setID("new-trail"_spr);

        this->m_regularTrail = newTrail;
        this->m_regularTrail->retain();

        if (auto parentLayer = this->m_parentLayer) parentLayer->addChild(this->m_regularTrail);

        log::info("new addr: 0x{}", fmt::ptr(this->m_regularTrail));
    }
};
