#ifndef EDITOR_COMPONENTS_HIGHLIGHTRADIOBUTTON_HPP
#define EDITOR_COMPONENTS_HIGHLIGHTRADIOBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/RadioButton.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Helper GUI component that functions as a RadioButton but also renders a conspicuous
 *        highlight when it is active
 *
 * @ingroup UIComponents
 */
class HighlightRadioButton : public bl::gui::RadioButton {
public:
    /// Pointer to a HighlightRadioButton
    using Ptr = std::shared_ptr<HighlightRadioButton>;

    /**
     * @brief Creates a new HighlightRadioButton
     *
     * @param child Child element to use for button display
     * @param group Radio group to be a part of
     * @return Ptr The newly created HighlightRadioButton
     */
    static Ptr create(bl::gui::Element::Ptr child, bl::gui::RadioButton::Group* group = nullptr);

    /**
     * @brief Destroy the Highlight Radio Button
     */
    virtual ~HighlightRadioButton() = default;

private:
    HighlightRadioButton(bl::gui::Element::Ptr child, bl::gui::RadioButton::Group* group);

    virtual void onAcquisition() override;
    virtual bl::gui::rdr::Component* doPrepareRender(bl::gui::rdr::Renderer& renderer) override;
};

} // namespace component
} // namespace editor

#endif
