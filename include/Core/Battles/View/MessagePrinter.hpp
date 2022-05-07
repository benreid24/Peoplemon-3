#ifndef CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP
#define CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP

#include <Core/Player/Input/MenuDriver.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Interfaces/Utilities/GhostWriter.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Media/Shapes/Triangle.hpp>
#include <Core/Battles/Commands/Message.hpp>
#include <SFML/Graphics.hpp>
#include <Core/Components/Command.hpp>

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
 *
 */
class MessagePrinter {
public:
    /**
     * @brief Construct a new Message Printer utility
     *
     */
    MessagePrinter();

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
     */
    void process(component::Command cmd);

    /**
     * @brief Returns true when the full message is on display and finishPrint() has been called
     *
     */
    bool messageDone() const;

    /**
     * @brief Returns whether or not the player chose to forget a move
     *
     */
    bool choseToForget() const;

    /**
     * @brief Hides the text once the view is fully synced
     *
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

        PrintingMoveChoice,
        WaitingMoveChoice
    };

    State state;
    sf::Text text;
    bl::interface::GhostWriter writer;
    bl::shapes::Triangle triangle;
    bl::gfx::Flashing flasher;

    bl::menu::Menu menu;
    bl::menu::TextItem::Ptr yesItem;
    core::player::input::MenuDriver inputDriver;
    bool forget;

    void finishPrint();
    void makeChoice(bool forget);
};

} // namespace view
} // namespace battle
} // namespace core

#endif
