#ifndef CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP
#define CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP

#include <BLIB/Graphics/Triangle.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Interfaces/Utilities/GhostWriter.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <Core/Input/Control.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Core/Systems/HUD/ScreenKeyboard.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
class BattleState;

namespace view
{
/**
 * @brief Helper utility for printing battle messages
 *
 * @ingroup Battles
 */
class MessagePrinter {
public:
    /**
     * @brief Construct a new Message Printer utility
     */
    MessagePrinter(bl::engine::Engine& engine);

    /**
     * @brief Initializes the UI components. Call after the overlay is created
     */
    void init();

    /**
     * @brief Set the message to be printed
     *
     * @param state The current state of the battle
     * @param message The message to display
     */
    void setMessage(BattleState& state, const cmd::Message& message);

    /**
     * @brief Displays the full message. Call when user presses the continue button
     *
     * @param ctrl The control to process
     * @param ignoreDebounce True to always process, false to rate limit
     */
    void process(input::EntityControl ctrl, bool ignoreDebounce);

    /**
     * @brief Returns true when the full message is on display and finishPrint() has been called
     */
    bool messageDone() const;

    /**
     * @brief Returns whether or not the player chose to forget a move
     */
    bool choseToForget() const;

    /**
     * @brief Whether or not the player chose to set a nickname
     */
    bool choseToSetName() const;

    /**
     * @brief Returns the nickname chosen by the player
     */
    const std::string& chosenNickname() const;

    /**
     * @brief Hides the text once the view is fully synced
     */
    void hide();

    /**
     * @brief Updates the ghost writer
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the message to the given target
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

private:
    enum struct State {
        Hidden,
        Printing,
        ShowingNotAcked,
        ShowingAcked,

        PrintingYesNoChoice,
        WaitingYesNoChoice,
        WaitingNameEntry
    };

    bl::engine::Engine& engine;
    State state;
    bl::gfx::Text text;
    bl::interface::GhostWriter writer;
    bl::gfx::Triangle triangle;

    bl::menu::Menu menu;
    bl::menu::TextItem::Ptr yesItem;
    core::input::MenuDriver inputDriver;
    bool choseYes;
    system::hud::ScreenKeyboard keyboard;

    void finishPrint();
    void makeChoice(bool forget);
    void nameEntered();
    void setState(State state);
};

} // namespace view
} // namespace battle
} // namespace core

#endif
