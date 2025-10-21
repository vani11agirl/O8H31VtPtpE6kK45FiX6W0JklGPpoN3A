#include "ColorStreak.hpp"

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

void ColorStreak::setOpacity(GLubyte opacity) {
    CCNodeRGBA::setOpacity(opacity);
}

GLubyte ColorStreak::getOpacity() {
    return CCNodeRGBA::getOpacity();
}

bool ColorStreak::initWithColors(float fade, float minSeg, float stroke, const std::vector<ccColor3B>& colors) {
    if (colors.empty()) return false;
    m_colors = colors;

    int w = static_cast<int>(colors.size());
    std::vector<GLubyte> data(w * 4);

    for (int i = 0; i < w; ++i) {
        const auto& c = colors[w - 1 - i];
        data[i * 4 + 0] = c.r;
        data[i * 4 + 1] = c.g;
        data[i * 4 + 2] = c.b;
        data[i * 4 + 3] = 255;
    }

    m_texture = new CCTexture2D();
    m_texture->initWithData(data.data(), kCCTexture2DPixelFormat_RGBA8888, w, 1, CCSize{static_cast<float>(w), 1});

    ccTexParams params = {GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
    m_texture->setTexParameters(&params);
    m_texture->autorelease();

    if (!initWithFade(fade, minSeg, stroke, ccWHITE, m_texture)) return false;
    setPosition({0, 0});

    return true;
}

void ColorStreak::draw()
{
    if (m_uNuPoints <= 1)
        return;

    CC_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs(kCCVertexAttribFlag_PosColorTex);
    ccGLBlendFunc(m_tBlendFunc.src, m_tBlendFunc.dst);

    ccGLBindTexture2D(m_pTexture->getName());

    // make opacity actually apply cause the cocos devs didnt
    // "Set opacity no supported" https://github.com/matcool/cocos2d-x-gd/blob/main/cocos2dx/misc_nodes/CCMotionStreak.cpp#L184
    for (unsigned int i = 0; i < m_uNuPoints * 2; ++i) {
        m_pColorPointer[i * 4 + 3] = static_cast<GLubyte>(m_pColorPointer[i * 4 + 3] * _displayedOpacity / 255);
    }

    glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, m_pVertices);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, m_pTexCoords);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, m_pColorPointer);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(m_uNuPoints) * 2);

    *getNumberOfDraws() += 1;
}

ColorStreak* ColorStreak::create(float fade, float minSeg, float stroke, const std::vector<ccColor3B>& colors) {
    auto ret = new ColorStreak();
    if (!ret->initWithColors(fade, minSeg, stroke, colors)) {
        delete ret;
        return nullptr;
    }
    ret->autorelease();
    return ret;
}