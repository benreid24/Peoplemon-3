#ifndef CORE_SYSTEMS_HUD_HPP
#define CORE_SYSTEMS_HUD_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Interfaces/Utilities.hpp>
#include <BLIB/Media/Graphics/Flashing.hpp>
#include <BLIB/Media/Shapes.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Input/Listener.hpp>
#include <Core/Player/Input/MenuDriver.hpp>
#include <SFML/Graphics.hpp>
#include <functional>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief The primary HUD system for the player. Manages displaying messages and asking questions. A
 *        callback is issued when messages are complete or when choices are made
 *
 * @ingroup Systems
 *
 */
class HUD {
public:
    /**
     * @brief Signature for HUD callbacks. Used for both messages completing and choices made
     *
     * @param value The message that completed or the choice that was chosen
     *
     */
    using Callback = std::function<void(const std::string& value)>;

    /**
     * @brief Construct a new HUD system
     *
     * @param owner The primary systems object
     */
    HUD(Systems& owner);

    /**
     * @brief Updates any displayed elements
     *
     * @param dt Time elapsed, in seconds
     */
    void update(float dt);

    /**
     * @brief Renders the HUD if any elements are visible
     *
     * @param target The target to render to
     * @param lag Time elapsed not accounted for in update
     */
    void render(sf::RenderTarget& target, float lag);

    /**
     * @brief Displays a message in the HUD textbox. Messages are queued in order that they arrive
     *
     * @param message The message to display
     * @param cb Callback to issue when the message is completed and the player hits continue
     */
    void displayMessage(
        const std::string& message, Callback cb = [](const std::string&) {});

    /**
     * @brief Asks the player a question through the HUD
     *
     * @param prompt The question to ask
     * @param choices The available choices the player can make
     * @param cb The callback to issue with the choice that was chosen when the player chooses
     */
    void promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                    Callback cb);

private:
    enum State { Hidden, Printing, WaitingContinue, WaitingPrompt };

    class Item {
    public:
        enum Type { Message, Prompt };

        Item(const std::string& message, Callback cb);
        Item(const std::string& prompt, const std::vector<std::string>& choices, Callback cb);

        Type getType() const;
        const std::string& getMessage() const;
        const std::vector<std::string>& getChoices() const;
        const Callback& getCallback() const;

    private:
        const Type type;
        const Callback cb;
        const std::string message;
        const std::optional<std::vector<std::string>> choices;
    };

    struct HudListener : public player::input::Listener {
        HudListener(HUD& owner);
        virtual ~HudListener() = default;
        virtual void process(component::Command cmd) override;

        HUD& owner;
    };

    Systems& owner;
    State state;

    HudListener inputListener;
    std::queue<Item> queuedOutput;
    bl::interface::GhostWriter currentMessage;

    bl::resource::Resource<sf::Texture>::Ref textboxTxtr;
    const sf::Vector2f viewSize;
    sf::Sprite textbox;
    sf::Text displayText;
    bl::shapes::Triangle promptTriangle;
    bl::gfx::Flashing flashingTriangle;

    sf::RectangleShape choiceBackground;
    bl::menu::Menu choiceMenu;
    core::player::input::MenuDriver choiceDriver;
    const float choiceBoxX;

    void ensureActive();
    void startPrinting();
    void printDoneStateTransition();
    void choiceMade(unsigned int i);
    void next();
};

} // namespace system
} // namespace core

#endif
