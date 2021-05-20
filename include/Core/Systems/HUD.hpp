#ifndef CORE_SYSTEMS_HUD_HPP
#define CORE_SYSTEMS_HUD_HPP

#include <BLIB/Media/Shapes.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Menu/GhostWriter.hpp>
#include <Core/Menu/WordWrap.hpp>
#include <Core/Player/Input/Listener.hpp>
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

class HUD {
public:
    using Callback = std::function<void(const std::string&)>;

    HUD(Systems& owner);

    void update(float dt);

    void render(sf::RenderTarget& target);

    void displayMessage(
        const std::string& message, Callback cb = [](const std::string&) {});

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
    menu::GhostWriter currentMessage;

    bl::resource::Resource<sf::Texture>::Ref textboxTxtr;
    sf::Sprite textbox;
    sf::Text displayText;
    bl::shapes::Triangle promptTriangle;

    void ensureActive();
    void startPrinting();
    void printDoneStateTransition();
};

} // namespace system
} // namespace core

#endif
