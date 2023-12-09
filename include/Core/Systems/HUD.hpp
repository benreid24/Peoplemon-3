#ifndef CORE_SYSTEMS_HUD_HPP
#define CORE_SYSTEMS_HUD_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Interfaces/Utilities.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Input/MenuDriver.hpp>
#include <Core/Systems/HUD/QtyEntry.hpp>
#include <Core/Systems/HUD/ScreenKeyboard.hpp>
#include <functional>
#include <queue>
#include <string>
#include <variant>
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
     * @brief Called when a qty is entered
     *
     * @param qty The user selected qty
     *
     */
    using QtyCallback = std::function<void(int qty)>;

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
     * @brief Displays a message in the HUD textbox. Messages are queued in order that they arrive
     *
     * @param message The message to display
     * @param cb Callback to issue when the message is completed and the player hits continue
     */
    void displayMessage(
        const std::string& message, const Callback& cb = [](const std::string&) {});

    /**
     * @brief Displays a message in the HUD textbox. Sticky messages stay displayed until
     *        programmatically dismissed. Events can queue up behind sticky messages
     *
     * @param message The message to display
     * @param ghostWrite True to display text char by char, false to show entire message at once
     * @param cb Callback to issue when the message is fully shown
     */
    void displayStickyMessage(
        const std::string& message, bool ghostWrite,
        const Callback& cb = [](const std::string&) {});

    /**
     * @brief Dismisses the currrently active sticky message
     *
     * @param ignoreGhostWrite True to dismiss even if message is not done printing, false to wait
     * @return True if a message was dismissed, false otherwise
     */
    bool dismissStickyMessage(bool ignoreGhostWrite = true);

    /**
     * @brief Asks the player a question through the HUD
     *
     * @param prompt The question to ask
     * @param choices The available choices the player can make
     * @param cb The callback to issue with the choice that was chosen when the player chooses
     */
    void promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                    const Callback& cb);

    /**
     * @brief Prompts the player to input a string using the screen keyboard
     *
     * @param prompt Text prompt to display. Does not ghost write in
     * @param minLen Minimum length string to allow
     * @param maxLen Maximum length string to allow
     * @param cb Callback to trigger when the string is entered
     */
    void getInputString(const std::string& prompt, unsigned int minLen, unsigned int maxLen,
                        const Callback& cb);

    /**
     * @brief Gets a number from the player
     *
     * @param prompt Text to prompt the player with
     * @param minQty The minimum number to accept
     * @param maxQty The maximum number to accept
     * @param cb Callback to call with the selected number
     */
    void getQty(const std::string& prompt, int minQty, int maxQty, const QtyCallback& cb);

    /**
     * @brief Displays a card to indicate entering a new town, route, or map
     *
     * @param name The name to display inside the card
     */
    void displayEntryCard(const std::string& name);

    /**
     * @brief Hides the entry card
     *
     */
    void hideEntryCard();

private:
    enum State {
        Hidden,
        Printing,
        WaitingContinue,
        WaitingSticky,
        WaitingPrompt,
        WaitingKeyboard,
        WaitingQty
    };

    class Item {
    public:
        enum Type { Message, Prompt, Keyboard, Qty };

        Item(const std::string& message, bool sticky, bool ghost, const Callback& cb);
        Item(const std::string& prompt, const std::vector<std::string>& choices,
             const Callback& cb);
        Item(const std::string& prompt, unsigned int minLen, unsigned int maxLen,
             const Callback& cb);
        Item(const std::string& prompt, int minQty, int maxQty, const QtyCallback& cb);

        Type getType() const;
        const std::string& getMessage() const;
        bool isSticky() const;
        bool ghostWrite() const;
        const std::vector<std::string>& getChoices() const;
        const Callback& getCallback() const;
        const QtyCallback& getQtyCallback() const;
        unsigned int minInputLength() const;
        unsigned int maxInputLength() const;
        int getMinQty() const;
        int getMaxQty() const;

    private:
        const Type type;
        const std::variant<Callback, QtyCallback> cb;
        const std::string message;
        const std::variant<std::pair<bool, bool>, std::vector<std::string>,
                           std::pair<unsigned int, unsigned int>, std::pair<int, int>>
            data;
    };

    struct HudListener : public bl::input::Listener {
        HudListener(HUD& owner);
        virtual ~HudListener() = default;
        virtual bool observe(const bl::input::Actor&, unsigned int activatedControl,
                             bl::input::DispatchType, bool eventTriggered) override;

        HUD& owner;
    };

    class EntryCard {
    public:
        EntryCard(bl::engine::Engine& engine);
        void display(const std::string& text);
        void update(float dt);
        void hide();

    private:
        bl::engine::Engine& engine;
        bl::rc::Overlay* currentOverlay;
        bl::rc::res::TextureRef txtr;
        bl::gfx::Sprite card;
        bl::gfx::Text text;

        void ensureCreated();

        enum State { Hidden, Dropping, Holding, Rising } state;
        float stateVar;
    };

    Systems& owner;
    State state;

    HudListener inputListener;
    std::queue<Item> queuedOutput;
    bl::interface::GhostWriter currentMessage;
    hud::ScreenKeyboard screenKeyboard;
    EntryCard entryCard;

    bl::rc::Overlay* currentOverlay;
    bl::rc::res::TextureRef textboxTxtr;
    bl::gfx::Sprite textbox;
    bl::gfx::Text displayText;
    bl::gfx::Triangle promptTriangle;
    hud::QtyEntry qtyEntry;

    bl::menu::Menu choiceMenu;
    core::input::MenuDriver choiceDriver;
    float choiceBoxX;

    void setState(State newState);
    void ensureCreated();
    void ensureActive();
    void startPrinting();
    void printDoneStateTransition();
    void choiceMade(unsigned int i);
    void keyboardSubmit(const std::string& input);
    void qtySelected(int qty);
    void next();
};

} // namespace system
} // namespace core

#endif
