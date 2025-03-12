#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PresetPopup : public geode::Popup<> {
protected:
    bool setup() override;
    CCMenu* m_presetsMenu;

    bool setupPresets();
public:
    static PresetPopup* create() {
        auto ret = new PresetPopup();
        if (ret->initAnchored(320.f, 240.f)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }
};