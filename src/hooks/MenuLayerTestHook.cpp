#include "../nodes/ColorStreak.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(M, MenuLayer) {
    struct Fields {
        ColorStreak* streak;
    };

    bool init() {
        if (!MenuLayer::init()) return false;
        auto streak = ColorStreak::create(getSettingFast<"fade-time", float>(), getSettingFast<"min-seg", float>(), getSettingFast<"trail-width", float>(), {
            {91, 206, 250},
            {245, 169, 184},
            {255, 255, 255},
            {245, 169, 184},
            {91, 206, 250},
        });
        if (!streak) return true;

        addChild(streak);
        m_fields->streak = streak;
        streak->setOpacity(getSettingFast<"trail-opacity", int>());

        schedule(schedule_selector(M::sched));

        return true;
    }

    void sched(float dt) {
        m_fields->streak->setPosition(m_menuGameLayer->m_playerObject->getPosition());
    }
};