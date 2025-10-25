#pragma once

#include <Geode/utils/string.hpp>

using namespace geode::prelude;

template <geode::utils::string::ConstexprString S, typename T>
T const& getSettingFast() {
    static T value = (
        geode::listenForSettingChanges<T>(S.data(), [](T val) {
            value = std::move(val);
        }),
        geode::getMod()->getSettingValue<T>(S.data())
    );
    return value;
}

CCTexture2D* createTextureFromColors(std::vector<ccColor3B> const& colors);
CCTexture2D* createTextureFromColors(std::vector<std::string> const& colors);

std::vector<ccColor3B> hexToColorList(std::vector<std::string> const& colors);
std::vector<std::string> colorToHexList(std::vector<ccColor3B> const& colors);