#ifndef GAME_BATTLES_BATTLEVIEW_HPP
#define GAME_BATTLES_BATTLEVIEW_HPP

#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Battles/Commands/Command.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <Core/Battles/View/MessagePrinter.hpp>
#include <Core/Battles/View/MoveAnimation.hpp>
#include <Core/Battles/View/PeoplemonAnimation.hpp>
#include <Core/Battles/View/PlayerMenu.hpp>
#include <Core/Battles/View/StatBoxes.hpp>
#include <Core/Player/Input/Listener.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <queue>

namespace core
{
namespace battle
{
class BattleState;

/**
 * @brief This is the top level class for rendering battles. It takes commands and state from a
 *        BattleController and updates itself independently of battle logic
 *
 * @ingroup Battles
 *
 */
class BattleView : public player::input::Listener {
public:
    /**
     * @brief Construct a new Battle View
     *
     * @param state The current state of the battle
     * @param canRun Whether or not the player menu should allow running
     */
    BattleView(BattleState& state, bool canRun);

    /**
     * @brief Sets up the subviews from the view used during battle
     *
     * @param parentView The view to be used
     */
    void configureView(const sf::View& parentView);

    /**
     * @brief Access the player's menu
     *
     */
    view::PlayerMenu& menu();

    /**
     * @brief Returns true if the view is done going through the queued commands and all components
     *        are synchronised with the desired state. Returns false if the view is still changing,
     *        playing animations, or printing messages
     *
     */
    bool actionsCompleted() const;

    /**
     * @brief Processes a command and updates the view
     *
     * @param command The command to process
     */
    void processCommand(const Command& command);

    /**
     * @brief Updates the view, including contained animations and printing text
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the view to the given target. The target's camera must have the correct size
     *        from Properties and be centered properly.
     *
     * @param target The target to render to
     * @param lag Time elapsed in seconds since update()
     */
    void render(sf::RenderTarget& target, float lag) const;

private:
    BattleState& battleState;
    view::PlayerMenu playerMenu;
    view::StatBoxes statBoxes;
    view::MessagePrinter printer;
    view::PeoplemonAnimation localPeoplemon;
    view::PeoplemonAnimation opponentPeoplemon;
    view::MoveAnimation moveAnimation;

    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    sf::Sprite background;

    virtual void process(component::Command control) override;

    /*
    For switches thinking of a multistep approach of
      1. Play recall anim
      2. Sync state
      3. Play send-out anim

      Peoplemon animations when created, or when peoplemon change, start in a hidden state and only
      go the regular state when a send-out anim is triggered. This makes switching and battle intro
      easier to do with the current command model
    */
};

} // namespace battle
} // namespace core

#endif
