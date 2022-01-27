#include <Editor/Components/WeatherSelect.hpp>

namespace editor
{
namespace component
{
WeatherSelect::Ptr WeatherSelect::create() { return Ptr(new WeatherSelect()); }

WeatherSelect::WeatherSelect()
: ComboBox() {
    addOption("None");
    addOption("AllRandom");
    addOption("LightRain");
    addOption("LightRainThunder");
    addOption("HardRain");
    addOption("HardRainThunder");
    addOption("LightSnow");
    addOption("LightSnowThunder");
    addOption("HardSnow");
    addOption("HardSnowThunder");
    addOption("ThinFog");
    addOption("ThickFog");
    addOption("Sunny");
    addOption("SandStorm");
    addOption("WaterRandom");
    addOption("SnowRandom");
    addOption("DesertRandom");
    setSelectedOption(0);
    setMaxHeight(300);
}

core::map::Weather::Type WeatherSelect::selectedWeather() const {
    return getSelectedOption() >= 0 ? static_cast<core::map::Weather::Type>(getSelectedOption()) :
                                      core::map::Weather::None;
}

void WeatherSelect::setSelectedWeather(core::map::Weather::Type t) {
    setSelectedOption(static_cast<int>(t));
}

} // namespace component
} // namespace editor
