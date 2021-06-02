#ifndef EDITOR_PAGES_TODO_HPP
#define EDITOR_PAGES_TODO_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Helper page to track tasks marked as "TODO". Things such as entities, position on a map,
 *        conversations, and scripts may be saved as "TODO" and will show up on this page. Clicking
 *        that entry will change the editor state to that object. This allows developers to easily
 *        revisit things without having to remember or record what they were or where
 *
 * @ingroup Pages
 *
 */
class Todo : public Page {
public:
    /**
     * @brief Construct a new Todo page
     *
     * @param systems The primary systems object
     */
    Todo(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Todo() = default;

    /**
     * @brief Does nothing
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    // TODO
};

} // namespace page
} // namespace editor

#endif
