#ifndef EDITOR_COMPONENTS_WEATHERSELECT_HPP
#define EDITOR_COMPONENTS_WEATHERSELECT_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Weather.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Helper component to select weather
 *
 * @ingroup Components
 *
 */
class WeatherSelect : public bl::gui::ComboBox {
public:
    /// Pointer to the weather selector
    using Ptr = std::shared_ptr<WeatherSelect>;

    /**
     * @brief Creates a new weather selector
     *
     */
    static Ptr create();

    /**
     * @brief Returns the currently selected weather
     *
     */
    core::map::Weather::Type selectedWeather() const;

    /**
     * @brief Sets the currently selected weather
     *
     */
    void setSelectedWeather(core::map::Weather::Type type);

private:
    WeatherSelect();
};

} // namespace component
} // namespace editor

#endif
