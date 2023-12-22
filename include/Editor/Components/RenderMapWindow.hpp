#ifndef EDITOR_COMPONENTS_RENDERMAPWINDOW_HPP
#define EDITOR_COMPONENTS_RENDERMAPWINDOW_HPP

#include <BLIB/Interfaces.hpp>
#include <Editor/Components/LightSlider.hpp>
#include <functional>

namespace editor
{
namespace component
{
/**
 * @brief Options window to help trigger map renderings
 *
 * @ingroup Components
 *
 */
class RenderMapWindow {
public:
    /// @brief Callback for when the render should trigger
    using StartRender = std::function<void()>;

    /**
     * @brief Construct a new Render Map Window
     *
     * @param onStart Callback to call when a rendering should take place
     */
    RenderMapWindow(const StartRender& onStart);

    /**
     * @brief Opens the window
     *
     * @param gui The parent GUI object
     */
    void open(bl::gui::GUI* gui);

    /**
     * @brief Returns the filename to output to
     *
     */
    const std::string& outputPath() const;

    /**
     * @brief Returns whether or not to render the characters in the map
     *
     */
    bool renderCharacters() const;

    /**
     * @brief Returns the light level to render the map at
     *
     */
    std::uint8_t lightLevel() const;

private:
    const StartRender trigger;
    bl::gui::Window::Ptr window;
    std::string output;
    bl::gui::CheckButton::Ptr charBut;
    LightSlider::Ptr lightSelect;

    void close();
    void start();
};

} // namespace component
} // namespace editor

#endif
