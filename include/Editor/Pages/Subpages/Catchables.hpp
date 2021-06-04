#ifndef EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP
#define EDITOR_PAGES_SUBPAGES_CATCHABLES_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Catch.hpp>

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
    core::map::Catch selected() const;

private:
    bl::gui::Box::Ptr content;
    core::map::Catch active;
};

} // namespace page
} // namespace editor

#endif
