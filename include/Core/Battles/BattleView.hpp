#ifndef GAME_BATTLES_BATTLEVIEW_HPP
#define GAME_BATTLES_BATTLEVIEW_HPP

#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <Core/Battles/View/MessagePrinter.hpp>
#include <Core/Battles/View/MoveAnimation.hpp>
#include <Core/Battles/View/PeoplemonAnimation.hpp>
#include <Core/Battles/View/PlayerMenu.hpp>
#include <Core/Battles/View/StatBoxes.hpp>
#include <Core/Player/Input/Listener.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

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
     */
    BattleView();

    /**
     * @brief Returns true if the view is done going through the queued commands and all components
     *        are synchronised with the desired state. Returns false if the view is still changing,
     *        playing animations, or printing messages
     *
     */
    bool actionsCompleted() const;

    /**
     * @brief Queues a message to be displayed
     *
     * @param message The message to display
     */
    void queueMessage(const Message& message);

    /**
     * @brief Begins playing the given animation
     *
     * @param animation The animation to play
     */
    void playAnimation(const Animation& animation);

    /**
     * @brief Synchronises the display with the battle state. This updates health bars, XP bar,
     *        ailments, and peoplemon graphics
     *
     * @param state The current state of the battle
     */
    void syncDisplay(const BattleState& state);

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
    sf::Text temp;

    view::PlayerMenu playerMenu;
    view::StatBoxes statBoxes;
    view::MessagePrinter printer;
    view::PeoplemonAnimation localPeoplemon;
    view::PeoplemonAnimation opponentPeoplemon;
    view::MoveAnimation moveAnimation;

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
