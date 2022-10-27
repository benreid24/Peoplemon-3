#ifndef GAME_STATES_FLYMAP_HPP
#define GAME_STATES_FLYMAP_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input.hpp>
#include <Game/States/State.hpp>
#include <SFML/Graphics.hpp>
#include <Core/Maps/Town.hpp>

namespace game
{
namespace state
{
class FlyMap : public State {
public:
    /**
     * @brief Creates the fly map state
     *
     * @param systems The primary systems object
     * @return bl::engine::State::Ptr The new state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the New Game state
     *
     */
    virtual ~FlyMap() = default;

    /**
     * @brief Returns "FlyMap"
     *
     */
    virtual const char* name() const override;

    /**
     * @brief Activates the state
     *
     * @param engine The game engine
     */
    virtual void activate(bl::engine::Engine& engine) override;

    /**
     * @brief Deactivates the state
     *
     * @param engine The game engine
     */
    virtual void deactivate(bl::engine::Engine& engine) override;

    /**
     * @brief Updates the state and menus and whatnot
     *
     * @param engine The game engine
     * @param dt Time elapsed in seconds
     */
    virtual void update(bl::engine::Engine& engine, float dt) override;

    /**
     * @brief Renders the new game features
     *
     * @param engine The game engine
     * @param lag Time elapsed not accounted for in update
     */
    virtual void render(bl::engine::Engine& engine, float lag) override;

private:
    std::vector<sf::Vector2f> townPositions;
    bool hudActive;
    sf::View oldView;

    bl::resource::Resource<sf::Texture>::Ref mapTxtr;
    bl::resource::Resource<sf::Texture>::Ref townTxtr;
    bl::resource::Resource<sf::Texture>::Ref cursorTxtr;
    sf::Sprite map;
    sf::Sprite town;
    sf::Sprite cursor;
    bl::gfx::Flashing cursorFlasher;

    bl::resource::Resource<sf::Texture>::Ref panelTxtr;
    sf::Sprite panel;
    sf::Text townName;
    sf::Text townDesc;

    bl::menu::Menu townMenu;
    core::player::input::MenuDriver inputDriver;

    FlyMap(core::system::Systems& systems);
    void clearHover();
    void hoverTown(unsigned int i);
    void selectTown(unsigned int i);
    void onFlyChoice(const std::string& choice, const core::map::Town& town);
    void messageDone();
    void close();
    void wrap();
};

} // namespace state
} // namespace game

#endif
