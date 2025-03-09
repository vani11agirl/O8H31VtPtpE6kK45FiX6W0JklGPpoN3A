#include "../bettercocos/CCMultiColorMotionStreak.hpp"
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/Geode.hpp>
#include "../utils/utils.hpp"
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

        std::vector<ccColor3B> stripeColors = {
            ccc3(91, 206, 250),
            ccc3(245, 169, 184),
            ccc3(255, 255, 255),
            ccc3(245, 169, 184),
            ccc3(91, 206, 250)
        }; // you can guess what it is

        auto texture = cocos2d::CCTextureCache::sharedTextureCache()->addImage("streak_05_001.png", true);


        auto newTrail = CCMultiColorMotionStreak::create(fastGetSetting<"fade-time", float>(), fastGetSetting<"min-seg", float>(), fastGetSetting<"trail-width", float>(), fastGetSetting<"opacity", int>(), stripeColors, texture, fastGetSetting<"disable-blending", bool>());
        newTrail->setID("new-trail"_spr);

        this->m_regularTrail = newTrail;
        this->m_regularTrail->retain();

        if (auto parentLayer = this->m_parentLayer) parentLayer->addChild(this->m_regularTrail);

        log::info("new addr: 0x{}", fmt::ptr(this->m_regularTrail));
    }
};
