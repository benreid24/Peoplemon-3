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
constexpr glm::vec2 BarSize(96.f, 6.f);

constexpr glm::vec2 XpBarPos(52.f, 88.f);
constexpr glm::vec2 XpBarSize(192.f, 4.f);

constexpr glm::vec2 OpBoxPos(0.f, 0.f);
constexpr glm::vec2 OpBarPos(100.f, 50.f);

constexpr glm::vec2 LpBoxPos(540.f, 357.f);
constexpr glm::vec2 LpBarPos(148.f, 54.f);

constexpr float NameSize  = 18.f;
constexpr float LevelSize = 15.f;
const sf::Color LevelColor(20.f, 20.f, 140.f);

std::string makeHpStr(unsigned int cur, unsigned int max) {
    std::stringstream ss;
    ss << cur << "/" << max;
    return ss.str();
}

} // namespace

StatBoxes::StatBoxes(bl::engine::Engine& engine)
: engine(engine)
, localPlayer(nullptr)
, opponent(nullptr)
, pState(State::Hidden)
, oState(State::Hidden)
, opHpBarTarget(0.f)
, lpHpBarTarget(0.f)
, lpXpBarTarget(0.f) {}

void StatBoxes::init(bl::rc::scene::CodeScene* scene) {
    constexpr auto join        = bl::util::FileUtil::joinPath;
    const std::string& ImgPath = Properties::ImagePath();

    opBoxTxtr = engine.renderer().texturePool().getOrLoadTexture(join(ImgPath, "Battle/opBox.png"));
    opBox.create(engine, opBoxTxtr);
    opBox.getTransform().setPosition(-opBoxTxtr->size().x, OpBoxPos.y);
    opBox.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpBoxTxtr = engine.renderer().texturePool().getOrLoadTexture(join(ImgPath, "Battle/pBox.png"));
    lpBox.create(engine, lpBoxTxtr);
    lpBox.getTransform().setPosition(Properties::WindowWidth() + lpBoxTxtr->size().x, LpBoxPos.y);
    lpBox.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpXpBar.create(engine, {100.f, 100.f});
    lpXpBar.setFillColor(sf::Color::Blue);
    lpXpBar.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    opName.create(engine, Properties::MenuFont(), "", NameSize, sf::Color::Black, sf::Text::Bold);
    opName.addToScene(scene, bl::rc::UpdateSpeed::Static);

    opLevel.create(engine, Properties::MenuFont(), "", LevelSize, LevelColor, sf::Text::Bold);
    opLevel.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpName.create(engine, Properties::MenuFont(), "", NameSize, sf::Color::Black, sf::Text::Bold);
    lpName.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpLevel.create(engine, Properties::MenuFont(), "", LevelSize, LevelColor, sf::Text::Bold);
    lpLevel.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpHp.create(engine, Properties::MenuFont(), "", 14, sf::Color::Black);
    lpHp.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpHpBar.create(engine, {100.f, 100.f});
    lpHpBar.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    opHpBar.create(engine, {100.f, 100.f});
    opHpBar.addToScene(scene, bl::rc::UpdateSpeed::Dynamic);

    opAil.create(engine, engine.renderer().texturePool().getBlankTexture());
    opAil.addToScene(scene, bl::rc::UpdateSpeed::Static);

    lpAil.create(engine, engine.renderer().texturePool().getBlankTexture());
    lpAil.addToScene(scene, bl::rc::UpdateSpeed::Static);

    // opponent positions + parenting
    opHpBar.getTransform().setPosition(OpBarPos);
    opHpBar.setParent(opBox);
    opAil.getTransform().setPosition(glm::vec2(10.f, 46.f));
    opAil.setParent(opBox);
    opName.getTransform().setPosition(glm::vec2(5.f, 18.f));
    opName.setParent(opBox);
    opLevel.getTransform().setPosition(glm::vec2(168.f, 21.f));
    opLevel.setParent(opBox);

    // player positions + parenting
    lpAil.getTransform().setPosition(glm::vec2(33.f, 61.f));
    lpAil.setParent(lpBox);
    lpName.getTransform().setPosition(glm::vec2(41.f, 22.f));
    lpName.setParent(lpBox);
    lpLevel.getTransform().setPosition(glm::vec2(218.f, 24.f));
    lpLevel.setParent(lpBox);
    lpHp.getTransform().setPosition(glm::vec2(131.f, 66.f));
    lpHp.setParent(lpBox);
    lpHpBar.getTransform().setPosition(LpBarPos);
    lpHpBar.setParent(lpBox);
    lpXpBar.getTransform().setPosition(XpBarPos);
    lpXpBar.setParent(lpBox);
}

void StatBoxes::setOpponent(pplmn::BattlePeoplemon* ppl) {
    opponent = ppl;
    opHpBar.scaleToSize({0.f, BarSize.y});
    if (oState == State::Hidden) { oState = State::Sliding; }
    sync(false);
}

void StatBoxes::setPlayer(pplmn::BattlePeoplemon* ppl) {
    localPlayer = ppl;
    lpHpBar.scaleToSize({0.f, BarSize.y});
    lpXpBar.scaleToSize({0.f, XpBarSize.y});
    if (pState == State::Hidden) { pState = State::Sliding; }
    sync(true);
}

void StatBoxes::sync(bool fromSwitch) {
    if (localPlayer) {
        lpName.getSection().setString(localPlayer->base().name());
        lpLevel.getSection().setString(std::to_string(localPlayer->base().currentLevel()));
        lpHp.getSection().setString(
            makeHpStr(localPlayer->base().currentHp(), localPlayer->currentStats().hp));
        lpAil.setTexture(ailmentTexture(localPlayer->base().currentAilment()));
        lpHpBarTarget = (static_cast<float>(localPlayer->base().currentHp()) /
                         static_cast<float>(localPlayer->currentStats().hp)) *
                        BarSize.x;
        lpXpBarTarget = (static_cast<float>(localPlayer->base().currentXP()) /
                         static_cast<float>(localPlayer->base().nextLevelXP())) *
                        XpBarSize.x;
        if (fromSwitch) { lpXpBar.scaleToSize({lpXpBarTarget, XpBarSize.y}); }
    }
    else {
        BL_LOG_WARN << "sync() called with null player peoplemon";
        lpHpBarTarget = 0.f;
        lpXpBarTarget = 0.f;
        lpName.getSection().setString("");
    }

    if (opponent) {
        opName.getSection().setString(opponent->base().name());
        opLevel.getSection().setString(std::to_string(opponent->base().currentLevel()));
        opAil.setTexture(ailmentTexture(opponent->base().currentAilment()));
        opHpBarTarget = (static_cast<float>(opponent->base().currentHp()) /
                         static_cast<float>(opponent->currentStats().hp)) *
                        BarSize.x;
    }
    else {
        BL_LOG_WARN << "sync() called with null opponent peoplemon";
        opHpBarTarget = 0.f;
        opName.getSection().setString("");
    }
}

void StatBoxes::update(float dt) {
    const float change = dt * BarRate * BarSize.x;

    // opponent box slide
    if (oState == State::Sliding) {
        const float dx = SlideSpeed * dt;
        opBox.getTransform().move({dx, 0.f});
        if (opBox.getTransform().getLocalPosition().x > OpBoxPos.x) {
            opBox.getTransform().setPosition(OpBoxPos);
            oState = State::Showing;
        }
    }
    else if (oState == State::Showing) {
        // opponent bar size
        if (opHpBar.getGlobalSize().x < opHpBarTarget) {
            const float nw = std::min(opHpBarTarget, opHpBar.getGlobalSize().x + change);
            opHpBar.scaleToSize({nw, BarSize.y});
        }
        else if (opHpBar.getGlobalSize().x > opHpBarTarget) {
            const float nw = std::max(opHpBarTarget, opHpBar.getGlobalSize().x - change);
            opHpBar.scaleToSize({nw, BarSize.y});
        }

        // opponent bar color
        const float opHp = opHpBar.getSize().x / BarSize.x;
        opHpBar.setFillColor(Properties::HPBarColor(opHp));
    }

    // player box slide
    if (pState == State::Sliding) {
        const float dx = SlideSpeed * dt;
        lpBox.getTransform().move({-dx, 0.f});
        if (lpBox.getTransform().getLocalPosition().x < LpBoxPos.x) {
            lpBox.getTransform().setPosition(LpBoxPos);
            pState = State::Showing;
        }
    }
    else if (pState == State::Showing) {
        // player bar size
        if (lpHpBar.getGlobalSize().x < lpHpBarTarget) {
            const float nw = std::min(lpHpBarTarget, lpHpBar.getGlobalSize().x + change);
            lpHpBar.scaleToSize({nw, BarSize.y});
        }
        else if (lpHpBar.getGlobalSize().x > lpHpBarTarget) {
            const float nw = std::max(lpHpBarTarget, lpHpBar.getGlobalSize().x - change);
            lpHpBar.scaleToSize({nw, BarSize.y});
        }

        // player bar color
        const float lphp = lpHpBar.getGlobalSize().x / BarSize.x;
        lpHpBar.setFillColor(Properties::HPBarColor(lphp));

        // xp bar size
        const float xpChange = dt * BarRate * XpBarSize.x;
        if (lpXpBar.getGlobalSize().x < lpXpBarTarget) {
            const float nw = std::min(lpXpBarTarget, lpXpBar.getGlobalSize().x + xpChange);
            lpXpBar.scaleToSize({nw, XpBarSize.y});
        }
        else if (lpXpBar.getGlobalSize().x > lpXpBarTarget) {
            // no transition down
            lpXpBar.scaleToSize({lpXpBarTarget, XpBarSize.y});
        }
    }
}

bool StatBoxes::synced() const {
    return opHpBar.getGlobalSize().x == opHpBarTarget &&
           lpHpBar.getGlobalSize().x == lpHpBarTarget && lpXpBar.getGlobalSize().x == lpXpBarTarget;
}

void StatBoxes::render(bl::rc::scene::CodeScene::RenderContext& ctx) {
    if (oState != State::Hidden) {
        opHpBar.draw(ctx);
        opHpBar.draw(ctx);
        opBox.draw(ctx);
        opName.draw(ctx);
        opLevel.draw(ctx);
        opAil.draw(ctx);
    }

    if (pState != State::Hidden) {
        lpHpBar.draw(ctx);
        lpXpBar.draw(ctx);
        lpBox.draw(ctx);
        lpName.draw(ctx);
        lpHp.draw(ctx);
        lpLevel.draw(ctx);
        lpAil.draw(ctx);
    }
}

bl::rc::res::TextureRef StatBoxes::ailmentTexture(pplmn::Ailment ail) {
    const auto src = Properties::AilmentTexture(ail);
    return src.empty() ? engine.renderer().texturePool().getBlankTexture() :
                         engine.renderer().texturePool().getOrLoadTexture(src);
}

} // namespace view
} // namespace battle
} // namespace core
