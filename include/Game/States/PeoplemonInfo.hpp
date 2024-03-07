#ifndef GAME_STATES_PEOPLEMONINFO_HPP
#define GAME_STATES_PEOPLEMONINFO_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Game/States/State.hpp>

namespace game
{
namespace state
{
/**
 * @brief Menu for displaying information about a specific peoplemon the player owns
 *
 * @ingroup States
 */
class PeoplemonInfo
: public State
, public bl::input::Listener {
public:
    /**
     * @brief Create a new PeoplemonInfo engine state
     *
     * @param systems The main game systems
     * @param ppl The peoplemon to display for
     * @return bl::engine::State::Ptr The new game state
     */
    static bl::engine::State::Ptr create(core::system::Systems& systems,
                                         const core::pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Destroy the Peoplemon Info object
     */
    virtual ~PeoplemonInfo() = default;

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
    enum ActivePage { Basics, Moves };

    ActivePage activePage;
    float inputDebounce;
    float flashDelay;

    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;

    bl::rc::res::TextureRef leftTxtr;
    bl::gfx::Sprite leftArrow;
    bl::rc::res::TextureRef rightTxtr;
    bl::gfx::Sprite rightArrow;
    bl::gfx::Text pageLabel;

    bl::rc::res::TextureRef thumbTxtr;
    bl::gfx::Sprite thumbnail;
    bl::gfx::Text nameLabel;
    bl::gfx::Text idLabel;
    bl::gfx::Text levelLabel;
    bl::gfx::Text itemLabel;
    bl::gfx::Text curXpLabel;
    bl::gfx::Text nextXpLabel;
    bl::gfx::Text ailLabel;

    bl::rc::res::TextureRef basicsTxtr;
    bl::gfx::Sprite basicsBox;
    bl::gfx::Text speciesLabel;
    bl::gfx::Text typeLabel;
    bl::gfx::Text hpLabel;
    bl::gfx::Text atkLabel;
    bl::gfx::Text defLabel;
    bl::gfx::Text spdLabel;
    bl::gfx::Text spAtkLabel;
    bl::gfx::Text spDefLabel;
    bl::gfx::Text descLabel;
    bl::gfx::Text abilityLabel;
    bl::gfx::Text abilityDescLabel;

    bl::menu::Menu moveMenu;
    bl::rc::res::TextureRef moveTxtr;
    bl::gfx::Sprite moveBox;
    bl::gfx::Text movePwrLabel;
    bl::gfx::Text moveAccLabel;
    bl::gfx::Text moveTypeLabel;
    bl::gfx::Text moveDescLabel;

    PeoplemonInfo(core::system::Systems& systems, const core::pplmn::OwnedPeoplemon& ppl);
    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
    void highlightMove(core::pplmn::MoveId move);
    void setPage(ActivePage page);
};

} // namespace state
} // namespace game

#endif
