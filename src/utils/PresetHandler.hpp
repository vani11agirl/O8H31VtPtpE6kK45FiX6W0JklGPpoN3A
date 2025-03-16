#pragma once

#include <Geode.hpp>

using namespace geode::prelude;

struct ColorPreset {
  std::string name;
  ccColor3B accent;
  std::vector<ccColor3B> colors;
};

class PresetHandler {
  protected:
    static PresetHandler* m_instance;
    
    std::map<std::string, ColorPreset> m_presets = {};

    void init();
  public:
    static PresetHandler* get();

    std::map<std::string, ColorPreset> getPresets();
    void loadPresets();
};