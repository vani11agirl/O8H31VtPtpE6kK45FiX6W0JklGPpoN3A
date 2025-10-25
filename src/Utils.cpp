#include "Utils.hpp"

CCTexture2D* createTextureFromColors(std::vector<ccColor3B> const& colors) {
    int w = static_cast<int>(colors.size());
    std::vector<GLubyte> data(w * 4);

    for (int i = 0; i < w; ++i) {
        const auto& c = colors[i];
        data[i * 4 + 0] = c.r;
        data[i * 4 + 1] = c.g;
        data[i * 4 + 2] = c.b;
        data[i * 4 + 3] = 255;
    }

    auto texture = new CCTexture2D();
    texture->initWithData(data.data(), kCCTexture2DPixelFormat_RGBA8888, w, 1, CCSize{static_cast<float>(w), 1});

    ccTexParams params = {GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};
    texture->setTexParameters(&params);
    texture->autorelease();
    return texture;
}

CCTexture2D* createTextureFromColors(std::vector<std::string> const& colors) {
    return createTextureFromColors(hexToColorList(colors));
}

std::vector<ccColor3B> hexToColorList(std::vector<std::string> const& colors) {
    std::vector<ccColor3B> cols3b = {};
    for (auto hex : colors) {
        auto res = cc3bFromHexString(hex);
        if (res.isOk()) {
            cols3b.push_back(res.unwrap());
        } else {
            log::error("Failed to parse color from hex {}: {}", hex, res.unwrapErr());
        }
    }
    return cols3b;
}

std::vector<std::string> colorToHexList(std::vector<ccColor3B> const& colors) {
    std::vector<std::string> colsHex = {};
    colsHex.reserve(colors.size());
    for (auto col3b : colors) {
        colsHex.push_back(cc3bToHexString(col3b));
    }
    return colsHex;
}