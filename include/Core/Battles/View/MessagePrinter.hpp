#ifndef CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP
#define CORE_BATTLES_VIEW_MESSAGEPRINTER_HPP

#include <BLIB/Interfaces/Utilities/GhostWriter.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Media/Shapes/Triangle.hpp>
#include <SFML/Graphics.hpp>

namespace core
{
namespace battle
{
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
     * @param message The message to display
     */
    void setMessage(const std::string& message);

    /**
     * @brief Displays the full message. Call when user presses the continue button
     *
     */
    void finishPrint();

    /**
     * @brief Returns true when the full message is on display and finishPrint() has been called
     *
     */
    bool messageDone() const;

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
    sf::Text text;
    bl::interface::GhostWriter writer;
    bl::shapes::Triangle triangle;
    bl::gfx::Flashing flasher;
};

} // namespace view
} // namespace battle
} // namespace core

#endif