#ifndef EDITOR_PAGES_SUBPAGES_LEVELTRANSITIONS_HPP
#define EDITOR_PAGES_SUBPAGES_LEVELTRANSITIONS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/LevelTransition.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Subpage for selecting level transitions to place on the map
 *
 * @ingroup Pages
 *
 */
class LevelTransitions {
public:
    /**
     * @brief Construct a new Level Transitions GUI page
     *
     */
    LevelTransitions();

    /**
     * @brief Returns a packable GUI element to add this page to the GUI
     *
     */
    bl::gui::Element::Ptr getContent();

    /**
     * @brief Returns the currently selected level transition type
     *
     */
    core::map::LevelTransition getActive() const;

private:
    bl::gui::Box::Ptr content;
    core::map::LevelTransition active;

    void update(core::map::LevelTransition lt);
};

} // namespace page
} // namespace editor

#endif
