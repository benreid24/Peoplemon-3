#ifndef EDITOR_PAGES_SUBPAGES_GAMETESTING_HPP
#define EDITOR_PAGES_SUBPAGES_GAMETESTING_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Systems/Systems.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Subpage in the map editor for creating and testing game saves
 *
 * @ingroup Pages
 *
 */
class GameTesting {
public:
    /**
     * @brief Construct a new Game Testing page
     *
     */
    GameTesting();

    /**
     * @brief Returns the content to pack
     *
     */
    const bl::gui::Box::Ptr& getContent() const;

private:
    bl::gui::Box::Ptr content;
};

} // namespace page
} // namespace editor

#endif
