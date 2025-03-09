//
// Created by transgendercat on 3/5/2025.
//

#ifndef UTILS_H
#define UTILS_H

template<unsigned N>
struct TemplateStr {
    char buf[N + 1]{};

    constexpr TemplateStr(char const* s) {
        for (unsigned i = 0; i != N; ++i) buf[i] = s[i];
    }

    constexpr operator char const*() const { return buf; }
    constexpr operator std::string_view() const { return std::string_view(buf); }
};

template<unsigned N> TemplateStr(char const (&)[N]) -> TemplateStr<N - 1>;


template <TemplateStr settingName, typename T>
/**
 * @brief getSettingValue but FAST :money_mouth:
 *
 * @tparam T The type of the setting value
 * @return The cached setting value
 */
T fastGetSetting() {
    static T cachedSetting = (geode::listenForSettingChanges<T>(settingName, [](T v) {
        cachedSetting = v;
    }), geode::Mod::get()->getSettingValue<T>(settingName));
    return cachedSetting;
}

#endif //UTILS_H
