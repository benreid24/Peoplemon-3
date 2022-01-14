#ifndef EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP
#define EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP

#include <BLIB/Interfaces/GUI.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Editor subpage for selecting catch tiles to place
 *
 * @ingroup Pages
 *
 */
class Catchables {
public:
    /**
     * @brief Construct a new Catchables GUI
     *
     */
    Catchables();

    /**
     * @brief Returns the GUI element to pack
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Returns the currently selected catch type
     *
     */
    std::uint8_t selected() const;

    /**
     * @brief Returns the color to use for the given catch region
     *
     */
    static sf::Color getColor(std::uint8_t index);

private:
    bl::gui::Box::Ptr content;
    std::uint8_t active;
};

} // namespace page
} // namespace editor

#endif
