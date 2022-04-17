#include <Core/Battles/View/StatBoxes.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
constexpr float BarRate = 0.45f; // 45% of total width per second
const sf::Vector2f BarSize(96.f, 6.f);

const sf::Vector2f XpBarPos(52.f, 88.f);
const sf::Vector2f XpBarSize(192.f, 4.f);

constexpr float GreenThresh  = 0.55f;
constexpr float YellowThresh = 0.25f;

const sf::Vector2f OpBoxPos(0.f, 0.f);
const sf::Vector2f OpBarPos(100.f, 50.f);

const sf::Vector2f LpBoxPos(540.f, 357.f);
const sf::Vector2f LpBarPos(148.f, 54.f);

constexpr float NameSize  = 18.f;
constexpr float LevelSize = 15.f;
const sf::Color LevelColor(20.f, 20.f, 140.f);

std::string makeHpStr(unsigned int cur, unsigned int max) {
    std::stringstream ss;
    ss << cur << "/" << max;
    return ss.str();
}

} // namespace

StatBoxes::StatBoxes()
: localPlayer(nullptr)
, opponent(nullptr)
, opHpBarTarget(0.f)
, lpHpBarTarget(0.f)
, lpXpBarTarget(0.f) {
    auto& textures             = bl::engine::Resources::textures();
    constexpr auto join        = bl::util::FileUtil::joinPath;
    const std::string& ImgPath = Properties::ImagePath();

    annoyTxtr  = textures.load(join(ImgPath, "Battle/annoy.png")).data;
    frustTxtr  = textures.load(join(ImgPath, "Battle/frustrate.png")).data;
    stickyTxtr = textures.load(join(ImgPath, "Battle/sticky.png")).data;
    slpTxtr    = textures.load(join(ImgPath, "Battle/sleep.png")).data;
    frzTxtr    = textures.load(join(ImgPath, "Battle/frozen.png")).data;
    grdTxtr    = textures.load(join(ImgPath, "Battle/guarded.png")).data;

    opBoxTxtr = textures.load(join(ImgPath, "Battle/opBox.png")).data;
    opBox.setTexture(*opBoxTxtr, true);
    opBox.setPosition(OpBoxPos);
    opHpBar.setPosition(OpBoxPos + OpBarPos);

    lpBoxTxtr = textures.load(join(ImgPath, "Battle/pBox.png")).data;
    lpBox.setTexture(*lpBoxTxtr, true);
    lpBox.setPosition(LpBoxPos);
    lpHpBar.setPosition(LpBoxPos + LpBarPos);
    lpXpBar.setPosition(LpBoxPos + XpBarPos);

    opAil.setPosition(OpBoxPos + sf::Vector2f(10.f, 46.f));
    opName.setPosition(OpBoxPos + sf::Vector2f(5.f, 18.f));
    opLevel.setPosition(OpBoxPos + sf::Vector2f(168.f, 21.f));
    opName.setFont(Properties::MenuFont());
    opName.setFillColor(sf::Color::Black);
    opName.setCharacterSize(NameSize);
    opName.setStyle(sf::Text::Bold);
    opLevel.setFont(Properties::MenuFont());
    opLevel.setFillColor(LevelColor);
    opLevel.setCharacterSize(LevelSize);
    opLevel.setStyle(sf::Text::Bold);

    lpAil.setPosition(LpBoxPos + sf::Vector2f(33.f, 61.f));
    lpName.setPosition(LpBoxPos + sf::Vector2f(41.f, 22.f));
    lpLevel.setPosition(LpBoxPos + sf::Vector2f(218.f, 24.f));
    lpName.setFont(Properties::MenuFont());
    lpName.setFillColor(sf::Color::Black);
    lpName.setCharacterSize(NameSize);
    lpName.setStyle(sf::Text::Bold);
    lpLevel.setFont(Properties::MenuFont());
    lpLevel.setFillColor(LevelColor);
    lpLevel.setCharacterSize(LevelSize);
    lpLevel.setStyle(sf::Text::Bold);
    lpHp.setFont(Properties::MenuFont());
    lpHp.setPosition(LpBoxPos + sf::Vector2f(131.f, 68.f));
    lpHp.setCharacterSize(14.f);
    lpHp.setFillColor(sf::Color::Black);
}

void StatBoxes::setOpponent(pplmn::BattlePeoplemon* ppl) {
    opponent = ppl;
    sync();
}

void StatBoxes::setPlayer(pplmn::BattlePeoplemon* ppl) {
    localPlayer = ppl;
    sync();
}

void StatBoxes::sync() {
    // TODO - add immediate sync for switches?

    if (localPlayer) {
        lpName.setString(localPlayer->base().name());
        lpLevel.setString(std::to_string(localPlayer->base().currentLevel()));
        lpHp.setString(makeHpStr(localPlayer->base().currentHp(), localPlayer->currentStats().hp));
        lpAil.setTexture(ailmentTexture(localPlayer->base().currentAilment()), true);
        lpHpBarTarget = (static_cast<float>(localPlayer->base().currentHp()) /
                         static_cast<float>(localPlayer->currentStats().hp)) *
                        BarSize.x;
        lpXpBarTarget = (static_cast<float>(localPlayer->base().currentXP()) /
                         static_cast<float>(localPlayer->base().nextLevelXP())) *
                        XpBarSize.x;
        BL_LOG_INFO << lpHpBarTarget;
    }
    else {
        BL_LOG_WARN << "sync() called with null player peoplemon";
        lpHpBarTarget = 0.f;
        lpXpBarTarget = 0.f;
        lpName.setString("<ERROR>");
    }

    if (opponent) {
        opName.setString(opponent->base().name());
        opLevel.setString(std::to_string(opponent->base().currentLevel()));
        opAil.setTexture(ailmentTexture(opponent->base().currentAilment()), true);
        opHpBarTarget = (static_cast<float>(opponent->base().currentHp()) /
                         static_cast<float>(opponent->currentStats().hp)) *
                        BarSize.x;
    }
    else {
        BL_LOG_WARN << "sync() called with null opponent peoplemon";
        opHpBarTarget = 0.f;
        opName.setString("<ERROR>");
    }
}

void StatBoxes::update(float dt) {
    const float change = dt * BarRate * BarSize.x;

    // opponent bar size
    if (opHpBar.getSize().x < opHpBarTarget) {
        const float nw = std::min(opHpBarTarget, opHpBar.getSize().x + change);
        opHpBar.setSize({nw, BarSize.y});
    }
    else if (opHpBar.getSize().y > opHpBarTarget) {
        const float nw = std::max(opHpBarTarget, opHpBar.getSize().x - change);
        opHpBar.setSize({nw, BarSize.y});
    }

    // player bar size
    if (lpHpBar.getSize().x < lpHpBarTarget) {
        const float nw = std::min(lpHpBarTarget, lpHpBar.getSize().x + change);
        lpHpBar.setSize({nw, BarSize.y});
    }
    else if (lpHpBar.getSize().y > lpHpBarTarget) {
        const float nw = std::max(lpHpBarTarget, lpHpBar.getSize().x - change);
        lpHpBar.setSize({nw, BarSize.y});
    }

    // opponent bar color
    const float opHp = opHpBar.getSize().x / BarSize.x;
    if (opHp >= GreenThresh) { opHpBar.setFillColor(sf::Color::Green); }
    else if (opHp >= YellowThresh) {
        opHpBar.setFillColor(sf::Color::Yellow);
    }
    else {
        opHpBar.setFillColor(sf::Color::Red);
    }

    // player bar color
    const float lphp = lpHpBar.getSize().x / BarSize.x;
    if (lphp >= GreenThresh) { lpHpBar.setFillColor(sf::Color::Green); }
    else if (lphp >= YellowThresh) {
        lpHpBar.setFillColor(sf::Color::Yellow);
    }
    else {
        lpHpBar.setFillColor(sf::Color::Red);
    }

    // xp bar size
    const float xpChange = dt * BarRate * XpBarSize.x;
    if (lpXpBar.getSize().x < lpXpBarTarget) {
        const float nw = std::min(lpXpBarTarget, lpXpBar.getSize().x + xpChange);
        lpXpBar.setSize({nw, XpBarSize.y});
    }
    else if (lpXpBar.getSize().x > lpXpBarTarget) {
        // no transition down
        lpXpBar.setSize({lpXpBarTarget, XpBarSize.y});
    }
}

bool StatBoxes::synced() const {
    return opHpBar.getSize().x == opHpBarTarget && lpHpBar.getSize().x == lpHpBarTarget &&
           lpXpBar.getSize().x == lpXpBarTarget;
}

void StatBoxes::render(sf::RenderTarget& target) const {
    target.draw(opHpBar);
    target.draw(opBox);
    target.draw(opName);
    target.draw(opLevel);
    target.draw(opAil);

    target.draw(lpHpBar);
    target.draw(lpXpBar);
    target.draw(lpBox);
    target.draw(lpName);
    target.draw(lpHp);
    target.draw(lpLevel);
    target.draw(lpAil);
}

const sf::Texture& StatBoxes::ailmentTexture(pplmn::Ailment ail) const {
    using Ailment = pplmn::Ailment;

    switch (ail) {
    case Ailment::Annoyed:
        return *annoyTxtr;
    case Ailment::Frozen:
        return *frzTxtr;
    case Ailment::Frustrated:
        return *frustTxtr;
    case Ailment::Guarded:
        return *grdTxtr;
    case Ailment::Sleep:
        return *slpTxtr;
    case Ailment::Sticky:
        return *stickyTxtr;
    case Ailment::None:
    default:
        return blank;
    }
}

} // namespace view
} // namespace battle
} // namespace core
