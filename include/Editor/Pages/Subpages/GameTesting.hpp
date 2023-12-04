#ifndef EDITOR_PAGES_SUBPAGES_GAMETESTING_HPP
#define EDITOR_PAGES_SUBPAGES_GAMETESTING_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/GameSave.hpp>
#include <Core/Systems/Systems.hpp>
#include <Editor/Components/GameTestingWindow.hpp>

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
     * @brief Sets the primary GUI object
     *
     */
    void registerGUI(bl::gui::GUI* gui);

    /**
     * @brief Notifies of the spawn being set
     *
     * @param map The name of the map to spawn into
     * @param level The level to spawn on
     * @param pos The position to spawn at
     */
    void notifyClick(const std::string& map, unsigned int level, const sf::Vector2i& pos);

    /**
     * @brief Returns the content to pack
     *
     */
    const bl::gui::Box::Ptr& getContent() const;

private:
    std::vector<core::file::GameSave> saves;
    core::file::GameSave newSave;

    bl::gui::GUI* gui;
    bl::gui::Box::Ptr content;
    bl::gui::ComboBox::Ptr saveSelector;
    bl::gui::Label::Ptr posLabel;
    component::GameTestingWindow window;

    void sync();
    void updatePosLabel(unsigned int level, const sf::Vector2i& pos);
    void launchGame();
};

} // namespace page
} // namespace editor

#endif
