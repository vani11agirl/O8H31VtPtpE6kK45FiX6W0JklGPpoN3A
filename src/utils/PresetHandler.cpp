#include "PresetHandler.hpp"

template <>
struct matjson::Serialize<ColorPreset>
{
    static Result<ColorPreset> fromJson(Value const &value)
    {
        ColorPreset result = {
          .accent = value["accent"].as<ccColor3B>().unwrapOrDefault(),
          .colors = value["colors"].as<std::vector<ccColor3B>>().unwrapOrDefault(),
        };
        return Ok(result);
    }

    static Value toJson(ColorPreset const &value)
    {
        auto ret = matjson::makeObject({
          {"name", value.name},
          {"accent", value.accent},
          {"colors", value.colors},
        });
        return ret;
    }
};

PresetHandler* PresetHandler::m_instance = nullptr;

void PresetHandler::loadPresets() {
    auto mod = Mod::get();

    std::filesystem::path presetsJson = mod->getResourcesDir() / "presets.json";

    log::debug("Reading presets from {}", presetsJson.string());

    // we read the file meow
    std::ifstream file(presetsJson);

    auto res = matjson::parse(file);
    if (!res) {
        log::error("Failed to parse trail presets from presets.json!");
        return;
    }
    matjson::Value root = res.unwrapOr("Oops...");
    if (root == "Oops..." || !root.isObject()) {
        log::error("Failed to parse preset root!");
        return;
    }

    for (auto& [name, val] : root)
    {
        auto preset = val.as<ColorPreset>().unwrapOr(ColorPreset{});
        preset.name = name;
        if (preset.colors.empty()) {
            log::error("Failed to parse a preset: {}", name);
            continue;
        }

        m_presets[name] = (preset);
    }
}

std::map<std::string, ColorPreset> PresetHandler::getPresets() {
    return m_presets;
}

PresetHandler* PresetHandler::get() {
    if (!m_instance) {
        m_instance = new PresetHandler();
        m_instance->loadPresets();
    }
    return m_instance;
}

$on_mod(Loaded) {
    // Preload presets
    PresetHandler::get();
}