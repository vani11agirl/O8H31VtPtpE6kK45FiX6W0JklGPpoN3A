#include <Geode/cocos/textures/CCTextureCache.h>
#include <Geode/cocos/shaders/ccGLStateCache.h>
#include <Geode/cocos/shaders/CCGLProgram.h>
#include <Geode/cocos/shaders/CCShaderCache.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/support/CCVertex.h>
#include <Geode/cocos/support/CCPointExtension.h>
#include "CCMultiColorMotionStreak.hpp"

#include <loader/Log.hpp>

using namespace geode::prelude;

// courtesy of jasmine, thanks <3
unsigned int* getNumberOfDraws() {
    #ifdef GEODE_IS_MACOS
    static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update macOS offsets!");
    return reinterpret_cast<unsigned int*>(geode::base::get() + GEODE_ARM_MAC(0x8b0f60) GEODE_INTEL_MAC(0x98bf30));
    #elif defined(GEODE_IS_IOS)
    static_assert(GEODE_COMP_GD_VERSION == 22074, "Please update iOS offsets!");
    return reinterpret_cast<unsigned int*>(geode::base::get() + 0x8791d0);
    #else
    return &g_uNumberOfDraws;
    #endif
}

CCMultiColorMotionStreak::CCMultiColorMotionStreak()
: m_blendingEnabled(false),
  m_doUnspeakableThings(false), // The "inefficient" mode. Fire variable name
  m_verticesPerPoint(5) // default to 5 stripes; set via setStripeColors()
{
}

CCMultiColorMotionStreak::~CCMultiColorMotionStreak()
= default;

CCMultiColorMotionStreak* CCMultiColorMotionStreak::create(
    const float fadeTime,
    const float minSeg,
    const float trailWidth,
    const float opacity,
    const std::vector<ccColor3B>& colors,
    CCTexture2D* texture,
    const bool disableBlending,
    const bool doUnspeakableThings // false
) {
    const std::vector<ccColor3B> reversedColors(colors.rbegin(), colors.rend());
    CCTexture2D* finalTexture = texture;
    if (disableBlending) {
        std::vector<GLubyte> texData;
        for (const auto& color : reversedColors) {
            texData.push_back(color.r);
            texData.push_back(color.g);
            texData.push_back(color.b);
            texData.push_back(255); // Force alpha to 255
        }
        const auto stripeTexture = new CCTexture2D();
        stripeTexture->initWithData(
            texData.data(),
            kCCTexture2DPixelFormat_RGBA8888,
            static_cast<unsigned int>(reversedColors.size()),
            1,
            CCSize(static_cast<float>(reversedColors.size()), 1)
        );
        ccTexParams texParams = {GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
        stripeTexture->setTexParameters(&texParams);
        finalTexture = stripeTexture;
        stripeTexture->autorelease();
    }

    auto *pRet = new CCMultiColorMotionStreak();
    if (pRet->initWithColors(fadeTime, minSeg, trailWidth, colors, finalTexture, disableBlending)) {
        pRet->setBlendingEnabled(!disableBlending);
        pRet->setOpacity(opacity); // NOLINT(*-narrowing-conversions)
        pRet->m_doUnspeakableThings = doUnspeakableThings;
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

void doUnspeakableThings(
    CCMultiColorMotionStreak* thisPtr,
    const int i
    )
{
    const auto parent = thisPtr->getParent();
    // if (parent) {
    //     const auto parentPos = parent->convertToWorldSpace(thisPtr->m_pPointVertexes[i]);
    //     const auto thisPos = thisPtr->convertToWorldSpace(thisPtr->m_pPointVertexes[i]);
    //     const auto diff = ccpSub(parentPos, thisPos);
    //     thisPtr->setPosition(ccpAdd(thisPtr->getPosition(), diff));
    // }
    const auto pos = thisPtr->getPosition();
    const auto point = thisPtr->m_pPointVertexes[i];
    const float totalWidth = thisPtr->m_fStroke;
    const float dividedWidth = totalWidth / as<float>(i);
    const auto colors = thisPtr->m_stripeColors;
    const auto tex = thisPtr->m_pTexture;
    const float fadeTime = thisPtr->m_fFadeDelta;
    const float minSeg = thisPtr->m_fMinSeg;
    const float opacity = thisPtr->getOpacity();
    const bool disableBlending = thisPtr->m_blendingEnabled;

    thisPtr->removeMeAndCleanup();
    for (int j = 0; j < i; j++) {
        const float offsetFactor = (i + 0.5f) / j - 0.5f;
        if (const auto newStreak = CCMultiColorMotionStreak::create(
            fadeTime,
            minSeg,
            dividedWidth,
            opacity,
            colors,
            tex,
            disableBlending
        ))
        {
            newStreak->setPosition(ccpAdd(pos, ccpMult(point, dividedWidth)));
            newStreak->setOpacity(opacity);
            newStreak->setBlendingEnabled(!disableBlending);
            newStreak->m_doUnspeakableThings = true;
            newStreak->setPosition(ccp(pos.x + offsetFactor * totalWidth, pos.y));
            // newStreak->autorelease();
            if (parent) parent->addChild(newStreak); else (log::error("fuck off"));
        }
    }
}

bool CCMultiColorMotionStreak::initWithColors(
    const float fade, const float minSeg, const float stroke,
    const std::vector<ccColor3B>& colors,
    CCTexture2D* texture, const bool disableBlending
) {
    if (!initWithFade(fade, minSeg, stroke, {255, 255, 255}, texture))
        return false;

    setStripeColors(colors);

    const unsigned int totalVertices = m_uMaxPoints * m_verticesPerPoint;
    m_pVertices = static_cast<ccVertex2F*>(realloc(m_pVertices, sizeof(ccVertex2F) * totalVertices));
    m_pTexCoords = static_cast<ccTex2F*>(realloc(m_pTexCoords, sizeof(ccTex2F) * totalVertices));

    if (!disableBlending) {
        m_pColorPointer = static_cast<GLubyte*>(realloc(m_pColorPointer, sizeof(GLubyte) * totalVertices * 4));
    }

    return true;
}
void CCMultiColorMotionStreak::setStripeColors(const std::vector<ccColor3B>& colors) {
    m_stripeColors = colors;
    // please work meow meow
    m_verticesPerPoint = static_cast<unsigned int>(colors.size() + 1);
}


void CCMultiColorMotionStreak::setBlendingEnabled(const bool enabled) {
    m_blendingEnabled = enabled;
}

void CCMultiColorMotionStreak::update(const float delta) {
    CCMotionStreak::update(delta);
    if (m_uNuPoints < 2)
        return;

    for (unsigned int i = 0; i < m_uNuPoints; i++) {
        CCPoint tangent;
        if (i == 0 && m_uNuPoints > 1) {
            tangent = ccpNormalize(ccpSub(m_pPointVertexes[1], m_pPointVertexes[0]));
        } else if (i == m_uNuPoints - 1) {
            tangent = ccpNormalize(ccpSub(m_pPointVertexes[i], m_pPointVertexes[i - 1]));
        } else {
            CCPoint forward = ccpSub(m_pPointVertexes[i + 1], m_pPointVertexes[i]);
            CCPoint backward = ccpSub(m_pPointVertexes[i], m_pPointVertexes[i - 1]);
            tangent = ccpNormalize(ccpAdd(forward, backward));
        }
        CCPoint normal = ccp(-tangent.y, tangent.x);

        for (unsigned int j = 0; j < m_verticesPerPoint; j++) {
            const float factor = (static_cast<float>(j) / (m_verticesPerPoint - 1)) - 0.5f;
            CCPoint offset = ccpMult(normal, factor * m_fStroke);
            CCPoint vertexPos = ccpAdd(m_pPointVertexes[i], offset);

            unsigned int vertexIndex = i * m_verticesPerPoint + j;
            m_pVertices[vertexIndex] = { vertexPos.x, vertexPos.y };
            m_pTexCoords[vertexIndex] = tex2(static_cast<float>(j) / (m_verticesPerPoint - 1),
                                             static_cast<float>(i) / m_uNuPoints);

            const float fadeFactor = (m_uNuPoints > 1) ? (1.0f - static_cast<float>(i) / (m_uNuPoints - 1)) : 1.0f;
            const auto vertexAlpha = static_cast<GLubyte>(255 * fadeFactor);

            if (m_blendingEnabled) {
                ccColor3B color;
                if (j == 0)
                    color = m_stripeColors.front();
                else if (j == m_verticesPerPoint - 1)
                    color = m_stripeColors.back();
                else
                    color = m_stripeColors[j - 1];
                m_pColorPointer[vertexIndex * 4 + 0] = color.r;
                m_pColorPointer[vertexIndex * 4 + 1] = color.g;
                m_pColorPointer[vertexIndex * 4 + 2] = color.b;
                m_pColorPointer[vertexIndex * 4 + 3] = vertexAlpha;
            } else {
                const float u = static_cast<float>(j) / (m_verticesPerPoint - 1);
                m_pTexCoords[vertexIndex] = tex2(u, static_cast<float>(i) / m_uNuPoints);
                m_pColorPointer[vertexIndex * 4 + 3] = vertexAlpha;
            }
        }
    }
}

void CCMultiColorMotionStreak::draw() {
    if (m_uNuPoints < 2) return;

    CCGLProgram* shader;
    if (m_blendingEnabled) {
        shader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor);
        ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords | kCCVertexAttribFlag_Color);
        ccGLBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        shader = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTexture);
        ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
        ccGLBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Force full opacity
        // Override shader to ensure alpha is 1.0
        shader->use();
        // shader->setUniformFloat("u_alpha", 1.0f); // Add this uniform to your shader if needed
    }
    shader->setUniformsForBuiltins();

    ccGLBindTexture2D(m_pTexture->getName());

    for (unsigned int stripe = 0; stripe < m_verticesPerPoint - 1; stripe++) {
        std::vector<ccVertex2F> stripVertices;
        std::vector<ccTex2F> stripTexCoords;
        std::vector<GLubyte> stripColors;

        for (unsigned int i = 0; i < m_uNuPoints; i++) {
            const int leftIndex = i * m_verticesPerPoint + stripe; // NOLINT(*-narrowing-conversions)
            const int rightIndex = leftIndex + 1;

            stripVertices.push_back(m_pVertices[leftIndex]);
            stripVertices.push_back(m_pVertices[rightIndex]);

            stripTexCoords.push_back(m_pTexCoords[leftIndex]);
            stripTexCoords.push_back(m_pTexCoords[rightIndex]);

            if (m_blendingEnabled) {
                // Include vertex colors for blending mode
                stripColors.insert(stripColors.end(), &m_pColorPointer[leftIndex * 4], &m_pColorPointer[leftIndex * 4 + 4]);
                stripColors.insert(stripColors.end(), &m_pColorPointer[rightIndex * 4], &m_pColorPointer[rightIndex * 4 + 4]);
            }
        }

        glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, stripVertices.data());
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, stripTexCoords.data());
        if (m_blendingEnabled) {
            glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, stripColors.data());
        }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(stripVertices.size()));
    }

    *getNumberOfDraws() += 1;
}
