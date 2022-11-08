#ifndef GAME_STATES_PEOPLEMONINFO_HPP
#define GAME_STATES_PEOPLEMONINFO_HPP

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
 *
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
     *
     */
    virtual ~PeoplemonInfo() = default;

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
    enum ActivePage { Basics, Moves };

    const core::pplmn::OwnedPeoplemon& ppl;
    ActivePage activePage;
    sf::Clock inputTimer;

    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    sf::Sprite background;

    bl::resource::Resource<sf::Texture>::Ref leftTxtr;
    sf::Sprite leftArrow;
    bl::gfx::Flashing leftFlasher;
    bl::resource::Resource<sf::Texture>::Ref rightTxtr;
    sf::Sprite rightArrow;
    bl::gfx::Flashing rightFlasher;
    sf::Text pageLabel;

    bl::resource::Resource<sf::Texture>::Ref thumbTxtr;
    sf::Sprite thumbnail;
    sf::Text nameLabel;
    sf::Text idLabel;
    sf::Text levelLabel;
    sf::Text itemLabel;
    sf::Text curXpLabel;
    sf::Text nextXpLabel;
    sf::Text ailLabel;

    bl::resource::Resource<sf::Texture>::Ref basicsTxtr;
    sf::Sprite basicsBox;
    sf::Text speciesLabel;
    sf::Text typeLabel;
    sf::Text hpLabel;
    sf::Text atkLabel;
    sf::Text defLabel;
    sf::Text spdLabel;
    sf::Text spAtkLabel;
    sf::Text spDefLabel;
    sf::Text descLabel;
    sf::Text abilityLabel;
    sf::Text abilityDescLabel;

    bl::menu::Menu moveMenu;
    bl::resource::Resource<sf::Texture>::Ref moveTxtr;
    sf::Sprite moveBox;
    sf::Text movePwrLabel;
    sf::Text moveAccLabel;
    sf::Text moveTypeLabel;
    sf::Text moveDescLabel;

    PeoplemonInfo(core::system::Systems& systems, const core::pplmn::OwnedPeoplemon& ppl);
    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
    void highlightMove(core::pplmn::MoveId move);
    void setPage(ActivePage page);
};

} // namespace state
} // namespace game

#endif
