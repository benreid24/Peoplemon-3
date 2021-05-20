#include <Core/Systems/HUD.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Menu/ViewUtil.hpp>
#include <Core/Menu/WordWrap.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
bool isNextCommand(component::Command cmd) {
    return cmd == component::Command::Back || cmd == component::Command::Interact;
}
} // namespace

HUD::HUD(Systems& owner)
: owner(owner)
, state(Hidden)
, inputListener(*this)
, promptTriangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flashingTriangle(promptTriangle, 0.75f, 0.65f) {
    textboxTxtr = bl::engine::Resources::textures().load(Properties::TextboxFile()).data;
    textbox.setTexture(*textboxTxtr, true);
    displayText.setFont(Properties::MenuFont());
    displayText.setCharacterSize(Properties::HudFontSize());
    displayText.setFillColor(sf::Color::Black);
    displayText.setPosition(static_cast<float>(textboxTxtr->getSize().x) * 0.03f,
                            static_cast<float>(textboxTxtr->getSize().y) * 0.03f);
    promptTriangle.setFillColor(sf::Color(255, 77, 0));
    promptTriangle.setOutlineColor(sf::Color(255, 238, 128, 185));
    promptTriangle.setOutlineThickness(1.5f);
    promptTriangle.setPosition(static_cast<float>(textboxTxtr->getSize().x) * 0.96f,
                               static_cast<float>(textboxTxtr->getSize().y) * 0.92f);
}

void HUD::update(float dt) {
    switch (state) {
    case Printing:
        if (currentMessage.update(dt)) {
            displayText.setString(std::string(currentMessage.getVisible()));
        }
        if (currentMessage.finished()) printDoneStateTransition();
        break;

    case WaitingContinue:
        flashingTriangle.update(dt);
        break;

    case WaitingPrompt:
        // TODO - add menu and update it
        break;

    default:
        break;
    }
}

void HUD::render(sf::RenderTarget& target, float lag) {
    if (state == Hidden) return;

    const sf::View oldView = target.getView();
    target.setView(menu::ViewUtil::computeViewAnchoredPreserveAR(
        static_cast<sf::Vector2f>(textboxTxtr->getSize()), oldView, 0.7f, menu::ViewUtil::Bottom));
    target.draw(textbox);
    target.draw(displayText);
    if (state == WaitingContinue) { flashingTriangle.render(target, {}, lag); }
    else if (state == WaitingPrompt) {
        // TODO - draw menu prompt if visible
    }
    target.setView(oldView);
}

void HUD::displayMessage(const std::string& msg, Callback cb) {
    sf::Text text = displayText;
    text.setString(msg);
    menu::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), cb);
    ensureActive();
}

void HUD::promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                     Callback cb) {
    sf::Text text = displayText;
    text.setString(prompt);
    menu::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), choices, cb);
    ensureActive();
}

void HUD::ensureActive() {
    if (state == Hidden && !queuedOutput.empty()) {
        state = Printing;
        startPrinting();
    }
}

void HUD::startPrinting() {
    currentMessage.setContent(queuedOutput.front().getMessage());
    displayText.setString("");
    owner.player().inputSystem().addListener(inputListener);
}

void HUD::printDoneStateTransition() {
    state = queuedOutput.front().getType() == Item::Message ? WaitingContinue : WaitingPrompt;
}

HUD::HudListener::HudListener(HUD& o)
: owner(o) {}

void HUD::HudListener::process(component::Command cmd) {
    switch (owner.state) {
    case Printing:
        if (isNextCommand(cmd)) {
            owner.currentMessage.showAll();
            owner.displayText.setString(owner.currentMessage.getContent());
            owner.printDoneStateTransition();
        }
        break;

    case WaitingContinue:
        if (isNextCommand(cmd)) {
            owner.queuedOutput.pop();
            if (!owner.queuedOutput.empty()) { owner.startPrinting(); }
            else {
                owner.state = HUD::Hidden;
                owner.owner.player().inputSystem().removeListener(owner.inputListener);
            }
        }
        break;

    case WaitingPrompt:
        // TODO - update menu. Can pass command into unsubscribed menu driver input listener
        break;

    default:
        BL_LOG_WARN << "Input received by HUD while in invalid state: " << owner.state;
        break;
    }
}

HUD::Item::Item(const std::string& msg, HUD::Callback cb)
: type(Message)
, cb(cb)
, message(msg) {}

HUD::Item::Item(const std::string& p, const std::vector<std::string>& c, HUD::Callback cb)
: type(Prompt)
, cb(cb)
, message(p)
, choices(c) {}

HUD::Item::Type HUD::Item::getType() const { return type; }

const std::string& HUD::Item::getMessage() const { return message; }

const std::vector<std::string>& HUD::Item::getChoices() const { return choices.value(); }

const HUD::Callback& HUD::Item::getCallback() const { return cb; }

} // namespace system
} // namespace core
