#pragma once

using namespace geode::prelude;

class ColorStreak : public CCMotionStreak {
protected:
    std::vector<ccColor3B> m_colors;
    CCTexture2D* m_texture;
    bool initWithColors(float fade, float minSeg, float stroke, const std::vector<ccColor3B>& colors);

    void draw() override;
public:
    void setOpacity(GLubyte opacity) override;
    GLubyte getOpacity() override;

    static ColorStreak* create(float fade, float minSeg, float stroke, const std::vector<ccColor3B>& colors);
    static ColorStreak* create(float fade, float minSeg, float stroke, const std::vector<std::string>& colors);
};