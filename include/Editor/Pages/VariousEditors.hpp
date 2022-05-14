#ifndef EDITOR_PAGES_VARIOUSEDITORS_HPP
#define EDITOR_PAGES_VARIOUSEDITORS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Editor/Components/ConversationWindow.hpp>
#include <Editor/Components/NPCEditorWindow.hpp>
#include <Editor/Components/PlaylistEditorWindow.hpp>
#include <Editor/Components/TrainerEditorWindow.hpp>
#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for opening various editor windows outside the map editor
 *
 * @ingroup Pages
 *
 */
class VariousEditors : public Page {
public:
    /**
     * @brief Construct a new VariousEditors tab
     *
     * @param systems The primary systems object
     */
    VariousEditors(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~VariousEditors() = default;

    /**
     * @brief Does nothing
     *
     */
    virtual void update(float dt) override;

private:
    component::ConversationWindow conversationEditor;
    component::NpcEditorWindow npcEditor;
    component::TrainerEditorWindow trainerEditor;
    component::PlaylistEditorWindow playlistEditor;

    void openConvEditor();
    void openNpcEditor();
    void openTrainerEditor();
    void openPlaylistEditor();
};

} // namespace page
} // namespace editor

#endif
