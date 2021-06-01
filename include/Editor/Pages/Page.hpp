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
     * @brief Returns the title of the page. Used in the tab
     *
     */
    virtual const char* title() const = 0;

    /**
     * @brief Returns the content of the page itself
     *
     */
    virtual bl::gui::Box::Ptr content() = 0;

    /**
     * @brief Any custom page update logic outside of event callbacks goes here
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) = 0;

protected:
    core::system::Systems& systems;
};

} // namespace page
} // namespace editor

#endif
