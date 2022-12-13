#ifndef GAME_STATES_PEOPLEDEX_HPP
#define GAME_STATES_PEOPLEDEX_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Player/Peopledex.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief The peopledex menu state
 *
 * @ingroup States
 *
 */
class Peopledex
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Create a new Peopledex state
     *
     * @param systems The main game systems
     * @return bl::engine::State::Ptr The new game state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems);

    /**
     * @brief Destroy the Peopledex object
     *
     */
    virtual ~Peopledex() = default;

    /**
     * @brief Returns "Peopledex"
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
    bl::menu::Menu menu;
    core::input::MenuDriver menuDriver;

    bl::resource::Ref<sf::Texture> bgndTxtr;
    sf::Sprite background;

    core::pplmn::Id firstId;
    core::pplmn::Id lastId;
    bl::resource::Ref<sf::Texture> upTxtr;
    sf::Sprite upArrow;
    bl::resource::Ref<sf::Texture> downTxtr;
    sf::Sprite downArrow;

    bl::resource::Ref<sf::Texture> seenTxtr;
    sf::Sprite seenBox;
    sf::Text seenLabel;

    bl::resource::Ref<sf::Texture> ownedTxtr;
    sf::Sprite ownedBox;
    sf::Text ownedLabel;

    bl::resource::Ref<sf::Texture> thumbTxtr;
    sf::Sprite thumbnail;
    sf::Text nameLabel;
    sf::Text descLabel;
    sf::Text locationLabel;

    Peopledex(core::system::Systems& systems);
    void onHighlight(core::pplmn::Id ppl);
    void onSelect(core::pplmn::Id ppl);
    bl::menu::Item::Ptr makeRow(core::pplmn::Id ppl);

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace state
} // namespace game

#endif
