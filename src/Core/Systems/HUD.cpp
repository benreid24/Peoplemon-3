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

bool isNextCommand(unsigned int cmd) {
    return cmd == input::Control::Back || cmd == input::Control::Interact;
}

} // namespace

HUD::HUD(Systems& owner)
: owner(owner)
, state(Hidden)
, inputListener(*this)
, screenKeyboard(std::bind(&HUD::keyboardSubmit, this, std::placeholders::_1))
, textboxTxtr(bl::engine::Resources::textures().load(Properties::TextboxFile()).data)
, viewSize(static_cast<float>(textboxTxtr->getSize().x) * 2.f,
           static_cast<float>(textboxTxtr->getSize().y) * 4.f)
, promptTriangle({0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f}, true)
, flashingTriangle(promptTriangle, 0.75f, 0.65f)
, choiceMenu(bl::menu::ArrowSelector::create(10.f, sf::Color::Black))
, choiceBoxX(viewSize.x * 0.75f + 3.f) {
    const sf::Vector2f boxSize = sf::Vector2f(textboxTxtr->getSize());
    textbox.setTexture(*textboxTxtr, true);
    textbox.setPosition(boxSize.x * 0.5f, boxSize.y * 3.f);

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
    choiceMenu.configureBackground(sf::Color::White, sf::Color::Black, 2.f, {18.f, 2.f, 4.f, 8.f});
    screenKeyboard.setPosition({viewSize.x * 0.5f - screenKeyboard.getSize().x * 0.5f,
                                textbox.getPosition().y - screenKeyboard.getSize().y - 2.f});
    qtyEntry.setPosition({textbox.getPosition().x + textbox.getGlobalBounds().width - 50.f,
                          textbox.getPosition().y - 70.f});
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

    case WaitingKeyboard:
    case WaitingPrompt:
        // noop
        break;

    default:
        break;
    }

    entryCard.update(dt);
}

void HUD::render(sf::RenderTarget& target, float lag) {
    if (core::Properties::InEditor()) return;

    entryCard.render(target);
    if (state == Hidden) return;

    const sf::View oldView = target.getView();
    target.setView(bl::interface::ViewUtil::computeViewAnchoredPreserveAR(
        viewSize, oldView, 1.f, bl::interface::ViewUtil::Bottom));
    target.draw(textbox);
    target.draw(displayText);

    switch (state) {
    case WaitingContinue:
        flashingTriangle.render(target, {}, lag);
        break;
    case WaitingPrompt:
        choiceMenu.render(target);
        break;
    case WaitingKeyboard:
        target.draw(screenKeyboard);
        break;
    case WaitingQty:
        qtyEntry.render(target);
        break;
    default:
        break;
    }

    target.setView(oldView);
}

void HUD::displayMessage(const std::string& msg, const Callback& cb) {
    sf::Text text = displayText;
    text.setString(msg);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), cb);
    ensureActive();
}

void HUD::promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                     const Callback& cb) {
    sf::Text text = displayText;
    text.setString(prompt);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), choices, cb);
    ensureActive();
}

void HUD::getInputString(const std::string& prompt, unsigned int mn, unsigned int mx,
                         const Callback& cb) {
    sf::Text text = displayText;
    text.setString(prompt);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), mn, mx, cb);
    ensureActive();
}

void HUD::getQty(const std::string& prompt, int minQty, int maxQty, const QtyCallback& cb) {
    sf::Text text = displayText;
    text.setString(prompt);
    bl::interface::wordWrap(text, textboxTxtr->getSize().x);
    queuedOutput.emplace(text.getString().toAnsiString(), minQty, maxQty, cb);
    ensureActive();
}

void HUD::displayEntryCard(const std::string& name) { entryCard.display(name); }

void HUD::hideEntryCard() { entryCard.hide(); }

void HUD::ensureActive() {
    if (state == Hidden && !queuedOutput.empty()) { startPrinting(); }
}

void HUD::startPrinting() {
    state = Printing;
    currentMessage.setContent(queuedOutput.front().getMessage());
    displayText.setString("");
    owner.engine().inputSystem().getActor().addListener(inputListener);
}

void HUD::printDoneStateTransition() {
    switch (queuedOutput.front().getType()) {
    case Item::Message:
        state = WaitingContinue;
        break;

    case Item::Prompt: {
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
            prev = mitem.get();
        }
        const sf::FloatRect bounds = choiceMenu.getBounds();
        const float y              = viewSize.y - bounds.height - 18.f;
        choiceMenu.setPosition({choiceBoxX + 18.f, y + 2.f});
        choiceDriver.drive(&choiceMenu);
    } break;

    case Item::Keyboard:
        state = WaitingKeyboard;
        screenKeyboard.start(queuedOutput.front().minInputLength(),
                             queuedOutput.front().maxInputLength());
        break;

    case Item::Qty:
        state = WaitingQty;
        qtyEntry.configure(queuedOutput.front().getMinQty(), queuedOutput.front().getMaxQty(), 1);
        break;

    default:
        state = Hidden;
        next();
        break;
    }
}

void HUD::choiceMade(unsigned int i) {
    queuedOutput.front().getCallback()(queuedOutput.front().getChoices()[i]);
    choiceDriver.drive(nullptr);
    next();
}

void HUD::qtySelected(int qty) {
    queuedOutput.front().getQtyCallback()(qty);
    next();
}

void HUD::next() {
    queuedOutput.pop();
    if (!queuedOutput.empty()) { startPrinting(); }
    else {
        state = HUD::Hidden;
        owner.engine().inputSystem().getActor().removeListener(inputListener);
    }
}

void HUD::keyboardSubmit(const std::string& i) {
    queuedOutput.front().getCallback()(i);
    screenKeyboard.stop();
    next();
}

HUD::HudListener::HudListener(HUD& o)
: owner(o) {}

bool HUD::HudListener::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                               bool eventTriggered) {
    switch (owner.state) {
    case Printing:
        if (isNextCommand(ctrl) && eventTriggered) {
            owner.currentMessage.showAll();
            owner.displayText.setString(owner.currentMessage.getContent());
            owner.printDoneStateTransition();
        }
        break;

    case WaitingContinue:
        if (isNextCommand(ctrl) && eventTriggered) {
            owner.queuedOutput.front().getCallback()(owner.queuedOutput.front().getMessage());
            owner.next();
        }
        break;

    case WaitingPrompt:
        owner.choiceDriver.sendControl(ctrl, eventTriggered);
        break;

    case WaitingKeyboard:
        owner.screenKeyboard.process(ctrl, eventTriggered);
        break;

    case WaitingQty:
        switch (ctrl) {
        case input::Control::MoveDown:
            owner.qtyEntry.down(1, eventTriggered);
            break;
        case input::Control::MoveUp:
            owner.qtyEntry.up(1, eventTriggered);
            break;
        case input::Control::MoveRight:
            owner.qtyEntry.up(10, eventTriggered);
            break;
        case input::Control::MoveLeft:
            owner.qtyEntry.down(10, eventTriggered);
            break;
        case input::Control::Interact:
            owner.qtySelected(owner.qtyEntry.curQty());
            break;
        case input::Control::Back:
            owner.qtySelected(0);
            break;
        default:
            break;
        }
        break;

    default:
        BL_LOG_WARN << "Input received by HUD while in invalid state: " << owner.state;
        break;
    }

    return true;
}

HUD::Item::Item(const std::string& msg, const HUD::Callback& cb)
: type(Message)
, cb(cb)
, message(msg)
, data(std::in_place_type_t<Empty>{}) {}

HUD::Item::Item(const std::string& p, const std::vector<std::string>& c, const HUD::Callback& cb)
: type(Prompt)
, cb(cb)
, message(p)
, data(std::in_place_type_t<std::vector<std::string>>{}, c) {}

HUD::Item::Item(const std::string& prompt, unsigned int minLen, unsigned int maxLen,
                const Callback& cb)
: type(Keyboard)
, cb(cb)
, message(prompt)
, data(std::in_place_type_t<std::pair<unsigned int, unsigned int>>{}, minLen, maxLen) {}

HUD::Item::Item(const std::string& prompt, int minQty, int maxQty, const QtyCallback& cb)
: type(Qty)
, cb(cb)
, message(prompt)
, data(std::in_place_type_t<std::pair<int, int>>{}, minQty, maxQty) {}

HUD::Item::Type HUD::Item::getType() const { return type; }

const std::string& HUD::Item::getMessage() const { return message; }

const std::vector<std::string>& HUD::Item::getChoices() const {
    return *std::get_if<std::vector<std::string>>(&data);
}

unsigned int HUD::Item::minInputLength() const {
    return std::get_if<std::pair<unsigned int, unsigned int>>(&data)->first;
}

unsigned int HUD::Item::maxInputLength() const {
    return std::get_if<std::pair<unsigned int, unsigned int>>(&data)->second;
}

int HUD::Item::getMinQty() const { return std::get_if<std::pair<int, int>>(&data)->first; }

int HUD::Item::getMaxQty() const { return std::get_if<std::pair<int, int>>(&data)->second; }

const HUD::Callback& HUD::Item::getCallback() const { return *std::get_if<Callback>(&cb); }

const HUD::QtyCallback& HUD::Item::getQtyCallback() const { return *std::get_if<QtyCallback>(&cb); }

HUD::EntryCard::EntryCard() {
    txtr = bl::engine::Resources::textures()
               .load(bl::util::FileUtil::joinPath(Properties::MenuImagePath(), "HUD/namecard.png"))
               .data;
    card.setTexture(*txtr, true);
    text.setFont(Properties::MenuFont());
    text.setCharacterSize(20);
    text.setFillColor(sf::Color(20, 200, 240));
}

void HUD::EntryCard::update(float dt) {
    constexpr float MoveSpeed = 150.f;
    constexpr float HoldTime  = 2.5f;

    switch (state) {
    case Dropping:
        stateVar -= MoveSpeed * dt;
        if (stateVar <= 0.f) {
            stateVar = 0.f;
            state    = Holding;
        }
        break;

    case Rising:
        stateVar += MoveSpeed * dt;
        if (stateVar >= card.getGlobalBounds().height) { state = Hidden; }
        break;

    case Holding:
        stateVar += dt;
        if (stateVar >= HoldTime) {
            state    = Rising;
            stateVar = 0.f;
        }
        break;

    case Hidden:
    default:
        break;
    }
}

void HUD::EntryCard::display(const std::string& t) {
    constexpr float Padding = 10.f;

    text.setString(t);
    bl::interface::wordWrap(text, card.getGlobalBounds().width - Padding * 2.f);
    text.setPosition(card.getGlobalBounds().width * 0.5f - text.getGlobalBounds().width * 0.5f,
                     card.getGlobalBounds().height * 0.5f - text.getGlobalBounds().height * 0.5f);

    state    = Dropping;
    stateVar = card.getGlobalBounds().height;
}

void HUD::EntryCard::render(sf::RenderTarget& target) const {
    static const sf::Vector2f Size(core::Properties::WindowWidth(),
                                   core::Properties::WindowHeight());
    static const sf::Vector2f Center(Size * 0.5f);

    if (state == Hidden) return;

    sf::RenderStates states;
    if (state == Dropping || state == Rising) { states.transform.translate(0.f, -stateVar); }
    states.transform.translate(40.f, 0.f);

    const sf::View oldView = target.getView();
    sf::View view          = oldView;
    view.setCenter(Center);
    view.setSize(Size);
    target.setView(view);
    target.draw(card, states);
    target.draw(text, states);
    target.setView(oldView);
}

void HUD::EntryCard::hide() { state = State::Hidden; }

} // namespace system
} // namespace core
