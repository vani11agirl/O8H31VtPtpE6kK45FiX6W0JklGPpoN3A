#include <Geode/cocos/textures/CCTextureCache.h>
#include <Geode/cocos/shaders/ccGLStateCache.h>
#include <Geode/cocos/shaders/CCGLProgram.h>
#include <Geode/cocos/shaders/CCShaderCache.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/support/CCVertex.h>
#include <Geode/cocos/support/CCPointExtension.h>
#include "CCMultiColorMotionStreak.h"

using namespace geode::prelude;

CCMultiColorMotionStreak::CCMultiColorMotionStreak()
: m_verticesPerPoint(5), // default to 5 stripes; set via setStripeColors()
  m_blendingEnabled(false)
{
}

CCMultiColorMotionStreak::~CCMultiColorMotionStreak()
= default;

CCMultiColorMotionStreak* CCMultiColorMotionStreak::create(
    float fadeTime,      // Duration for fade (in seconds)
    float minSeg,        // Minimum segment length
    float trailWidth,    // Width/size of the trail
    float opacity,       // Opacity of the trail
    const std::vector<ccColor3B>& colors,
    CCTexture2D* texture,
    bool disableBlending
) {
    CCTexture2D* finalTexture = texture;
    if (disableBlending) {
        std::vector<GLubyte> texData;
        for (const auto& color : colors) {
            texData.push_back(color.r);
            texData.push_back(color.g);
            texData.push_back(color.b);
            texData.push_back(255); // Force alpha to 255
        }
        CCTexture2D* stripeTexture = new CCTexture2D();
        stripeTexture->initWithData(
            texData.data(),
            kCCTexture2DPixelFormat_RGBA8888,
            static_cast<unsigned int>(colors.size()),
            1,
            CCSize(static_cast<float>(colors.size()), 1)
        );
        ccTexParams texParams = {GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
        stripeTexture->setTexParameters(&texParams);
        finalTexture = stripeTexture;
        stripeTexture->autorelease(); // Use autorelease instead of manual release
    }

    auto *pRet = new CCMultiColorMotionStreak();
    if (pRet && pRet->initWithColors(fadeTime, minSeg, trailWidth, colors, finalTexture, disableBlending)) {
        pRet->autorelease();
        pRet->setBlendingEnabled(!disableBlending);
        pRet->setOpacity(opacity);
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}


bool CCMultiColorMotionStreak::initWithColors(
    float fade, float minSeg, float stroke,
    const std::vector<ccColor3B>& colors,
    CCTexture2D* texture, bool disableBlending
) {
    // Use the provided texture (either user's or generated 1D texture)
    if (!CCMotionStreak::initWithFade(fade, minSeg, stroke, {255, 255, 255}, texture))
        return false;

    setStripeColors(colors);

    // Update vertex buffer
    const unsigned int totalVertices = m_uMaxPoints * m_verticesPerPoint;
    m_pVertices = static_cast<ccVertex2F*>(realloc(m_pVertices, sizeof(ccVertex2F) * totalVertices));
    m_pTexCoords = static_cast<ccTex2F*>(realloc(m_pTexCoords, sizeof(ccTex2F) * totalVertices));

    // Only allocate colors if blending is enabled
    if (!disableBlending) {
        m_pColorPointer = static_cast<GLubyte*>(realloc(m_pColorPointer, sizeof(GLubyte) * totalVertices * 4));
    }

    return true;
}
void CCMultiColorMotionStreak::setStripeColors(const std::vector<ccColor3B>& colors)
{
    m_stripeColors = colors;
    m_verticesPerPoint = static_cast<unsigned int>(colors.size());
}

void CCMultiColorMotionStreak::setBlendingEnabled(bool enabled) {
    m_blendingEnabled = enabled;
}

// Updated update() function: now iterates from i = 0 to m_uNuPoints
void CCMultiColorMotionStreak::update(float delta) {
    CCMotionStreak::update(delta);
    if (m_uNuPoints < 2)
        return;

    // Loop through all points in the trail
    for (unsigned int i = 0; i < m_uNuPoints; i++) {
        CCPoint tangent;
        if (i == 0 && m_uNuPoints > 1) {
            tangent = ccpNormalize(ccpSub(m_pPointVertexes[1], m_pPointVertexes[0]));
        } else if (i == m_uNuPoints - 1) {
            tangent = ccpNormalize(ccpSub(m_pPointVertexes[i], m_pPointVertexes[i - 1]));
        } else {
            // Average tangent for smoother bends
            CCPoint forward = ccpSub(m_pPointVertexes[i + 1], m_pPointVertexes[i]);
            CCPoint backward = ccpSub(m_pPointVertexes[i], m_pPointVertexes[i - 1]);
            tangent = ccpNormalize(ccpAdd(forward, backward));
        }
        CCPoint normal = ccp(-tangent.y, tangent.x);

        for (unsigned int j = 0; j < m_verticesPerPoint; j++) {
            float factor = (static_cast<float>(j) / (m_verticesPerPoint - 1)) - 0.5f;
            CCPoint offset = ccpMult(normal, factor * m_fStroke);
            CCPoint vertexPos = ccpAdd(m_pPointVertexes[i], offset);

            unsigned int vertexIndex = i * m_verticesPerPoint + j;
            m_pVertices[vertexIndex] = { vertexPos.x, vertexPos.y };
            m_pTexCoords[vertexIndex] = tex2(static_cast<float>(j) / (m_verticesPerPoint - 1),
                                             static_cast<float>(i) / m_uNuPoints);

            // Compute fade factor: older points become more transparent
            float fadeFactor = (m_uNuPoints > 1) ? (static_cast<float>(i) / (m_uNuPoints - 1)) : 1.0f;
            GLubyte vertexAlpha = static_cast<GLubyte>(255 * fadeFactor);

            if (m_blendingEnabled) {
                ccColor3B color = m_stripeColors[j % m_stripeColors.size()];
                m_pColorPointer[vertexIndex * 4 + 0] = color.r;
                m_pColorPointer[vertexIndex * 4 + 1] = color.g;
                m_pColorPointer[vertexIndex * 4 + 2] = color.b;
                m_pColorPointer[vertexIndex * 4 + 3] = vertexAlpha;
            } else {
                float u = (static_cast<float>(j) + 0.5f) / m_verticesPerPoint;
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
            int leftIndex = i * m_verticesPerPoint + stripe;
            int rightIndex = leftIndex + 1;

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

    CC_INCREMENT_GL_DRAWS(1);
}
