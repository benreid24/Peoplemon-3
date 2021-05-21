#include <Core/Systems/HUD.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
constexpr float ChoiceHeight  = 25.f;
constexpr float ChoicePadding = 8.f;

bool isNextCommand(component::Command cmd) {
    return cmd == component::Command::Back || cmd == component::Command::Interact;
}

sf::Text choiceText() {
    sf::Text text;
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(Properties::HudFontSize());
    text.setFillColor(sf::Color::Black);
    return text;
}

} // namespace

HUD::HUD(Systems& owner)
: owner(owner)
, state(Hidden)
, inputListener(*this)
, promptTriangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flashingTriangle(promptTriangle, 0.75f, 0.65f) {
    textboxTxtr = bl::engine::Resources::textures().load(Properties::TextboxFile()).data;
    const sf::Vector2f boxSize = static_cast<sf::Vector2f>(textboxTxtr->getSize());
    textbox.setTexture(*textboxTxtr, true);
    textbox.setPosition(boxSize.x * 0.5f, boxSize.y);
    viewSize = boxSize * 2.f;

    displayText.setFont(Properties::MenuFont());
    displayText.setCharacterSize(Properties::HudFontSize());
    displayText.setFillColor(sf::Color::Black);
    displayText.setPosition(textbox.getPosition() + sf::Vector2f(10.f, 8.f));
    promptTriangle.setFillColor(sf::Color(255, 77, 0));
    promptTriangle.setOutlineColor(sf::Color(255, 238, 128, 185));
    promptTriangle.setOutlineThickness(1.5f);
    promptTriangle.setPosition(textbox.getPosition() + boxSize - sf::Vector2f(13.f, 10.f));

    choiceArrow = bl::menu::ArrowSelector::create(10.f);
    choiceArrow->getArrow().setFillColor(sf::Color::Black);
    choiceRenderer.setVerticalPadding(ChoicePadding);
    choiceRenderer.setUniformSize({0.f, ChoiceHeight});
    choiceBackground.setFillColor(sf::Color::White);
    choiceBackground.setOutlineColor(sf::Color::Black);
    choiceBackground.setOutlineThickness(1.5f);
    choicePosition.x = viewSize.x * 0.5f + boxSize.x * 0.5f + 3.f;
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
        // noop
        break;

    default:
        break;
    }
}

void HUD::render(sf::RenderTarget& target, float lag) {
    if (state == Hidden) return;

    const sf::View oldView = target.getView();
    target.setView(bl::interface::ViewUtil::computeViewAnchoredPreserveAR(
        viewSize, oldView, 1.f, bl::interface::ViewUtil::Bottom));
    target.draw(textbox);
    target.draw(displayText);
    if (state == WaitingContinue) { flashingTriangle.render(target, {}, lag); }
    else if (state == WaitingPrompt) {
        target.draw(choiceBackground);
        choiceMenu.get().render(choiceRenderer, target, choicePosition);
    }
    target.setView(oldView);
}

void HUD::displayMessage(const std::string& msg, Callback cb) {
    sf::Text text = displayText;
    text.setString(msg);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), cb);
    ensureActive();
}

void HUD::promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                     Callback cb) {
    sf::Text text = displayText;
    text.setString(prompt);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), choices, cb);
    ensureActive();
}

void HUD::ensureActive() {
    if (state == Hidden && !queuedOutput.empty()) { startPrinting(); }
}

void HUD::startPrinting() {
    state = Printing;
    currentMessage.setContent(queuedOutput.front().getMessage());
    displayText.setString("");
    owner.player().inputSystem().addListener(inputListener);
}

void HUD::printDoneStateTransition() {
    if (queuedOutput.front().getType() == Item::Message) { state = WaitingContinue; }
    else {
        state = WaitingPrompt;

        const std::vector<std::string>& choices = queuedOutput.front().getChoices();
        sf::Text text                           = choiceText();

        text.setString(choices.empty() ? "INVALID" : choices.front());
        choiceItems.push_back(bl::menu::Item::create(bl::menu::TextRenderItem::create(text)));
        sf::Vector2f size(
            text.getGlobalBounds().width,
            std::max(text.getGlobalBounds().height + text.getGlobalBounds().top, ChoiceHeight));
        for (unsigned int i = 1; i < choices.size(); ++i) {
            text.setString(choices[i]);
            choiceItems.push_back(bl::menu::Item::create(bl::menu::TextRenderItem::create(text)));
            choiceItems[i - 1]->attach(choiceItems.back(), bl::menu::Item::Bottom);

            size.x = std::max(size.x, text.getGlobalBounds().width);
            size.y +=
                std::max(text.getGlobalBounds().height + text.getGlobalBounds().top, ChoiceHeight) +
                ChoicePadding;
        }
        for (unsigned int i = 0; i < choices.size(); ++i) {
            const auto cb = [this, i]() { choiceMade(i); };
            choiceItems[i]->getSignal(bl::menu::Item::Activated).willCall(cb);
        }
        choiceMenu.emplace(choiceItems.front(), choiceArrow);
        choiceDriver.drive(choiceMenu.get());
        choiceBackground.setSize(size + sf::Vector2f(26.f, 14.f));
        choicePosition.y = viewSize.y - size.y - 18.f;
        choiceBackground.setPosition(choicePosition);
        choicePosition += sf::Vector2f(18.f, 2.f);
    }
}

void HUD::choiceMade(unsigned int i) {
    queuedOutput.front().getCallback()(queuedOutput.front().getChoices()[i]);
    choiceMenu.destroy();
    choiceItems.clear();
    next();
}

void HUD::next() {
    queuedOutput.pop();
    if (!queuedOutput.empty()) { startPrinting(); }
    else {
        state = HUD::Hidden;
        owner.player().inputSystem().removeListener(inputListener);
    }
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
            owner.queuedOutput.front().getCallback()(owner.queuedOutput.front().getMessage());
            owner.next();
        }
        break;

    case WaitingPrompt:
        owner.choiceDriver.process(cmd);
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
