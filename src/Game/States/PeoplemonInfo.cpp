#include <Game/States/PeoplemonInfo.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Peoplemon/Move.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/Menus/MoveInfoRow.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float FlashTime    = 0.35f;
constexpr float PageLabelX   = 612.f;
constexpr float ArrowPadding = 10.f;
constexpr float ThumbSize    = 270.f;
const glm::vec2 BasicPos(342.f, 78.f);
const glm::vec2 MovePos(329.f, 260.f);
} // namespace

bl::engine::State::Ptr PeoplemonInfo::create(core::system::Systems& s,
                                             const core::pplmn::OwnedPeoplemon& ppl) {
    return Ptr{new PeoplemonInfo(s, ppl)};
}

PeoplemonInfo::PeoplemonInfo(core::system::Systems& s, const core::pplmn::OwnedPeoplemon& ppl)
: State(s, bl::engine::StateMask::Menu)
, inputDebounce(0.f)
, flashDelay(0.f) {
    moveMenu.create(
        s.engine(), s.engine().renderer().getObserver(), bl::menu::NoSelector::create());

    const auto& font = core::Properties::MenuFont();
    const std::string ImgPath =
        bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(), "PplInfo");
    const auto loadImg = [&ImgPath, &s](const std::string& path) -> bl::rc::res::TextureRef {
        return s.engine().renderer().texturePool().getOrLoadTexture(
            bl::util::FileUtil::joinPath(ImgPath, path));
    };

    bgndTxtr = loadImg("background.png");
    background.create(s.engine(), bgndTxtr);

    leftTxtr = loadImg("leftArrow.png");
    leftArrow.create(s.engine(), leftTxtr);
    leftArrow.setParent(background);
    leftArrow.getTransform().setOrigin(leftTxtr->size().x, leftTxtr->size().y * 0.5f);
    leftArrow.getTransform().setPosition(0.f, 32.f);

    rightTxtr = loadImg("rightArrow.png");
    rightArrow.create(s.engine(), rightTxtr);
    rightArrow.getTransform().setOrigin(0.f, rightTxtr->size().y * 0.5f);
    rightArrow.getTransform().setPosition(0.f, 31.f);
    rightArrow.setParent(background);

    pageLabel.create(s.engine(), font, "", 30, sf::Color::Black);
    pageLabel.setParent(background);
    pageLabel.getTransform().setPosition(PageLabelX, 31.f);

    thumbTxtr = s.engine().renderer().texturePool().getOrLoadTexture(
        core::pplmn::Peoplemon::thumbnailImage(ppl.id()));
    thumbnail.create(s.engine(), thumbTxtr);
    thumbnail.getTransform().setPosition(30.f, 165.f);
    thumbnail.getTransform().setScale(ThumbSize / thumbTxtr->size());
    thumbnail.setParent(background);

    nameLabel.create(s.engine(), font, ppl.name(), 28, sf::Color(200, 40, 10));
    nameLabel.getTransform().setPosition(13.f, 75.f);
    nameLabel.setParent(background);

    idLabel.create(
        s.engine(), font, std::to_string(static_cast<int>(ppl.id())), 18, sf::Color(200, 200, 255));
    idLabel.getTransform().setPosition(57.f, 122.f);
    idLabel.setParent(background);

    levelLabel.create(
        s.engine(), font, std::to_string(ppl.currentLevel()), 22, sf::Color(190, 255, 220));
    levelLabel.getTransform().setPosition(254.f, 115.f);
    levelLabel.setParent(background);

    itemLabel.create(s.engine(),
                     font,
                     ppl.holdItem() != core::item::Id::None ?
                         core::item::Item::getName(ppl.holdItem()) :
                         "No hold item",
                     16,
                     sf::Color(200, 255, 230));
    itemLabel.getTransform().setPosition(79.f, 473.f);
    itemLabel.setParent(background);

    curXpLabel.create(
        s.engine(), font, std::to_string(ppl.currentXP()), 16, sf::Color(50, 200, 255));
    curXpLabel.getTransform().setPosition(101.f, 495.f);
    curXpLabel.setParent(background);

    nextXpLabel.create(
        s.engine(), font, std::to_string(ppl.nextLevelXP()), 16, sf::Color(20, 140, 220));
    nextXpLabel.getTransform().setPosition(101.f, 518.f);
    nextXpLabel.setParent(background);

    ailLabel.create(s.engine(),
                    font,
                    core::pplmn::Peoplemon::ailmentString(ppl.currentAilment()),
                    22,
                    ppl.currentAilment() == core::pplmn::Ailment::None ? sf::Color(40, 255, 120) :
                                                                         sf::Color(230, 50, 50));
    ailLabel.getTransform().setPosition(155.f, 563.f);
    ailLabel.setParent(background);

    basicsTxtr = loadImg("basicsBox.png");
    basicsBox.create(s.engine(), basicsTxtr);
    basicsBox.getTransform().setPosition(BasicPos);
    basicsBox.setParent(background);

    speciesLabel.create(
        s.engine(), font, core::pplmn::Peoplemon::name(ppl.id()), 30, sf::Color(165, 40, 20));
    speciesLabel.getTransform().setPosition(26.f, 83.f);
    speciesLabel.getTransform().setOrigin(0.f, speciesLabel.getLocalBounds().height * 0.5f);
    speciesLabel.setParent(basicsBox);

    typeLabel.create(s.engine(),
                     font,
                     core::pplmn::TypeUtil::getTypeString(ppl.type()),
                     20,
                     sf::Color(30, 170, 85));
    typeLabel.getTransform().setOrigin(0.f, typeLabel.getLocalBounds().height * 0.5f);
    typeLabel.getTransform().setPosition(speciesLabel.getLocalBounds().width + 36.f, 85.f);

    const core::pplmn::Stats stats = ppl.currentStats();
    const sf::Color statColor(30, 165, 80);

    hpLabel.create(s.engine(),
                   font,
                   std::to_string(ppl.currentHp()) + " / " + std::to_string(stats.hp),
                   18,
                   statColor);
    hpLabel.getTransform().setPosition(90.f, 135.f);
    hpLabel.setParent(basicsBox);

    atkLabel.create(s.engine(), font, std::to_string(stats.atk), 18, statColor);
    atkLabel.getTransform().setPosition(102.f, 189.f);
    atkLabel.setParent(basicsBox);

    defLabel.create(s.engine(), font, std::to_string(stats.def), 18, statColor);
    defLabel.getTransform().setPosition(102.f, 243.f);
    defLabel.setParent(basicsBox);

    spAtkLabel.create(s.engine(), font, std::to_string(stats.spatk), 18, statColor);
    spAtkLabel.getTransform().setPosition(296.f, 191.f);
    spAtkLabel.setParent(basicsBox);

    spDefLabel.create(s.engine(), font, std::to_string(stats.spdef), 18, statColor);
    spDefLabel.getTransform().setPosition(298.f, 245.f);
    spDefLabel.setParent(basicsBox);

    spdLabel.create(s.engine(), font, std::to_string(stats.spd), 18, statColor);
    spdLabel.getTransform().setPosition(298.f, 296.f);
    spdLabel.setParent(basicsBox);

    descLabel.create(s.engine(),
                     font,
                     core::pplmn::Peoplemon::description(ppl.id()),
                     14,
                     sf::Color(230, 230, 230));
    descLabel.getTransform().setPosition(30.f, 350.f);
    descLabel.wordWrap(397.f);
    descLabel.setParent(basicsBox);

    abilityLabel.create(s.engine(),
                        font,
                        core::pplmn::Peoplemon::abilityName(ppl.ability()),
                        19,
                        sf::Color(40, 200, 100));
    abilityLabel.getTransform().setPosition(30.f, 437.f);
    abilityLabel.setParent(basicsBox);

    abilityDescLabel.create(s.engine(),
                            font,
                            core::pplmn::Peoplemon::abilityDescription(ppl.ability()),
                            15,
                            sf::Color(220, 220, 220));
    abilityDescLabel.getTransform().setPosition(45.f,
                                                abilityLabel.getTransform().getLocalPosition().y +
                                                    abilityLabel.getLocalBounds().height + 4.f);
    abilityDescLabel.setParent(basicsBox);

    moveTxtr = loadImg("moveBox.png");
    moveBox.create(s.engine(), moveTxtr);
    moveBox.getTransform().setPosition(MovePos);
    moveBox.setParent(background);

    movePwrLabel.create(s.engine(), font, "999", 22, sf::Color(152, 81, 81));
    movePwrLabel.getTransform().setPosition(228.f, 54.f);
    movePwrLabel.setParent(moveBox);

    moveAccLabel.create(s.engine(), font, "999", 22, sf::Color(58, 161, 151));
    moveAccLabel.getTransform().setPosition(228.f, 93.f);
    moveAccLabel.setParent(moveBox);

    moveTypeLabel.create(s.engine(), font, "999", 22, sf::Color(76, 162, 50));
    moveTypeLabel.getTransform().setPosition(228.f, 137.f);
    moveTypeLabel.setParent(moveBox);

    moveDescLabel.create(s.engine(), font, "", 16, sf::Color(220, 220, 220));
    moveDescLabel.getTransform().setPosition(46.f, 196.f);
    moveDescLabel.wordWrap(376.f);
    moveDescLabel.setParent(moveBox);

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
    else { BL_LOG_CRITICAL << "Peoplemon has no moves!"; }
    moveMenu.setPosition({565.f - moveMenu.getBounds().width * 0.5f, 60.f});
}

const char* PeoplemonInfo::name() const { return "PeoplemonInfo"; }

void PeoplemonInfo::activate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().addListener(*this);

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    leftArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    rightArrow.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    pageLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    thumbnail.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    nameLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    idLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    levelLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    itemLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    curXpLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    nextXpLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    ailLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    basicsBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    speciesLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    typeLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    hpLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    atkLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    defLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    spdLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    spAtkLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    spDefLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    descLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    abilityLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    abilityDescLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    moveBox.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    movePwrLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    moveAccLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    moveTypeLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    moveDescLabel.addToScene(overlay, bl::rc::UpdateSpeed::Static);
    moveMenu.addToOverlay(background.entity());

    setPage(ActivePage::Basics);
    inputDebounce = 0.f;
    flashDelay    = 0.f;
}

void PeoplemonInfo::deactivate(bl::engine::Engine& engine) {
    systems.engine().inputSystem().getActor().removeListener(*this);
    engine.renderer().getObserver().popScene();
}

void PeoplemonInfo::update(bl::engine::Engine&, float dt, float) {
    inputDebounce += dt;

    if (flashDelay >= 0.f) {
        flashDelay += dt;
        if (flashDelay >= FlashTime) {
            flashDelay = -1.f;
            setPage(activePage);
        }
    }
}

bool PeoplemonInfo::observe(const bl::input::Actor&, unsigned int cmd, bl::input::DispatchType,
                            bool eventTriggered) {
    if (inputDebounce < 0.4f && !eventTriggered) return true;
    inputDebounce = 0.f;

    switch (cmd) {
    case core::input::Control::MoveLeft:
        if (activePage == ActivePage::Moves) { setPage(ActivePage::Basics); }
        break;
    case core::input::Control::MoveRight:
        if (activePage == ActivePage::Basics) { setPage(ActivePage::Moves); }
        break;
    case core::input::Control::MoveDown:
        if (activePage == ActivePage::Moves) {
            moveMenu.processEvent(
                bl::menu::Event(bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Bottom)));
        }
        break;
    case core::input::Control::MoveUp:
        if (activePage == ActivePage::Moves) {
            moveMenu.processEvent(
                bl::menu::Event(bl::menu::Event::MoveEvent(bl::menu::Item::AttachPoint::Top)));
        }
        break;
    case core::input::Control::Back:
        systems.engine().popState();
        break;
    default:
        break;
    }

    return true;
}

void PeoplemonInfo::highlightMove(core::pplmn::MoveId move) {
    using core::pplmn::Move;
    const int acc = Move::accuracy(move);

    movePwrLabel.getSection().setString(std::to_string(Move::damage(move)));
    moveAccLabel.getSection().setString(acc > 0 ? std::to_string(acc) : "Always hits");
    moveTypeLabel.getSection().setString(core::pplmn::TypeUtil::getTypeString(Move::type(move)));
    moveDescLabel.getSection().setString(Move::description(move));
}

void PeoplemonInfo::setPage(ActivePage page) {
    activePage = page;

    float offset = 0.f;
    switch (page) {
    case ActivePage::Basics:
        pageLabel.getSection().setString("Basic Information");
        pageLabel.getSection().setFillColor(sf::Color(30, 100, 255));
        basicsBox.setHidden(false);
        moveBox.setHidden(true);
        moveMenu.setHidden(true);

        offset = -(rightTxtr->size().x * 0.5f);
        if (flashDelay < 0.f) {
            rightArrow.flash(FlashTime, FlashTime);
            leftArrow.stopFlashing();
        }
        rightArrow.setHidden(false);
        leftArrow.setHidden(true);
        break;
    case ActivePage::Moves:
        pageLabel.getSection().setString("Known Moves");
        pageLabel.getSection().setFillColor(sf::Color(255, 50, 50));
        basicsBox.setHidden(true);
        moveBox.setHidden(false);
        moveMenu.setHidden(false);

        if (flashDelay < 0.f) {
            leftArrow.flash(FlashTime, FlashTime);
            rightArrow.stopFlashing();
        }
        leftArrow.setHidden(false);
        rightArrow.setHidden(true);
        break;
    }

    pageLabel.getTransform().setPosition(PageLabelX + offset,
                                         pageLabel.getTransform().getLocalPosition().y);
    pageLabel.getTransform().setOrigin(pageLabel.getGlobalSize() * 0.5f);
    leftArrow.getTransform().setPosition(pageLabel.getTransform().getLocalPosition().x -
                                             pageLabel.getTransform().getOrigin().x - ArrowPadding,
                                         leftArrow.getTransform().getLocalPosition().y);
    rightArrow.getTransform().setPosition(pageLabel.getTransform().getLocalPosition().x +
                                              pageLabel.getTransform().getOrigin().x + ArrowPadding,
                                          rightArrow.getTransform().getLocalPosition().y);
}

} // namespace state
} // namespace game
