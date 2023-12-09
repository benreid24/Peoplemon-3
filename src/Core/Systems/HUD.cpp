#include <Core/Systems/HUD.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
namespace
{
constexpr float ChoiceHeight  = 25.f;
constexpr float ChoicePadding = 8.f;
constexpr float TextPadding   = 10.f;
constexpr float FlashOn       = 0.75f;
constexpr float FlashOff      = 0.65f;

bool isNextCommand(unsigned int cmd) {
    return cmd == input::Control::Back || cmd == input::Control::Interact;
}

} // namespace

HUD::HUD(Systems& owner)
: owner(owner)
, state(Hidden)
, inputListener(*this)
, screenKeyboard(owner.engine(), std::bind(&HUD::keyboardSubmit, this, std::placeholders::_1))
, entryCard(owner.engine())
, currentOverlay(nullptr) {}

void HUD::ensureCreated() {
    if (!textboxTxtr) {
        textboxTxtr =
            owner.engine().renderer().texturePool().getOrLoadTexture(Properties::TextboxFile());

        const glm::vec2& boxSize = textboxTxtr->size();
        textbox.create(owner.engine(), textboxTxtr);
        textbox.getTransform().setPosition(Properties::WindowSize().x - boxSize.x * 0.5f,
                                           Properties::WindowSize().y - boxSize.y);
        choiceBoxX = boxSize.x * 2.f * 0.75f + 3.f;

        displayText.create(owner.engine(),
                           Properties::MenuFont(),
                           "",
                           Properties::HudFontSize(),
                           sf::Color::Black);
        displayText.getTransform().setPosition(TextPadding, 8.f);
        displayText.wordWrap(textboxTxtr->size().x - TextPadding);
        displayText.setParent(textbox);

        promptTriangle.create(owner.engine(), {0.f, 0.f}, {12.f, 5.5f}, {0.f, 11.f});
        promptTriangle.setFillColor(sf::Color(255, 77, 0));
        promptTriangle.setOutlineColor(sf::Color(255, 238, 128, 185));
        promptTriangle.setOutlineThickness(1.5f);
        promptTriangle.getTransform().setPosition(boxSize - glm::vec2(13.f, 10.f));
        promptTriangle.setParent(textbox);

        choiceMenu.create(owner.engine(),
                          owner.engine().renderer().getObserver(),
                          bl::menu::ArrowSelector::create(10.f, sf::Color::Black));
        choiceMenu.setPadding({0.f, ChoicePadding});
        choiceMenu.setMinHeight(ChoiceHeight);
        choiceMenu.configureBackground(
            sf::Color::White, sf::Color::Black, 2.f, {18.f, 2.f, 4.f, 4.f});

        screenKeyboard.setPosition(
            {Properties::WindowSize().x * 0.5f - screenKeyboard.getSize().x * 0.5f,
             textbox.getTransform().getLocalPosition().y - screenKeyboard.getSize().y - 2.f});
        qtyEntry.setPosition(
            {textbox.getTransform().getLocalPosition().x + textboxTxtr->size().x - 50.f,
             textbox.getTransform().getLocalPosition().y - 70.f});
    }
}

void HUD::update(float dt) {
    switch (state) {
    case Printing:
        if (currentMessage.update(dt)) {
            displayText.getSection().setString(std::string(currentMessage.getVisible()));
        }
        if (currentMessage.finished()) printDoneStateTransition();
        break;

    case WaitingKeyboard:
    case WaitingPrompt:
    case WaitingSticky:
        // noop
        break;

    default:
        break;
    }

    entryCard.update(dt);
}

void HUD::displayMessage(const std::string& msg, const Callback& cb) {
    queuedOutput.emplace(msg, false, true, cb);
    ensureActive();
}

void HUD::displayStickyMessage(const std::string& msg, bool ghost, const Callback& cb) {
    queuedOutput.emplace(msg, true, ghost, cb);
    ensureActive();
}

bool HUD::dismissStickyMessage(bool ignoreGhost) {
    if (queuedOutput.empty()) return false;
    if (queuedOutput.front().getType() != Item::Message) return false;
    if (!queuedOutput.front().isSticky()) return false;
    if (!ignoreGhost && !currentMessage.finished()) return false;

    next();
    return true;
}

void HUD::promptUser(const std::string& prompt, const std::vector<std::string>& choices,
                     const Callback& cb) {
    queuedOutput.emplace(prompt, choices, cb);
    ensureActive();
}

void HUD::getInputString(const std::string& prompt, unsigned int mn, unsigned int mx,
                         const Callback& cb) {
    queuedOutput.emplace(prompt, mn, mx, cb);
    ensureActive();
}

void HUD::getQty(const std::string& prompt, int minQty, int maxQty, const QtyCallback& cb) {
    queuedOutput.emplace(prompt, minQty, maxQty, cb);
    ensureActive();
}

void HUD::displayEntryCard(const std::string& name) { entryCard.display(name); }

void HUD::hideEntryCard() { entryCard.hide(); }

void HUD::ensureActive() {
    ensureCreated();
    if (state == Hidden && !queuedOutput.empty()) {
        owner.engine().inputSystem().getActor().addListener(inputListener);
        startPrinting();
    }
}

void HUD::startPrinting() {
    setState(Printing);
    currentMessage.setContent(queuedOutput.front().getMessage());
    if (!queuedOutput.front().ghostWrite()) { currentMessage.showAll(); }
    displayText.getSection().setString(std::string{currentMessage.getVisible()});
}

void HUD::printDoneStateTransition() {
    switch (queuedOutput.front().getType()) {
    case Item::Message:
        if (!queuedOutput.front().isSticky()) { setState(WaitingContinue); }
        else {
            setState(WaitingSticky);
            queuedOutput.front().getCallback()(queuedOutput.front().getMessage());
        }
        break;

    case Item::Prompt: {
        setState(WaitingPrompt);
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
        const float y              = Properties::WindowSize().y - bounds.height - 18.f;
        choiceMenu.setPosition({choiceBoxX + 18.f, y + 2.f});
        choiceDriver.drive(&choiceMenu);
    } break;

    case Item::Keyboard:
        setState(WaitingKeyboard);
        screenKeyboard.start(queuedOutput.front().minInputLength(),
                             queuedOutput.front().maxInputLength());
        break;

    case Item::Qty:
        setState(WaitingQty);
        qtyEntry.configure(queuedOutput.front().getMinQty(), queuedOutput.front().getMaxQty(), 1);
        break;

    default:
        setState(Hidden);
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
        setState(Hidden);
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
            owner.displayText.getSection().setString(owner.currentMessage.getContent());
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

    case HUD::WaitingSticky:
        // ignore input
        break;

    default:
        BL_LOG_WARN << "Input received by HUD while in invalid state: " << owner.state;
        break;
    }

    return true;
}

void HUD::setState(State ns) {
    textbox.setHidden(false);
    if (state == WaitingKeyboard && ns != WaitingKeyboard) { screenKeyboard.stop(); }
    if (state == WaitingQty && ns != WaitingQty) {
        // TODO - hide qty entry
    }
    if (ns != WaitingContinue) { promptTriangle.stopFlashing(); }
    if (state == Hidden || !currentOverlay ||
        owner.engine().renderer().getObserver().getOrCreateSceneOverlay() != currentOverlay) {
        currentOverlay = owner.engine().renderer().getObserver().getOrCreateSceneOverlay();
        textbox.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
        displayText.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
        promptTriangle.addToScene(currentOverlay, bl::rc::UpdateSpeed::Static);
    }
    if (state == WaitingPrompt && ns != WaitingPrompt) { choiceMenu.removeFromOverlay(); }

    state = ns;
    switch (state) {
    case Hidden:
        textbox.setHidden(true);
        currentOverlay = nullptr;
        break;
    case WaitingContinue:
        promptTriangle.flash(FlashOn, FlashOff);
        break;
    case WaitingPrompt:
        choiceMenu.addToOverlay();
        break;
    case WaitingQty:
        // TODO - add qty to scene
        break;
    case Printing:
    case WaitingSticky:
    case WaitingKeyboard:
    default:
        // noop
        break;
    }
}

HUD::Item::Item(const std::string& msg, bool sticky, bool ghost, const HUD::Callback& cb)
: type(Message)
, cb(cb)
, message(msg)
, data(std::in_place_type_t<std::pair<bool, bool>>{}, sticky, ghost) {}

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

bool HUD::Item::isSticky() const { return std::get_if<std::pair<bool, bool>>(&data)->first; }

bool HUD::Item::ghostWrite() const {
    const auto* p = std::get_if<std::pair<bool, bool>>(&data);
    return p ? p->second : true;
}

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

HUD::EntryCard::EntryCard(bl::engine::Engine& engine)
: engine(engine)
, currentOverlay(nullptr) {
   
}

void HUD::EntryCard::ensureCreated() {
    if (!txtr) {
        txtr = engine.renderer().texturePool().getOrLoadTexture(
            bl::util::FileUtil::joinPath(Properties::MenuImagePath(), "HUD/namecard.png"));
        card.create(engine, txtr);
        text.create(engine, Properties::MenuFont(), "", 20, sf::Color(20, 200, 240));
        text.setParent(card);
        text.wordWrapToParent(0.98f);
    }

    if (!currentOverlay ||
        engine.renderer().getObserver().getOrCreateSceneOverlay() != currentOverlay) {
        currentOverlay = engine.renderer().getObserver().getOrCreateSceneOverlay();
        card.addToScene(currentOverlay, bl::rc::UpdateSpeed::Dynamic);
        text.addToScene(currentOverlay, bl::rc::UpdateSpeed::Dynamic);
    }
}

void HUD::EntryCard::update(float dt) {
    constexpr float MoveSpeed = 150.f;
    constexpr float HoldTime  = 2.5f;

    switch (state) {
    case Dropping:
        stateVar -= MoveSpeed * dt;
        card.getTransform().setPosition(0.f, -stateVar);
        if (stateVar <= 0.f) {
            stateVar = 0.f;
            state    = Holding;
            card.getTransform().setPosition(0.f, 0.f);
        }
        break;

    case Rising:
        stateVar += MoveSpeed * dt;
        card.getTransform().setPosition(0.f, -stateVar);
        if (stateVar >= txtr->size().y) { hide(); }
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

    ensureCreated();
    text.getSection().setString(t);
    text.getTransform().setPosition(txtr->size() * 0.5f - text.getLocalSize() * 0.5f);

    state    = Dropping;
    stateVar = txtr->size().y;
}

void HUD::EntryCard::hide() { state = State::Hidden; }

} // namespace system
} // namespace core
