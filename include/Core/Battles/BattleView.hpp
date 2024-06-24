#ifndef GAME_BATTLES_BATTLEVIEW_HPP
#define GAME_BATTLES_BATTLEVIEW_HPP

#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <Core/Battles/Commands/Animation.hpp>
#include <Core/Battles/Commands/Command.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <Core/Battles/View/MessagePrinter.hpp>
#include <Core/Battles/View/MoveAnimation.hpp>
#include <Core/Battles/View/PeoplemonAnimation.hpp>
#include <Core/Battles/View/PlayerMenu.hpp>
#include <Core/Battles/View/StatBoxes.hpp>
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
 */
class BattleView : public bl::input::Listener {
public:
    /**
     * @brief Construct a new Battle View
     *
     * @param engine The game engine instance
     * @param state The current state of the battle
     * @param canRun Whether or not the player menu should allow running
     */
    BattleView(bl::engine::Engine& engine, BattleState& state, bool canRun);

    /**
     * @brief Initializes renderer data, loads resources, and adds entities to the scene
     *
     * @param scene The scene to add entities to
     */
    void init(bl::rc::scene::CodeScene* scene);

    /**
     * @brief Access the player's menu
     */
    view::PlayerMenu& menu();

    /**
     * @brief Returns whether or not the player chose to forget a move when prompted
     */
    bool playerChoseForgetMove() const;

    /**
     * @brief Returns whether or not the player chose to set a nickname on a new Peoplemon
     */
    bool playerChoseToSetName() const;

    /**
     * @brief Returns the nickname the player entered
     */
    const std::string& chosenNickname() const;

    /**
     * @brief Returns true if the view is done going through the queued commands and all components
     *        are synchronized with the desired state. Returns false if the view is still changing,
     *        playing animations, or printing messages
     */
    bool actionsCompleted() const;

    /**
     * @brief Hides the battle text when the view is synced
     */
    void hideText();

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
     * @brief Renders the battle view
     *
     * @param ctx The render context
     */
    void render(bl::rc::scene::CodeScene::RenderContext& ctx);

private:
    bl::engine::Engine& engine;
    BattleState& battleState;
    view::PlayerMenu playerMenu;
    view::StatBoxes statBoxes;
    view::MessagePrinter printer;
    view::PeoplemonAnimation localPeoplemon;
    view::PeoplemonAnimation opponentPeoplemon;
    view::MoveAnimation moveAnimation;

    bl::rc::res::TextureRef bgndTxtr;
    bl::gfx::Sprite background;

    virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                         bl::input::DispatchType, bool eventTriggered) override;
};

} // namespace battle
} // namespace core

#endif
