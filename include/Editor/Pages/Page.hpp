#ifndef EDITOR_PAGES_PAGE_HPP
#define EDITOR_PAGES_PAGE_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Systems/Systems.hpp>

/**
 * @defgroup Editor
 * @brief All classes and functionality used for implementing the game editor
 *
 */

/**
 * @addtogroup Pages
 * @ingroup Editor
 * @brief Classes that implement top level editor pages
 *
 */

/// All classes and functionality used for implementing the game editor
namespace editor
{
/// Classes that implement top level editor pages
namespace page
{
/**
 * @brief Base class for all editor pages
 *
 * @ingroup Pages
 *
 */
class Page {
public:
    /**
     * @brief Construct a new Page
     *
     * @param systems The primary systems object
     */
    Page(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Page() = default;

    /**
     * @brief Any custom page update logic outside of event callbacks goes here
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Returns the content to put in the editor
     *
     */
    bl::gui::Box::Ptr getContent();

    /**
     * @brief Registers the parent GUI
     *
     * @param parent The parent GUI object
     */
    void registerGui(bl::gui::GUI::Ptr parent);

protected:
    core::system::Systems& systems;
    bl::gui::Box::Ptr content;
    bl::gui::GUI::Ptr parent;
};

} // namespace page
} // namespace editor

#endif
