#include "../bettercocos/CCMultiColorMotionStreak.hpp"
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/Geode.hpp>
#include "../colorTypes.hpp"
#include "../utils/utils.hpp"
#include "../utils/color.hpp"
// This is included so we can actually get the setting, despite it thinking its unused
#include "../utils/customSettings.hpp"

using namespace geode::prelude;

class $modify(gErpaxdumjam4dumge, PlayerObject)
{
    static void onModify(ModifyBase<ModifyDerive<gErpaxdumjam4dumge, PlayerObject>>& self) {
        (void)self.setHookPriorityAfterPost("PlayerObject::setupStreak", "hiimjustin000.more_icons");
    }

    void setupStreak() {
        PlayerObject::setupStreak();

        if (this->m_regularTrail) {
            this->m_regularTrail->removeFromParent();
            this->m_regularTrail = nullptr;
        }

        std::vector<ccColor3B> stripeColors;
        auto colors = fastGetSetting<"stripe-colors", ColorList>().colors;
        if (colors.empty()) stripeColors = {
            ccc3(91, 206, 250),
            ccc3(245, 169, 184),
            ccc3(255, 255, 255),
            ccc3(245, 169, 184),
            ccc3(91, 206, 250)
        }; // you can guess what it is
        for (auto entry : colors) {
            stripeColors.push_back(custom::utils::color::ColorUtils::hexToColor3B(entry.m_hex));
        }

        auto texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage("streak_05_001.png", true);

        auto newTrail = CCMultiColorMotionStreak::create(fastGetSetting<"fade-time", float>(), fastGetSetting<"min-seg", float>(), fastGetSetting<"trail-width", float>(), fastGetSetting<"opacity", int>(), stripeColors, texture, fastGetSetting<"disable-blending", bool>());
        newTrail->setID("new-trail"_spr);

        this->m_regularTrail = newTrail;
        this->m_regularTrail->retain();

        if (auto parentLayer = this->m_parentLayer) parentLayer->addChild(this->m_regularTrail);

        log::info("new addr: 0x{}", fmt::ptr(this->m_regularTrail));
    }
};
