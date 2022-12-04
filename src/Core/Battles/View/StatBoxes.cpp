#include <Core/Battles/View/StatBoxes.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace battle
{
namespace view
{
namespace
{
constexpr float SlideSpeed = 400.f;
constexpr float BarRate    = 0.45f; // 45% of total width per second
const sf::Vector2f BarSize(96.f, 6.f);

const sf::Vector2f XpBarPos(52.f, 88.f);
const sf::Vector2f XpBarSize(192.f, 4.f);

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
, pState(State::Hidden)
, oState(State::Hidden)
, opHpBarTarget(0.f)
, lpHpBarTarget(0.f)
, lpXpBarTarget(0.f) {
    constexpr auto join        = bl::util::FileUtil::joinPath;
    const std::string& ImgPath = Properties::ImagePath();

    opBoxTxtr = TextureManager::load(join(ImgPath, "Battle/opBox.png")).data;
    opBox.setTexture(*opBoxTxtr, true);
    opBox.setPosition(-opBox.getGlobalBounds().width, OpBoxPos.y);

    lpBoxTxtr = TextureManager::load(join(ImgPath, "Battle/pBox.png")).data;
    lpBox.setTexture(*lpBoxTxtr, true);
    lpBox.setPosition(Properties::WindowWidth() + lpBox.getGlobalBounds().width, LpBoxPos.y);
    lpXpBar.setFillColor(sf::Color::Blue);

    opName.setFont(Properties::MenuFont());
    opName.setFillColor(sf::Color::Black);
    opName.setCharacterSize(NameSize);
    opName.setStyle(sf::Text::Bold);
    opLevel.setFont(Properties::MenuFont());
    opLevel.setFillColor(LevelColor);
    opLevel.setCharacterSize(LevelSize);
    opLevel.setStyle(sf::Text::Bold);

    lpName.setFont(Properties::MenuFont());
    lpName.setFillColor(sf::Color::Black);
    lpName.setCharacterSize(NameSize);
    lpName.setStyle(sf::Text::Bold);
    lpLevel.setFont(Properties::MenuFont());
    lpLevel.setFillColor(LevelColor);
    lpLevel.setCharacterSize(LevelSize);
    lpLevel.setStyle(sf::Text::Bold);
    lpHp.setFont(Properties::MenuFont());
    lpHp.setCharacterSize(14.f);
    lpHp.setFillColor(sf::Color::Black);
}

void StatBoxes::setOpponent(pplmn::BattlePeoplemon* ppl) {
    opponent = ppl;
    opHpBar.setSize({0.f, BarSize.y});
    if (oState == State::Hidden) { oState = State::Sliding; }
    sync(false);
}

void StatBoxes::setPlayer(pplmn::BattlePeoplemon* ppl) {
    localPlayer = ppl;
    lpHpBar.setSize({0.f, BarSize.y});
    lpXpBar.setSize({0.f, XpBarSize.y});
    if (pState == State::Hidden) { pState = State::Sliding; }
    sync(true);
}

void StatBoxes::sync(bool fromSwitch) {
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
        if (fromSwitch) { lpXpBar.setSize({lpXpBarTarget, XpBarSize.y}); }
    }
    else {
        BL_LOG_WARN << "sync() called with null player peoplemon";
        lpHpBarTarget = 0.f;
        lpXpBarTarget = 0.f;
        lpName.setString("");
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
        opName.setString("");
    }
}

void StatBoxes::update(float dt) {
    const float change = dt * BarRate * BarSize.x;

    // opponent box slide
    if (oState == State::Sliding) {
        const float dx = SlideSpeed * dt;
        opBox.move(dx, 0.f);
        if (opBox.getPosition().x > OpBoxPos.x) {
            opBox.setPosition(OpBoxPos);
            oState = State::Showing;
        }
        opHpBar.setPosition(opBox.getPosition() + OpBarPos);
        opAil.setPosition(opBox.getPosition() + sf::Vector2f(10.f, 46.f));
        opName.setPosition(opBox.getPosition() + sf::Vector2f(5.f, 18.f));
        opLevel.setPosition(opBox.getPosition() + sf::Vector2f(168.f, 21.f));
    }
    else if (oState == State::Showing) {
        // opponent bar size
        if (opHpBar.getSize().x < opHpBarTarget) {
            const float nw = std::min(opHpBarTarget, opHpBar.getSize().x + change);
            opHpBar.setSize({nw, BarSize.y});
        }
        else if (opHpBar.getSize().x > opHpBarTarget) {
            const float nw = std::max(opHpBarTarget, opHpBar.getSize().x - change);
            opHpBar.setSize({nw, BarSize.y});
        }

        // opponent bar color
        const float opHp = opHpBar.getSize().x / BarSize.x;
        opHpBar.setFillColor(Properties::HPBarColor(opHp));
    }

    // player box slide
    if (pState == State::Sliding) {
        const float dx = SlideSpeed * dt;
        lpBox.move(-dx, 0.f);
        if (lpBox.getPosition().x < LpBoxPos.x) {
            lpBox.setPosition(LpBoxPos);
            pState = State::Showing;
        }
        lpAil.setPosition(lpBox.getPosition() + sf::Vector2f(33.f, 61.f));
        lpName.setPosition(lpBox.getPosition() + sf::Vector2f(41.f, 22.f));
        lpLevel.setPosition(lpBox.getPosition() + sf::Vector2f(218.f, 24.f));
        lpHp.setPosition(lpBox.getPosition() + sf::Vector2f(131.f, 66.f));
        lpHpBar.setPosition(lpBox.getPosition() + LpBarPos);
        lpXpBar.setPosition(lpBox.getPosition() + XpBarPos);
    }
    else if (pState == State::Showing) {
        // player bar size
        if (lpHpBar.getSize().x < lpHpBarTarget) {
            const float nw = std::min(lpHpBarTarget, lpHpBar.getSize().x + change);
            lpHpBar.setSize({nw, BarSize.y});
        }
        else if (lpHpBar.getSize().x > lpHpBarTarget) {
            const float nw = std::max(lpHpBarTarget, lpHpBar.getSize().x - change);
            lpHpBar.setSize({nw, BarSize.y});
        }

        // player bar color
        const float lphp = lpHpBar.getSize().x / BarSize.x;
        lpHpBar.setFillColor(Properties::HPBarColor(lphp));

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
}

bool StatBoxes::synced() const {
    return opHpBar.getSize().x == opHpBarTarget && lpHpBar.getSize().x == lpHpBarTarget &&
           lpXpBar.getSize().x == lpXpBarTarget;
}

void StatBoxes::render(sf::RenderTarget& target) const {
    if (oState != State::Hidden) {
        target.draw(opHpBar);
        target.draw(opBox);
        target.draw(opName);
        target.draw(opLevel);
        target.draw(opAil);
    }

    if (pState != State::Hidden) {
        target.draw(lpHpBar);
        target.draw(lpXpBar);
        target.draw(lpBox);
        target.draw(lpName);
        target.draw(lpHp);
        target.draw(lpLevel);
        target.draw(lpAil);
    }
}

const sf::Texture& StatBoxes::ailmentTexture(pplmn::Ailment ail) {
    ailTxtr = Properties::AilmentTexture(ail);
    return ailTxtr ? *ailTxtr : blank;
}

} // namespace view
} // namespace battle
} // namespace core
