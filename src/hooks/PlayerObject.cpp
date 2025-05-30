// ReSharper disable CppHidingFunction
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
#include "modify/Modify.hpp"
#include <matjson.hpp>

using namespace geode::prelude;

// Global variable my beloved
static bool cantBeWoke = false;

class $modify(GTPlayerObject, PlayerObject)
{
    struct Fields {
        CCMultiColorMotionStreak *m_wokeTrail = nullptr;
    };

    static void onModify(ModifyBase<ModifyDerive<GTPlayerObject, PlayerObject>>& self) {
        (void)self.setHookPriorityAfterPost("PlayerObject::setupStreak", "hiimjustin000.more_icons");
    }

    static std::vector<ccColor3B> colorsForPreset(const std::string& preset) {
        if (preset == "Custom") {
            std::vector<ccColor3B> ret = {};
            for (const auto colors = fastGetSetting<"stripe-colors", ColorList>().colors; const auto& entry : colors) {
                ret.push_back(custom::utils::color::ColorUtils::hexToColor3B(entry.m_hex));
            }
            return ret;
        } else
        {
            const auto presetObj = PresetHandler::get()->getPresets()[preset];
            const auto presetColors = presetObj.colors;
            if (presetColors.empty()) {
                log::error("Failed to fetch preset colors! Oh no!");
                return {ccc3(255, 255, 255)};
            }
            
            std::vector<ccColor3B> ret = {};
            for (auto [r, g, b] : presetColors) {
                ccColor3B color = {
                    static_cast<GLubyte>(r),
                    static_cast<GLubyte>(g),
                    static_cast<GLubyte>(b),
                };
                ret.push_back(color);
            }
            if (ret.empty()) {
                log::error("Failed to parse preset colors! Oh no!");
                return {ccc3(255, 255, 255)};
            }
            return ret;
        }
    }

    $override void resetStreak() {
        if (!levelFlipping()) {
            m_fadeOutStreak = true;
            m_fields->m_wokeTrail->reset(); // just so that you don't comment this hook out, there's this midhook here
            m_regularTrail->reset();

            if (m_waveTrail) {
                constexpr float waveOffsetX = -5.0f;
                const CCPoint wavePos = getPosition() + CCPoint(waveOffsetX, 0.0f);

                m_waveTrail->m_currentPoint = wavePos;
                m_waveTrail->setPosition(getPosition());
                m_waveTrail->setOpacity(255);
                m_waveTrail->stopAllActions();
            }
        }
    }


    $override void activateStreak() {
        if (cantBeWoke) return PlayerObject::activateStreak();
        
        // log::debug("activateStreak() called");
        if (!levelFlipping() && !GameManager::sharedState()->m_editorEnabled && !m_isHidden) {
            m_fadeOutStreak = true;
            m_fields->m_wokeTrail->resumeStroke();

            if (m_isDart) {
                m_waveTrail->m_currentPoint = getPosition();
                m_waveTrail->stopAllActions();
                m_waveTrail->setOpacity(255);
                m_waveTrail->resumeStroke();
            }
        }
    }

    $override void update(const float dt) override
    {
        PlayerObject::update(dt);

        if (cantBeWoke) return; // DO NOT LOG, this is update after all

        if (m_fields->m_wokeTrail) {
            m_fields->m_wokeTrail->setPosition(getPosition());
        }
    }

    // ReSharper disable once CppHidingFunction
    $override void setupStreak() {
        PlayerObject::setupStreak();

        const std::vector<ccColor3B> stripeColors = colorsForPreset(Mod::get()->getSavedValue<std::string>("preset"));

        const auto texture = CCTextureCache::sharedTextureCache()->addImage("streak_05_001.png", true);

        const auto newTrail = CCMultiColorMotionStreak::create(fastGetSetting<"fade-time", float>(), fastGetSetting<"min-seg", float>(), fastGetSetting<"trail-width", float>(), fastGetSetting<"opacity", int>(), stripeColors, texture, fastGetSetting<"disable-blending", bool>());
        newTrail->setID("new-trail"_spr);
        newTrail->m_fMaxSeg = fastGetSetting<"max-seg", float>();
        if (cantBeWoke) newTrail->stopStroke();
        else m_regularTrail->stopStroke();
        // m_streakStrokeWidth = fastGetSetting<"trail-width", float>();

        m_fields->m_wokeTrail = newTrail;
        m_fields->m_wokeTrail->retain();

        if (const auto parentLayer = m_parentLayer) parentLayer->addChild(newTrail);

        log::debug("New trail made at address 0x{}", fmt::ptr(m_fields->m_wokeTrail));
    }
};

$on_mod(Loaded) {
    KeybindManager::get()->registerBind("trail-bind", []() {
        cantBeWoke = !cantBeWoke;
        std::vector<GTPlayerObject *> players = {};
        if (const auto pl = PlayLayer::get()) {
            players.push_back(typeinfo_cast<GTPlayerObject *>(pl->m_player1));
            players.push_back(typeinfo_cast<GTPlayerObject *>(pl->m_player2));
        }
        if (const auto ml = CCScene::get()->getChildByType<MenuLayer *>(0)) {
            if (ml->m_menuGameLayer) players.push_back(typeinfo_cast<GTPlayerObject *>(ml->m_menuGameLayer->m_playerObject));
        }
        for (const auto& plr : players) {
            if (!plr)
                continue;
            if (plr->m_regularTrail) {
                if (cantBeWoke) {
                    plr->m_fields->m_wokeTrail->stopStroke();
                    // plr->m_fields->m_wokeTrail->reset();

                    plr->m_regularTrail->resumeStroke();
                } else {
                    plr->m_fields->m_wokeTrail->resumeStroke();

                    plr->m_regularTrail->stopStroke();
                    // plr->m_regularTrail->reset();
                }
                log::debug("status: {}", cantBeWoke);
            }/* else {
                log::debug("oops...");
            }*/
        }
    });
}
