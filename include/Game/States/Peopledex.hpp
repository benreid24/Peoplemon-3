#ifndef GAME_STATES_PEOPLEDEX_HPP
#define GAME_STATES_PEOPLEDEX_HPP

#include <BLIB/Graphics.hpp>
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
     */
    virtual ~Peopledex() = default;

    /**
     * @brief Returns "Peopledex"
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
    virtual void update(bl::engine::Engine& engine, float dt, float) override;

private:
    bl::menu::Menu menu;
    core::input::MenuDriver menuDriver;

    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;

    core::pplmn::Id firstId;
    core::pplmn::Id lastId;
    bl::rc::res::TextureRef upTxtr;
    bl::gfx::Sprite upArrow;
    bl::rc::res::TextureRef downTxtr;
    bl::gfx::Sprite downArrow;

    bl::rc::res::TextureRef seenTxtr;
    bl::gfx::Sprite seenBox;
    bl::gfx::Text seenLabel;

    bl::rc::res::TextureRef ownedTxtr;
    bl::gfx::Sprite ownedBox;
    bl::gfx::Text ownedLabel;

    bl::rc::res::TextureRef thumbTxtr;
    bl::gfx::Sprite thumbnail;
    bl::gfx::Text nameLabel;
    bl::gfx::Text descLabel;
    bl::gfx::Text locationLabel;

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
