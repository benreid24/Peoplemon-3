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
: State(s)
//, menu(bl::menu::NoSelector::create()) {
{
    const std::string& ImgPath = core::Properties::MenuImagePath();
    auto& joinPath             = bl::util::FileUtil::joinPath;

    bgndTxtr = TextureManager::load(joinPath(ImgPath, "Peopledex/background.png"));
    background.setTexture(*bgndTxtr, true);

    seenTxtr = TextureManager::load(joinPath(ImgPath, "Peopledex/seenBox.png"));
    seenBox.setTexture(*seenTxtr, true);
    // seenBox.setPosition(SeenBoxPos);
    // seenLabel.setFont(core::Properties::MenuFont());
    seenLabel.setCharacterSize(26);
    seenLabel.setFillColor(sf::Color(20, 65, 245));
    /*seenLabel.setPosition(
        SeenBoxPos +
        glm::vec2(InfoPadding - 12.f, static_cast<float>(seenTxtr->getSize().y) * 0.5f));*/

    ownedTxtr = TextureManager::load(joinPath(ImgPath, "Peopledex/ownedBox.png"));
    ownedBox.setTexture(*ownedTxtr, true);
    // ownedBox.setPosition(OwnBoxPos);
    // ownedLabel.setFont(core::Properties::MenuFont());
    ownedLabel.setCharacterSize(26);
    ownedLabel.setFillColor(sf::Color::Red);
    /*ownedLabel.setPosition(
        OwnBoxPos + sf::Vector2(InfoPadding, static_cast<float>(ownedTxtr->getSize().y) * 0.5f));*/

    thumbnail.setPosition({InfoLeft + InfoWidth * 0.5f, InfoTop + 20.f});
    // nameLabel.setFont(core::Properties::MenuFont());
    nameLabel.setCharacterSize(26);
    nameLabel.setFillColor(sf::Color(0, 20, 65));
    nameLabel.setPosition(InfoLeft + 8.f, InfoTop + 245.f);
    // descLabel.setFont(core::Properties::MenuFont());
    descLabel.setCharacterSize(18);
    descLabel.setFillColor(sf::Color::Black);
    descLabel.setPosition(InfoLeft + 3.f, InfoTop + 275.f);
    // locationLabel.setFont(core::Properties::MenuFont());
    locationLabel.setCharacterSize(22);
    locationLabel.setFillColor(sf::Color(0, 65, 20));
    locationLabel.setPosition(130.f, 442.f);

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

    upTxtr = TextureManager::load(joinPath(ImgPath, "Peopledex/upArrow.png"));
    upArrow.setTexture(*upTxtr, true);
    upArrow.setOrigin(0.f, static_cast<float>(upTxtr->getSize().y));
    // upArrow.setPosition(MenuPos + glm::vec2(mw * 0.5f, -ArrowPadding));

    downTxtr = TextureManager::load(joinPath(ImgPath, "Peopledex/downArrow.png"));
    downArrow.setTexture(*downTxtr, true);
    downArrow.setPosition(
        MenuPos.x + mw * 0.5f,
        static_cast<float>(core::Properties::WindowHeight() - downTxtr->getSize().y - 5));
}

const char* Peopledex::name() const { return "Peopledex"; }

void Peopledex::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    /*engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));*/
}

void Peopledex::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    // engine.renderSystem().cameras().popCamera();
}

void Peopledex::update(bl::engine::Engine&, float, float) {}

bool Peopledex::observe(const bl::input::Actor&, unsigned int ctrl, bl::input::DispatchType,
                        bool fromEvent) {
    if (ctrl == core::input::Control::Back && fromEvent) { systems.engine().popState(); }
    else { menuDriver.sendControl(ctrl, fromEvent); }
    return true;
}

// void Peopledex::render(bl::engine::Engine& engine, float) {
//     engine.window().clear();
//
//     engine.window().draw(background);
//     engine.window().draw(upArrow);
//     engine.window().draw(downArrow);
//     menu.render(engine.window());
//
//     engine.window().draw(thumbnail);
//     engine.window().draw(nameLabel);
//     engine.window().draw(descLabel);
//     engine.window().draw(locationLabel);
//
//     engine.window().draw(seenBox);
//     engine.window().draw(seenLabel);
//     engine.window().draw(ownedBox);
//     engine.window().draw(ownedLabel);
//
//     engine.window().display();
// }

void Peopledex::onHighlight(core::pplmn::Id ppl) {
    if (systems.player().state().peopledex.getIntelLevel(ppl) != core::player::Peopledex::NoIntel) {
        nameLabel.setString(core::pplmn::Peoplemon::name(ppl));
        descLabel.setString(core::pplmn::Peoplemon::description(ppl));
        thumbTxtr = TextureManager::load(core::pplmn::Peoplemon::thumbnailImage(ppl));
    }
    else {
        nameLabel.setString("???");
        descLabel.setString("");
        thumbTxtr =
            TextureManager::load(core::pplmn::Peoplemon::thumbnailImage(core::pplmn::Id::Unknown));
    }
    thumbnail.setScale(200.f / static_cast<float>(thumbTxtr->getSize().x),
                       200.f / static_cast<float>(thumbTxtr->getSize().y));
    thumbnail.setTexture(*thumbTxtr, true);
    thumbnail.setOrigin(100.f, 0.f);

    // bl::interface::wordWrap(descLabel, InfoWidth - 20.f);
    locationLabel.setString(systems.player().state().peopledex.getFirstSeenLocation(ppl));
    seenLabel.setString(std::to_string(systems.player().state().peopledex.getSeen(ppl)));
    seenLabel.setOrigin(
        0.f, seenLabel.getGlobalBounds().height * 0.5f + seenLabel.getLocalBounds().top * 0.5f);
    ownedLabel.setString(std::to_string(systems.player().state().peopledex.getCaught(ppl)));
    ownedLabel.setOrigin(
        0.f, ownedLabel.getGlobalBounds().height * 0.5f + ownedLabel.getLocalBounds().top * 0.5f);

    if (ppl == firstId) { upArrow.setColor(HiddenColor); }
    else { upArrow.setColor(ShowingColor); }
    if (ppl == lastId) { downArrow.setColor(HiddenColor); }
    else { downArrow.setColor(ShowingColor); }
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
