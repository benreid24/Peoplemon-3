#include <Game/States/Peopledex.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/Menus/PeopledexRow.hpp>

namespace game
{
namespace state
{
namespace
{
const glm::vec2 MenuPos(420.f, 45.f);
constexpr float MenuWidth    = -1.f;
constexpr float MenuHeight   = 460.f;
constexpr float ArrowPadding = 2.f;

constexpr float InfoTop   = 60.f;
constexpr float InfoLeft  = 25.f;
constexpr float InfoWidth = 375.f;
const glm::vec2 SeenBoxPos(InfoLeft, 485.f);
const glm::vec2 OwnBoxPos(InfoLeft, SeenBoxPos.y + 64.f);
constexpr float InfoPadding = 105.f;

const sf::Color HiddenColor(255, 255, 255, 0);
const sf::Color ShowingColor(255, 255, 255, 255);
} // namespace

bl::engine::State::Ptr Peopledex::create(core::system::Systems& s) { return Ptr(new Peopledex(s)); }

Peopledex::Peopledex(core::system::Systems& s)
: State(s, bl::engine::StateMask::Menu) {
    const std::string& ImgPath = core::Properties::MenuImagePath();
    auto& joinPath             = bl::util::FileUtil::joinPath;

    menu.create(s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());

    bgndTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(ImgPath, "Peopledex/background.png"));
    background.create(s.engine(), bgndTxtr);

    seenTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(ImgPath, "Peopledex/seenBox.png"));
    BL_LOG_INFO << "SeenBox texture: " << seenTxtr.id();
    seenBox.create(s.engine(), seenTxtr);
    seenBox.getTransform().setPosition(SeenBoxPos);
    seenBox.setParent(background);

    seenLabel.create(s.engine(), core::Properties::MenuFont(), "", 26, sf::Color(20, 65, 245));
    seenLabel.setParent(seenBox);
    seenLabel.getTransform().setPosition(InfoPadding - 12.f, seenTxtr->size().y * 0.5f);

    ownedTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(ImgPath, "Peopledex/ownedBox.png"));
    ownedBox.create(s.engine(), ownedTxtr);
    ownedBox.getTransform().setPosition(OwnBoxPos);
    ownedBox.setParent(background);

    ownedLabel.create(s.engine(), core::Properties::MenuFont(), "", 26, sf::Color::Red);
    ownedLabel.setParent(ownedBox);
    ownedLabel.getTransform().setPosition(InfoPadding, ownedTxtr->size().y * 0.5f);

    thumbTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        core::pplmn::Peoplemon::thumbnailImage(core::pplmn::Id::Unknown));
    thumbnail.create(s.engine(), thumbTxtr);
    thumbnail.getTransform().setPosition(InfoLeft + InfoWidth * 0.5f, InfoTop + 20.f);
    thumbnail.getTransform().setOrigin(100.f, 0.f);
    thumbnail.setParent(background);

    nameLabel.create(s.engine(), core::Properties::MenuFont(), "", 26, sf::Color(0, 20, 65));
    nameLabel.getTransform().setPosition(InfoLeft + 8.f, InfoTop + 245.f);
    nameLabel.setParent(background);

    descLabel.create(s.engine(), core::Properties::MenuFont(), "", 18, sf::Color::Black);
    descLabel.setParent(background);
    descLabel.wordWrap(InfoWidth - 20.f);
    descLabel.getTransform().setPosition(InfoLeft + 3.f, InfoTop + 275.f);

    locationLabel.create(s.engine(), core::Properties::MenuFont(), "", 22, sf::Color(0, 65, 20));
    locationLabel.getTransform().setPosition(130.f, 442.f);
    locationLabel.setParent(background);

    upTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(ImgPath, "Peopledex/upArrow.png"));
    BL_LOG_INFO << "UpArrow texture: " << upTxtr.id();
    upArrow.create(s.engine(), upTxtr);
    upArrow.getTransform().setOrigin(0.f, upTxtr->size().y);
    upArrow.setParent(background);

    downTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        joinPath(ImgPath, "Peopledex/downArrow.png"));
    downArrow.create(s.engine(), downTxtr);
    downArrow.setParent(background);

    const auto& all = core::pplmn::Peoplemon::validIds();
    firstId         = all[1];
    lastId          = all.back();

    bl::menu::Item::Ptr first = makeRow(all[1]); // skip id 0
    bl::menu::Item* prev      = first.get();
    menu.setRootItem(first);
    for (unsigned int i = 2; i < all.size(); ++i) {
        bl::menu::Item::Ptr row = makeRow(all[i]);
        menu.addItem(row, prev, bl::menu::Item::Bottom);
        prev = row.get();
    }

    menu.setPosition(MenuPos);
    menu.setMaximumSize({MenuWidth, MenuHeight});
    menu.setSelectedItem(first.get());
    menuDriver.drive(&menu);
    const float mw = menu.getBounds().width;

    upArrow.getTransform().setPosition(MenuPos + glm::vec2(mw * 0.5f, -ArrowPadding));
    downArrow.getTransform().setPosition(MenuPos.x + mw * 0.5f,
                                         static_cast<float>(core::Properties::WindowHeight()) -
                                             downTxtr->size().y - 5.f);
}

const char* Peopledex::name() const { return "Peopledex"; }

void Peopledex::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    upArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    downArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    seenBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    seenLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    ownedBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    ownedLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    thumbnail.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    nameLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    descLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    locationLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    menu.addToOverlay(background.entity());
}

void Peopledex::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    engine.renderer().getObserver().popScene();
}

void Peopledex::update(bl::engine::Engine&, float, float) {}

bool Peopledex::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                        bool fromEvent) {
    if (ctrl == core::input::Control::Back && fromEvent) { systems.engine().popState(); }
    else { menuDriver.sendControl(ctrl, fromEvent); }
    return true;
}

void Peopledex::onHighlight(core::pplmn::Id ppl) {
    if (systems.player().state().peopledex.getIntelLevel(ppl) != core::player::Peopledex::NoIntel) {
        nameLabel.getSection().setString(core::pplmn::Peoplemon::name(ppl));
        descLabel.getSection().setString(core::pplmn::Peoplemon::description(ppl));
        thumbTxtr = systems.engine().renderer().texturePool().getOrLoadTexture(
            core::pplmn::Peoplemon::thumbnailImage(ppl));
    }
    else {
        nameLabel.getSection().setString("???");
        descLabel.getSection().setString("");
        thumbTxtr = systems.engine().renderer().texturePool().getOrLoadTexture(
            core::pplmn::Peoplemon::thumbnailImage(core::pplmn::Id::Unknown));
    }
    thumbnail.getTransform().setScale(200.f / thumbTxtr->size());
    thumbnail.setTexture(thumbTxtr);

    locationLabel.getSection().setString(
        systems.player().state().peopledex.getFirstSeenLocation(ppl));
    seenLabel.getSection().setString(
        std::to_string(systems.player().state().peopledex.getSeen(ppl)));
    seenLabel.getTransform().setOrigin(
        0.f, seenLabel.getLocalBounds().height * 0.5f + seenLabel.getLocalBounds().top * 0.5f);
    ownedLabel.getSection().setString(
        std::to_string(systems.player().state().peopledex.getCaught(ppl)));
    ownedLabel.getTransform().setOrigin(
        0.f, ownedLabel.getLocalBounds().height * 0.5f + ownedLabel.getLocalBounds().top * 0.5f);

    upArrow.setHidden(ppl == firstId);
    downArrow.setHidden(ppl == lastId);
}

void Peopledex::onSelect(core::pplmn::Id) {
    // TODO - open info screen
}

bl::menu::Item::Ptr Peopledex::makeRow(core::pplmn::Id ppl) {
    menu::PeopledexRow::Ptr row =
        menu::PeopledexRow::create(ppl, systems.player().state().peopledex);
    row->getSignal(bl::menu::Item::Activated)
        .willAlwaysCall(std::bind(&Peopledex::onSelect, this, ppl));
    row->getSignal(bl::menu::Item::Selected)
        .willAlwaysCall(std::bind(&Peopledex::onHighlight, this, ppl));
    return row;
}

} // namespace state
} // namespace game
