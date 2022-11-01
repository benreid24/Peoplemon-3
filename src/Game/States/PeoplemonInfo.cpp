#include <Game/States/PeoplemonInfo.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Game/Menus/MoveInfoRow.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float FlashTime    = 0.35f;
constexpr float ArrowPadding = 10.f;
constexpr float ThumbSize    = 270.f;
const sf::Vector2f BasicPos(342.f, 78.f);
const sf::Vector2f MovePos(329.f, 260.f);
} // namespace

bl::engine::State::Ptr PeoplemonInfo::create(core::system::Systems& s,
                                             const core::pplmn::OwnedPeoplemon& ppl) {
    return Ptr{new PeoplemonInfo(s, ppl)};
}

PeoplemonInfo::PeoplemonInfo(core::system::Systems& s, const core::pplmn::OwnedPeoplemon& ppl)
: State(s)
, ppl(ppl)
, leftFlasher(leftArrow, FlashTime, FlashTime)
, rightFlasher(rightArrow, FlashTime, FlashTime)
, moveMenu(bl::menu::NoSelector::create()) {
    const sf::Font& font = core::Properties::MenuFont();
    const std::string ImgPath =
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "PplInfo");
    const auto loadImg =
        [&ImgPath](const std::string& path) -> bl::resource::Resource<sf::Texture>::Ref {
        return bl::engine::Resources::textures()
            .load(bl::util::FileUtil::joinPath(ImgPath, path))
            .data;
    };

    bgndTxtr = loadImg("background.png");
    background.setTexture(*bgndTxtr, true);

    leftTxtr = loadImg("leftArrow.png");
    leftArrow.setTexture(*leftTxtr, true);
    leftArrow.setOrigin(static_cast<float>(leftTxtr->getSize().x),
                        static_cast<float>(leftTxtr->getSize().y) * 0.5f);
    leftArrow.setPosition(0.f, 32.f);

    rightTxtr = loadImg("rightArrow.png");
    rightArrow.setTexture(*rightTxtr, true);
    rightArrow.setOrigin(0.f, static_cast<float>(rightTxtr->getSize().y) * 0.5f);
    rightArrow.setPosition(0.f, 31.f);

    pageLabel.setFont(font);
    pageLabel.setCharacterSize(34);
    pageLabel.setPosition(612.f, 31.f);
    setPage(ActivePage::Basics);

    thumbTxtr = bl::engine::Resources::textures()
                    .load(core::pplmn::Peoplemon::thumbnailImage(ppl.id()))
                    .data;
    thumbnail.setTexture(*thumbTxtr, true);
    thumbnail.setPosition(30.f, 165.f);
    thumbnail.setScale(ThumbSize / static_cast<float>(thumbTxtr->getSize().x),
                       ThumbSize / static_cast<float>(thumbTxtr->getSize().y));

    nameLabel.setFont(font);
    nameLabel.setCharacterSize(28);
    nameLabel.setPosition(13.f, 75.f);
    nameLabel.setFillColor(sf::Color(200, 40, 10));
    nameLabel.setString(ppl.name());

    idLabel.setFont(font);
    idLabel.setCharacterSize(18);
    idLabel.setFillColor(sf::Color(200, 200, 255));
    idLabel.setPosition(57.f, 122.f);
    idLabel.setString(std::to_string(static_cast<int>(ppl.id())));

    levelLabel.setFont(font);
    levelLabel.setCharacterSize(22);
    levelLabel.setFillColor(sf::Color(190, 255, 220));
    levelLabel.setPosition(254.f, 115.f);
    levelLabel.setString(std::to_string(ppl.currentLevel()));

    itemLabel.setFont(font);
    itemLabel.setCharacterSize(16);
    itemLabel.setPosition(79.f, 473.f);
    itemLabel.setFillColor(sf::Color(200, 255, 230));
    itemLabel.setString(ppl.holdItem() != core::item::Id::None ?
                            core::item::Item::getName(ppl.holdItem()) :
                            "No hold item");

    curXpLabel.setFont(font);
    curXpLabel.setCharacterSize(16);
    curXpLabel.setPosition(101.f, 495.f);
    curXpLabel.setFillColor(sf::Color(50, 200, 255));
    curXpLabel.setString(std::to_string(ppl.currentXP()));
    nextXpLabel.setFont(font);
    nextXpLabel.setCharacterSize(16);
    nextXpLabel.setPosition(101.f, 518.f);
    nextXpLabel.setFillColor(sf::Color(20, 140, 220));
    nextXpLabel.setString(std::to_string(ppl.nextLevelXP()));

    ailLabel.setFont(font);
    ailLabel.setFillColor(ppl.currentAilment() == core::pplmn::Ailment::None ?
                              sf::Color(40, 255, 120) :
                              sf::Color(230, 50, 50));
    ailLabel.setCharacterSize(22);
    ailLabel.setPosition(155.f, 563.f);
    ailLabel.setString(core::pplmn::Peoplemon::ailmentString(ppl.currentAilment()));

    basicsTxtr = loadImg("basicsBox.png");
    basicsBox.setTexture(*basicsTxtr, true);
    basicsBox.setPosition(BasicPos);

    speciesLabel.setFont(font);
    speciesLabel.setCharacterSize(30);
    speciesLabel.setPosition(BasicPos + sf::Vector2f(26.f, 83.f));
    speciesLabel.setFillColor(sf::Color(165, 40, 20));
    speciesLabel.setString(core::pplmn::Peoplemon::name(ppl.id()));
    speciesLabel.setOrigin(0.f, speciesLabel.getGlobalBounds().height * 0.5f);

    typeLabel.setFont(font);
    typeLabel.setCharacterSize(20);
    typeLabel.setFillColor(sf::Color(30, 170, 85));
    typeLabel.setString(core::pplmn::TypeUtil::getTypeString(ppl.type()));
    typeLabel.setOrigin(0.f, typeLabel.getGlobalBounds().height * 0.5f);
    typeLabel.setPosition(BasicPos +
                          sf::Vector2f(speciesLabel.getGlobalBounds().width + 36.f, 85.f));

    const core::pplmn::Stats stats = ppl.currentStats();
    hpLabel.setString(std::to_string(ppl.currentHp()) + " / " + std::to_string(stats.hp));
    hpLabel.setPosition(90.f, 135.f);
    atkLabel.setString(std::to_string(stats.atk));
    atkLabel.setPosition(102.f, 189.f);
    defLabel.setString(std::to_string(stats.def));
    defLabel.setPosition(102.f, 243.f);
    spAtkLabel.setString(std::to_string(stats.spatk));
    spAtkLabel.setPosition(296.f, 191.f);
    spDefLabel.setString(std::to_string(stats.spdef));
    spDefLabel.setPosition(298.f, 245.f);
    spdLabel.setString(std::to_string(stats.spd));
    spdLabel.setPosition(298.f, 296.f);

    const std::array<sf::Text*, 6> stps{
        {&hpLabel, &atkLabel, &defLabel, &spdLabel, &spAtkLabel, &spDefLabel}};
    for (sf::Text* stat : stps) {
        stat->setFont(font);
        stat->setCharacterSize(18);
        stat->setFillColor(sf::Color(30, 165, 80));
        stat->move(BasicPos);
    }

    descLabel.setFont(font);
    descLabel.setCharacterSize(14);
    descLabel.setFillColor(sf::Color(230, 230, 230));
    descLabel.setString(core::pplmn::Peoplemon::description(ppl.id()));
    descLabel.setPosition(BasicPos + sf::Vector2f(30.f, 350.f));
    bl::interface::wordWrap(descLabel, 397.f);

    abilityLabel.setFont(font);
    abilityLabel.setCharacterSize(19);
    abilityLabel.setFillColor(sf::Color(40, 200, 100));
    abilityLabel.setPosition(BasicPos + sf::Vector2f(30.f, 437.f));
    abilityLabel.setString(core::pplmn::Peoplemon::abilityName(ppl.ability()));

    abilityDescLabel.setFont(font);
    abilityDescLabel.setCharacterSize(15);
    abilityDescLabel.setFillColor(sf::Color(220, 220, 220));
    abilityDescLabel.setPosition(BasicPos.x + 45.f,
                                 abilityLabel.getPosition().y +
                                     abilityLabel.getGlobalBounds().height + 4.f);
    abilityDescLabel.setString(core::pplmn::Peoplemon::abilityDescription(ppl.ability()));
    bl::interface::wordWrap(abilityDescLabel, 380.f);

    moveTxtr = loadImg("moveBox.png");
    moveBox.setTexture(*moveTxtr, true);
    moveBox.setPosition(MovePos);

    movePwrLabel.setFont(font);
    movePwrLabel.setCharacterSize(22);
    movePwrLabel.setPosition(MovePos + sf::Vector2f(228.f, 54.f));
    movePwrLabel.setFillColor(sf::Color(152, 81, 81));
    moveAccLabel.setFont(font);
    moveAccLabel.setCharacterSize(22);
    moveAccLabel.setPosition(MovePos + sf::Vector2f(228.f, 93.f));
    moveAccLabel.setFillColor(sf::Color(58, 161, 151));
    moveTypeLabel.setFont(font);
    moveTypeLabel.setPosition(MovePos + sf::Vector2f(228.f, 137.f));
    moveTypeLabel.setCharacterSize(22);
    moveTypeLabel.setFillColor(sf::Color(76, 162, 50));
    moveDescLabel.setFont(font);
    moveDescLabel.setCharacterSize(16);
    moveDescLabel.setFillColor(sf::Color(220, 220, 220));
    moveDescLabel.setPosition(MovePos + sf::Vector2f(50.f, 196.f));

    std::vector<menu::MoveInfoRow::Ptr> items;
    items.reserve(4);
    for (int i = 0; i < 4; ++i) {
        const core::pplmn::MoveId move = ppl.knownMoves()[i].id;
        if (move != core::pplmn::MoveId::Unknown) {
            items.emplace_back(menu::MoveInfoRow::create(move));
            items.back()
                ->getSignal(bl::menu::Item::Selected)
                .willAlwaysCall(std::bind(&PeoplemonInfo::highlightMove, this, move));
        }
    }
    if (!items.empty()) {
        moveMenu.setPadding({0.f, 4.f});
        moveMenu.setRootItem(items.front());
        bl::menu::Item* prev = items.front().get();
        for (unsigned int i = 1; i < items.size(); ++i) {
            moveMenu.addItem(items[i], prev, bl::menu::Item::Bottom);
            prev = items[i].get();
        }
        moveMenu.setSelectedItem(items.front().get());
    }
    else {
        BL_LOG_CRITICAL << "Peoplemon has no moves!";
    }
    moveMenu.setPosition({565.f - moveMenu.getBounds().width * 0.5f, 60.f});
}

const char* PeoplemonInfo::name() const { return "PeoplemonInfo"; }

void PeoplemonInfo::activate(bl::engine::Engine& engine) {
    systems.player().inputSystem().addListener(*this);

    engine.renderSystem().cameras().pushCamera(
        bl::render::camera::StaticCamera::create(core::Properties::WindowSize()));
}

void PeoplemonInfo::deactivate(bl::engine::Engine& engine) {
    systems.player().inputSystem().removeListener(*this);
    engine.renderSystem().cameras().popCamera();
}

void PeoplemonInfo::update(bl::engine::Engine&, float dt) {
    systems.player().inputSystem().update();
    leftFlasher.update(dt);
    rightFlasher.update(dt);
}

void PeoplemonInfo::render(bl::engine::Engine& engine, float lag) {
    auto& window = engine.window();
    window.clear();

    window.draw(background);
    window.draw(pageLabel);
    window.draw(idLabel);
    window.draw(nameLabel);
    window.draw(levelLabel);
    window.draw(thumbnail);
    window.draw(itemLabel);
    window.draw(curXpLabel);
    window.draw(nextXpLabel);
    window.draw(ailLabel);

    switch (activePage) {
    case ActivePage::Basics:
        rightFlasher.render(window, {}, lag);
        window.draw(basicsBox);
        window.draw(speciesLabel);
        window.draw(typeLabel);
        window.draw(hpLabel);
        window.draw(atkLabel);
        window.draw(defLabel);
        window.draw(spdLabel);
        window.draw(spAtkLabel);
        window.draw(spDefLabel);
        window.draw(descLabel);
        window.draw(abilityLabel);
        window.draw(abilityDescLabel);
        break;

    case ActivePage::Moves:
        leftFlasher.render(window, {}, lag);
        moveMenu.render(window);
        window.draw(moveBox);
        window.draw(movePwrLabel);
        window.draw(moveAccLabel);
        window.draw(moveDescLabel);
        window.draw(moveTypeLabel);
        break;
    }

    window.display();
}

void PeoplemonInfo::process(core::component::Command cmd) {
    if (inputTimer.getElapsedTime().asSeconds() < 0.4f) return;
    inputTimer.restart();

    switch (cmd) {
    case core::component::Command::MoveLeft:
        if (activePage == ActivePage::Moves) { setPage(ActivePage::Basics); }
        break;
    case core::component::Command::MoveRight:
        if (activePage == ActivePage::Basics) { setPage(ActivePage::Moves); }
        break;
    case core::component::Command::MoveDown:
        if (activePage == ActivePage::Moves) {
            moveMenu.processEvent(
                bl::menu::Event(bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Bottom)));
        }
        break;
    case core::component::Command::MoveUp:
        if (activePage == ActivePage::Moves) {
            moveMenu.processEvent(
                bl::menu::Event(bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Top)));
        }
        break;
    case core::component::Command::Back:
        systems.engine().popState();
        break;
    default:
        break;
    }
}

void PeoplemonInfo::highlightMove(core::pplmn::MoveId move) {
    using core::pplmn::Move;
    const int acc = Move::accuracy(move);

    movePwrLabel.setString(std::to_string(Move::damage(move)));
    moveAccLabel.setString(acc > 0 ? std::to_string(acc) : "Always hits");
    moveTypeLabel.setString(core::pplmn::TypeUtil::getTypeString(Move::type(move)));
    moveDescLabel.setString(Move::description(move));
    bl::interface::wordWrap(moveDescLabel, 380.f);
}

void PeoplemonInfo::setPage(ActivePage page) {
    activePage = page;
    switch (page) {
    case ActivePage::Basics:
        pageLabel.setString("Basic Information");
        pageLabel.setFillColor(sf::Color(30, 100, 255));
        break;
    case ActivePage::Moves:
        pageLabel.setString("Known Moves");
        pageLabel.setFillColor(sf::Color(255, 50, 50));
        break;
    }
    pageLabel.setOrigin(pageLabel.getGlobalBounds().width * 0.5f,
                        pageLabel.getGlobalBounds().height * 0.5f);
    leftArrow.setPosition(pageLabel.getPosition().x - pageLabel.getOrigin().x - ArrowPadding,
                          leftArrow.getPosition().y);
    rightArrow.setPosition(pageLabel.getPosition().x + pageLabel.getOrigin().x + ArrowPadding,
                           rightArrow.getPosition().y);
}

} // namespace state
} // namespace game
