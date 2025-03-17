#include "../bettercocos/CCMultiColorMotionStreak.hpp"
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/Geode.hpp>
#include "../colorTypes.hpp"
#include "../utils/utils.hpp"
#include "../utils/color.hpp"
#include "../utils/PresetHandler.hpp"
#include "../utils/Keybinds.hpp"
// This is included so we can actually get the setting, despite it thinking this is unused
// ReSharper disable once CppUnusedIncludeDirective
#include "../utils/customSettings.hpp"
#include "ccTypes.h"
#include <matjson.hpp>

// What are you on to.
#define GEODE_ERROR log::error
#define GEODE_WARN log::warn
#define GEODE_INFO log::info
#define GEODE_DEBUG log::debug

using namespace geode::prelude;

class $modify(gErpaxdumjam4dumge, PlayerObject)
{

    struct Fields {
        bool m_cantBeWoke = false;
    };

    static void onModify(ModifyBase<ModifyDerive<gErpaxdumjam4dumge, PlayerObject>>& self) {
        (void)self.setHookPriorityAfterPost("PlayerObject::setupStreak", "hiimjustin000.more_icons");
    }

    static std::vector<ccColor3B> colorsForPreset(const std::string& preset) {
        if (preset == "Custom") {
            std::vector<ccColor3B> ret = {};
            auto colors = fastGetSetting<"stripe-colors", ColorList>().colors;
            for (auto entry : colors) {
                ret.push_back(custom::utils::color::ColorUtils::hexToColor3B(entry.m_hex));
            }
            return ret;
        } else
        {
            auto presetObj = PresetHandler::get()->getPresets()[preset];
            auto presetColors = presetObj.colors;
            if (presetColors.empty()) {
                GEODE_ERROR("Failed to fetch preset colors! Oh no!");
                return {ccColor3B(255, 255, 255)};
            }
            
            std::vector<ccColor3B> ret = {};
            for (size_t i = 0; i < presetColors.size(); i++) {
                ccColor3B color = {
                    static_cast<GLubyte>(presetColors[i].r),
                    static_cast<GLubyte>(presetColors[i].g),
                    static_cast<GLubyte>(presetColors[i].b),
                };
                ret.push_back(color);
            }
            if (ret.empty()) {
                GEODE_ERROR("Failed to parse preset colors! Oh no!");
                return {ccColor3B(255, 255, 255)};
            }
            return ret;
        }
    }

    $override void resetStreak() {
        log::debug("using my own resetstreak impl");

        if (!levelFlipping()) {
            m_fadeOutStreak = true;
            m_regularTrail->reset();

            if (m_waveTrail) {
                float waveOffsetX = -5.0f;
                CCPoint wavePos = getPosition() + CCPoint(waveOffsetX, 0.0f);

                m_waveTrail->m_currentPoint = wavePos;
                m_waveTrail->setPosition(getPosition());
                m_waveTrail->setOpacity(255);
                m_waveTrail->stopAllActions();
            }
        }
    }


    $override void activateStreak() {
        if ( m_fields->m_cantBeWoke ) return GEODE_DEBUG(":pensive:");

        log::debug("activateStreak() called");
        if (!levelFlipping() && !GameManager::sharedState()->m_editorEnabled && !m_isHidden) {
            m_fadeOutStreak = true;
            if ( !m_fields->m_cantBeWoke ) m_regularTrail->resumeStroke();

            if (m_isDart) {
                m_waveTrail->m_currentPoint = getPosition();
                m_waveTrail->stopAllActions();
                m_waveTrail->setOpacity(255);
                m_waveTrail->resumeStroke();
            }
        }
    }

    // ReSharper disable once CppHidingFunction
    $override void setupStreak() {
        PlayerObject::setupStreak();

        // Should be it...
        Loader::get()->queueInMainThread([this] {
            if (auto pl = PlayLayer::get()) if (!(pl->m_player1 == this || pl->m_player2 == this)) return GEODE_WARN("Sorry...");
        });
        
        if (m_regularTrail) {
            m_regularTrail->removeFromParent();
            m_regularTrail = nullptr;
        }

        std::vector<ccColor3B> stripeColors = colorsForPreset(Mod::get()->getSavedValue<std::string>("preset"));

        auto texture = CCTextureCache::sharedTextureCache()->addImage("streak_05_001.png", true);

        auto newTrail = CCMultiColorMotionStreak::create(fastGetSetting<"fade-time", float>(), fastGetSetting<"min-seg", float>(), fastGetSetting<"trail-width", float>(), fastGetSetting<"opacity", int>(), stripeColors, texture, fastGetSetting<"disable-blending", bool>());
        newTrail->setID("new-trail"_spr);
        newTrail->m_fMaxSeg = fastGetSetting<"max-seg", float>();

        m_regularTrail = newTrail;
        m_regularTrail->retain();

        if (auto parentLayer = m_parentLayer) parentLayer->addChild(m_regularTrail);

        log::info("new addr: 0x{}", fmt::ptr(m_regularTrail));

        KeybindManager::get()->registerBind("trail-bind", [this]() {
            if (m_regularTrail) {
                m_fields->m_cantBeWoke = !m_fields->m_cantBeWoke;
                if (m_fields->m_cantBeWoke) m_regularTrail->reset(); 
                log::debug("status: {}", m_fields->m_cantBeWoke);
            } else {
                log::debug("oops...");
            }
        });
    }
};
