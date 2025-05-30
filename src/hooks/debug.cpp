//
// Suffering started on 3/14/2025.
//
// ReSharper disable CppHidingFunction
// ReSharper disable CppMemberFunctionMayBeConst
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#if false
using namespace geode::prelude;

class $modify(DebugPlayerObject, PlayerObject)
{
    $override void resetStreak() {
        log::debug("using my own resetstreak impl");

        CCPoint pos = this->getPosition();

        if (!this->levelFlipping()) {
            this->m_fadeOutStreak = true;
            this->m_regularTrail->reset();

            if (this->m_waveTrail) {
                float waveOffsetX = -5.0f;
                CCPoint wavePos = pos + CCPoint(waveOffsetX, 0.0f);

                this->m_waveTrail->m_currentPoint = wavePos;
                this->m_waveTrail->setPosition(pos);
                this->m_waveTrail->setOpacity(255);
                this->m_waveTrail->stopAllActions();
            }
        }
    }


    $override void activateStreak() {
        log::debug("activateStreak() called");
        if (!this->levelFlipping() && !GameManager::sharedState()->m_editorEnabled && !this->m_isHidden) {
            this->m_fadeOutStreak = true;
            this->m_regularTrail->resumeStroke();

            if (this->m_isDart) {
                m_waveTrail->m_currentPoint = this->getPosition();
                m_waveTrail->stopAllActions();
                m_waveTrail->setOpacity(255);
                m_waveTrail->resumeStroke();
            }
        }
    }

    $override void addAllParticles()
    {
        log::debug("adding particles");
        this->m_parentLayer->addChild(this->m_playerGroundParticles, 39);
        this->m_parentLayer->addChild(this->m_ufoClickParticles, 39);
        this->m_parentLayer->addChild(this->m_dashParticles, 39);
        this->m_parentLayer->addChild(this->m_robotBurstParticles, 39);
        this->m_parentLayer->addChild(this->m_trailingParticles, 39);
        this->m_parentLayer->addChild(this->m_shipClickParticles, 39);
        this->m_parentLayer->addChild(this->m_vehicleGroundParticles, 61);
        this->m_parentLayer->addChild(this->m_landParticles0, 61);
        this->m_parentLayer->addChild(this->m_landParticles1, 61);
        this->m_parentLayer->addChild(this->m_swingBurstParticles1, 39);
        this->m_parentLayer->addChild(this->m_swingBurstParticles2, 39);
    }

    $override void animatePlatformerJump(float jumpFactor) {
        if (!m_holdingLeft && !m_holdingRight &&
            !m_platformerMovingLeft && !m_platformerMovingRight &&
            !m_isShip && !m_isBird && !m_isDart && !m_isSwing &&
            !m_isBall && !m_isRobot && !m_isSpider) {

            int iterations = 1;
            do {
                float rotation = fabsf(this->getRotation());

                float scaleX1 = 0.8f;
                float scaleY1 = jumpFactor * 1.35f;
                float scaleX2 = 1.1f;
                float scaleY2 = 0.9f;

                if ((static_cast<int>(rotation) - 46 < 89) || (static_cast<int>(rotation) - 226 < 89)) {
                    scaleX1 = jumpFactor * 1.35f;
                    scaleY1 = 0.8f;
                    scaleX2 = 0.9f;
                    scaleY2 = 1.1f;
                }

                auto scaleTo1 = CCScaleTo::create(0.1f / m_gravityMod, scaleX1, scaleY1);
                auto ease1 = CCEaseInOut::create(scaleTo1, 2.0f);

                auto scaleTo2 = CCScaleTo::create(0.25f / m_gravityMod, scaleX2, scaleY2);
                auto ease2 = CCEaseInOut::create(scaleTo2, 2.0f);

                auto scaleTo3 = CCScaleTo::create(0.15f / m_gravityMod, 1.0f, 1.0f);
                auto ease3 = CCEaseInOut::create(scaleTo3, 2.0f);

                auto sequence = CCSequence::create(ease1, ease2, ease3, nullptr);
                sequence->setTag(13);

                if (iterations == 0)
                    m_iconSprite->runAction(sequence);
                else
                    m_iconGlow->runAction(sequence);

                iterations++;
            } while (iterations < 2);
        }
    }


    // $override bool canStickToGround()
    // {
    //     if ( this->m_isShip ) return !this->m_stateRingJump;
    //     constexpr bool canStickToGround = true;
    //     if ( this->m_isDart ) return !this->m_stateRingJump;
    //     return canStickToGround;
    // }
};
#endif