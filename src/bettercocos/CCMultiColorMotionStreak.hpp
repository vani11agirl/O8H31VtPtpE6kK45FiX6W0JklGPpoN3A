#ifndef __CC_MULTI_COLOR_MOTION_STREAK_H__
#define __CC_MULTI_COLOR_MOTION_STREAK_H__

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/textures/CCTexture2D.h>
#include <Geode/cocos/support/CCPointExtension.h>
#include <Geode/cocos/misc_nodes/CCMotionStreak.h>
#include <vector>

using namespace geode::prelude;

class CCMultiColorMotionStreak : public CCMotionStreak {
public:
    CCMultiColorMotionStreak();
    ~CCMultiColorMotionStreak() override;

    static CCMultiColorMotionStreak* create(float fade, float minSeg, float trailWidth, float opacity,
                                            const std::vector<ccColor3B>& colors,
                                            CCTexture2D* texture, bool disableBlending);

    bool initWithColors(float fade, float minSeg, float stroke,
                        const std::vector<ccColor3B>& colors,
                        CCTexture2D* texture, bool disableBlending);

    void setStripeColors(const std::vector<ccColor3B>& colors);
    void setBlendingEnabled(bool enabled);
    virtual void update(float delta) override;
    void draw() override;

protected:
    std::vector<ccColor3B> m_stripeColors;
    unsigned int m_verticesPerPoint;
    bool m_blendingEnabled;
};

#endif // __CC_MULTI_COLOR_MOTION_STREAK_H__
