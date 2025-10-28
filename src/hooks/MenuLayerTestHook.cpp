#include "../nodes/ColorStreak.hpp"
#include "../Utils.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "../settings/ColorSetting.hpp" // included for serialization

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(M, MenuLayer) {
    struct Fields {
        ColorStreak* streak;
    };

    bool init() {
        if (!MenuLayer::init()) return false;
        auto streak = ColorStreak::create(getSettingFast<"fade-time", float>(), getSettingFast<"min-seg", float>(), getSettingFast<"trail-width", float>(), getSettingFast<"stripe-colors", std::vector<std::string>>());
        if (!streak) return true;
        streak->setZOrder(m_menuGameLayer->m_playerObject->m_regularTrail->getZOrder());
        m_menuGameLayer->addChild(streak);
        m_fields->streak = streak;
        streak->setOpacity(getSettingFast<"trail-opacity", int>());

        schedule(schedule_selector(M::sched));

        return true;
    }

    void sched(float dt) {
        m_fields->streak->setPosition(m_menuGameLayer->m_playerObject->getPosition());
        m_menuGameLayer->m_playerObject->m_regularTrail->stopStroke();
        m_menuGameLayer->m_playerObject->m_waveTrail->stopStroke();
    }
};
