#ifndef EDITOR_STATES_MAINEDITOR_HPP
#define EDITOR_STATES_MAINEDITOR_HPP

#include <BLIB/Engine.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Pages/Page.hpp>

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
class MainEditor : public bl::engine::State {
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
    virtual ~MainEditor();

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
    bl::gui::GUI::Ptr gui;
    std::vector<page::Page*> pages;

    MainEditor(core::system::Systems& systems);
};

} // namespace state
} // namespace editor

#endif
