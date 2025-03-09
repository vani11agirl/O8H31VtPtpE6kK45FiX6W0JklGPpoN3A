//
// Created by transgendercat on 3/5/2025.
//

#include "../utils/color.hpp"
namespace custom::utils::color {
    ccColor3B ColorUtils::hsvToRgb(const ccHSVValue& hsv) {
        const float hue = hsv.h;
        const float saturation = hsv.s;
        const float value = hsv.v;

        const int hi = static_cast<int>(std::floor(hue / 60.0f)) % 6;
        const float f = hue / 60.0f - std::floor(hue / 60.0f);

        const float p = value * (1 - saturation);
        const float q = value * (1 - f * saturation);
        const float t = value * (1 - (1 - f) * saturation);

        float r, g, b;

        switch (hi) {
        case 0: r = value; g = t; b = p; break;
        case 1: r = q; g = value; b = p; break;
        case 2: r = p; g = value; b = t; break;
        case 3: r = p; g = q; b = value; break;
        case 4: r = t; g = p; b = value; break;
        case 5: r = value; g = p; b = q; break;
        default: r = g = b = 0; break;
        }

        return ccc3(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(b * 255));
    }


    ccColor3B ColorUtils::hexToColor3B(const std::string &hexCode)
    {
        std::string clean_hex = hexCode[0] == '#' ? hexCode.substr(1) : hexCode;

        int color = geode::utils::numFromString<int>(clean_hex, 16).unwrapOr(0);

        GLubyte red = (color >> 16) & 0xFF;
        GLubyte green = (color >> 8) & 0xFF;
        GLubyte blue = color & 0xFF;

        return ccColor3B{
            .r = red,
            .g = green,
            .b = blue
    };
    }

    std::string ColorUtils::color3BToHex(const ccColor3B& color) {
        return fmt::format("#{:02X}{:02X}{:02X}", color.r, color.g, color.b);
    }
}