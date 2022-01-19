#ifndef EDITOR_COMPONENTS_LIGHTSLIDER_HPP
#define EDITOR_COMPONENTS_LIGHTSLIDER_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Editor/Components/LightPreview.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Light level slider
 *
 * @ingroup Components
 *
 */
class LightSlider : public bl::gui::Box {
public:
    using Ptr      = std::shared_ptr<LightSlider>;
    using ChangeCb = std::function<void()>;

    /**
     * @brief Create a new light level slider
     *
     * @param prompt Text to display next to the slider
     * @param onChange Function to call when changed
     * @return Ptr The new slider
     */
    static Ptr create(const std::string& prompt, const ChangeCb& onChange);

    /**
     * @brief Returns the currently selected light level
     *
     */
    std::uint8_t getLightLevel() const;

    /**
     * @brief Sets the selected light level
     *
     */
    void setLightLevel(std::uint8_t level);

    /**
     * @brief Sets the text prompt that is displayed
     *
     */
    void setPrompt(const std::string& prompt);

private:
    bl::gui::Label::Ptr prompt;
    bl::gui::Slider::Ptr slider;
    LightPreview::Ptr preview;

    LightSlider(const std::string& prompt, const ChangeCb& onChange);
};
} // namespace component
} // namespace editor

#endif
