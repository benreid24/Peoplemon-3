#ifndef EDITOR_PAGES_CONVERSATIONS_HPP
#define EDITOR_PAGES_CONVERSATIONS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Editor/Components/ConversationWindow.hpp>
#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for viewing and editing all conversations
 *
 * @ingroup Pages
 *
 */
class Conversations : public Page {
public:
    /**
     * @brief Construct a new Conversations tab
     *
     * @param systems The primary systems object
     */
    Conversations(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Conversations() = default;

    /**
     * @brief Does nothing
     *
     */
    virtual void update(float dt) override;

private:
    component::ConversationWindow editor;

    void openEditor();
};

} // namespace page
} // namespace editor

#endif
