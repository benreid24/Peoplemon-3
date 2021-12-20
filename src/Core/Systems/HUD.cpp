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

} // namespace

HUD::HUD(Systems& owner)
: owner(owner)
, state(Hidden)
, inputListener(*this)
, textboxTxtr(bl::engine::Resources::textures().load(Properties::TextboxFile()).data)
, viewSize(sf::Vector2f(textboxTxtr->getSize()) * 2.f)
, promptTriangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flashingTriangle(promptTriangle, 0.75f, 0.65f)
, choiceMenu(bl::menu::ArrowSelector::create(10.f, sf::Color::Black))
, choiceBoxX(viewSize.x * 0.75f + 3.f) {
    const sf::Vector2f boxSize = sf::Vector2f(textboxTxtr->getSize());
    textbox.setTexture(*textboxTxtr, true);
    textbox.setPosition(boxSize.x * 0.5f, boxSize.y);

    displayText.setFont(Properties::MenuFont());
    displayText.setCharacterSize(Properties::HudFontSize());
    displayText.setFillColor(sf::Color::Black);
    displayText.setPosition(textbox.getPosition() + sf::Vector2f(10.f, 8.f));
    promptTriangle.setFillColor(sf::Color(255, 77, 0));
    promptTriangle.setOutlineColor(sf::Color(255, 238, 128, 185));
    promptTriangle.setOutlineThickness(1.5f);
    promptTriangle.setPosition(textbox.getPosition() + boxSize - sf::Vector2f(13.f, 10.f));

    choiceMenu.setPadding({0.f, ChoicePadding});
    choiceMenu.setMinHeight(ChoiceHeight);
    choiceDriver.drive(choiceMenu);
    choiceBackground.setFillColor(sf::Color::White);
    choiceBackground.setOutlineColor(sf::Color::Black);
    choiceBackground.setOutlineThickness(1.5f);
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
        choiceMenu.render(target);
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
        state                                   = WaitingPrompt;
        const std::vector<std::string>& choices = queuedOutput.front().getChoices();

        bl::menu::Item::Ptr mitem =
            bl::menu::TextItem::create(choices.empty() ? "INVALID" : choices.front(),
                                       Properties::MenuFont(),
                                       sf::Color::Black,
                                       Properties::HudFontSize());
        mitem->getSignal(bl::menu::Item::Activated)
            .willAlwaysCall(std::bind(&HUD::choiceMade, this, 0));
        choiceMenu.setRootItem(mitem);

        bl::menu::Item* prev = mitem.get();
        for (unsigned int i = 1; i < choices.size(); ++i) {
            mitem = bl::menu::TextItem::create(
                choices[i], Properties::MenuFont(), sf::Color::Black, Properties::HudFontSize());
            mitem->getSignal(bl::menu::Item::Activated)
                .willAlwaysCall(std::bind(&HUD::choiceMade, this, i));
            choiceMenu.addItem(mitem, prev, bl::menu::Item::Bottom);
        }
        const sf::FloatRect& bounds = choiceMenu.getBounds();
        choiceBackground.setSize({bounds.width + 26.f, bounds.height + 14.f});
        const float y = viewSize.y - bounds.height - 18.f;
        choiceBackground.setPosition({choiceBoxX, y});
        choiceMenu.setPosition({choiceBoxX + 18.f, y + 2.f});
    }
}

void HUD::choiceMade(unsigned int i) {
    queuedOutput.front().getCallback()(queuedOutput.front().getChoices()[i]);
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
