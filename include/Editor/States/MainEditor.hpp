#ifndef EDITOR_STATES_MAINEDITOR_HPP
#define EDITOR_STATES_MAINEDITOR_HPP

#include <BLIB/Engine.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Pages/Page.hpp>

#include <Editor/Pages/Credits.hpp>
#include <Editor/Pages/Items.hpp>
#include <Editor/Pages/Map.hpp>
#include <Editor/Pages/Moves.hpp>
#include <Editor/Pages/Peoplemon.hpp>
#include <Editor/Pages/Playlists.hpp>
#include <Editor/Pages/Scripts.hpp>
#include <Editor/Pages/Testing.hpp>
#include <Editor/Pages/VariousEditors.hpp>

/**
 * @addtogroup States
 * @ingroup Editor
 * @brief Collection of engine states for the editor
 *
 */

namespace editor
{
/// Collection of engine states for the editor
namespace state
{
/**
 * @brief The primary state of the editor. Owns the GUI and all tabs
 *
 * @ingroup States
 *
 */
class MainEditor
: public bl::engine::State
, private bl::event::Listener<sf::Event> {
public:
    /**
     * @brief Construct a new Main Editor state
     *
     * @param systems The primary systems object
     */
    static Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Main Editor state
     *
     */
    virtual ~MainEditor() = default;

    /**
     * @brief Returns "MainEditor"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Subscribes the gui to window events
     *
     */
    virtual void activate(bl::engine::Engine&) override;

    /**
     * @brief Unsubscribes the gui from window events
     *
     */
    virtual void deactivate(bl::engine::Engine&) override;

    /**
     * @brief Updates the gui and the current page
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine&, float dt) override;

    /**
     * @brief Renders the gui
     *
     * @param lag Time not accounted for in update
     */
    virtual void render(bl::engine::Engine&, float lag) override;

private:
    core::system::Systems& systems;

    page::Map mapPage;
    page::VariousEditors variousEditorsPage;
    page::Scripts scriptPage;
    page::Testing testingPage;
    page::Peoplemon peoplemonPage;
    page::Moves movesPage;
    page::Items itemsPage;
    page::Playlists playlistsPage;
    page::Credits creditsPage;
    page::Page* currentPage;

    bl::gui::GUI::Ptr gui;
    bl::gui::Renderer::Ptr renderer;
    bl::gui::Notebook::Ptr notebook;

    MainEditor(core::system::Systems& systems);

    virtual void observe(const sf::Event& event) override;
};

} // namespace state
} // namespace editor

#endif
