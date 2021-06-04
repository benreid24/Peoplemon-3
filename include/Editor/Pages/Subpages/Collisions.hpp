#ifndef EDITOR_PAGES_SUBPAGES_COLLISIONS_HPP
#define EDITOR_PAGES_SUBPAGES_COLLISIONS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Collision.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Editor subpage for selecting collisions to apply
 *
 * @ingroup Pages
 *
 */
class Collisions {
public:
    /**
     * @brief Loads collision images and builds the gui content
     *
     */
    Collisions();

    /**
     * @brief Returns GUI content to pack
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Returns the currently selected collision
     *
     */
    core::map::Collision selected() const;

private:
    bl::gui::Box::Ptr content;
    core::map::Collision active;
};

} // namespace page
} // namespace editor

#endif
