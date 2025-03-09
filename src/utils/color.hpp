//
// Created by transgendercat on 3/5/2025.
//

#ifndef COLOR_HPP
#define COLOR_HPP

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace custom::utils::color {
    class ColorUtils {
    public:
        static ccColor3B hsvToRgb(const ccHSVValue& hsv);

        static ccColor3B hexToColor3B(const std::string& hexCode);

        static std::string color3BToHex(const ccColor3B& color);
    };
} // utils::color
#endif