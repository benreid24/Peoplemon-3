#include <Game/States/NewGame.hpp>

#include <Core/Properties.hpp>
#include <Core/Resources.hpp>
#include <Game/States/MainGame.hpp>

namespace game
{
namespace state
{
namespace
{
constexpr float RotateSpeed     = 120.f;
constexpr float FadeTime        = 1.6f;
constexpr float ProfMoveSpeedX  = -550.f;
constexpr float ProfMoveSpeedY  = 450.f;
constexpr float ProfRotateSpeed = -75.f;
} // namespace

NewGame::Ptr NewGame::create(core::system::Systems& systems) { return Ptr(new NewGame(systems)); }

NewGame::NewGame(core::system::Systems& systems)
: State(systems)
, fadeout(nullptr) {}

const char* NewGame::name() const { return "NewGame"; }

void NewGame::activate(bl::engine::Engine& engine) {
    if (!bgndTxtr) {
        bgndTxtr = engine.renderer().texturePool().getOrLoadTexture(bl::util::FileUtil::joinPath(
            core::Properties::MenuImagePath(), "NewGame/newGameBgnd.png"));
        profTxtr = engine.renderer().texturePool().getOrLoadTexture(bl::util::FileUtil::joinPath(
            core::Properties::MenuImagePath(), "NewGame/professor.png"));

        background.create(engine, bgndTxtr);
        background.getTransform().setOrigin(bgndTxtr->size() * 0.5f);
        background.getTransform().setPosition(core::Properties::WindowSize().x * 0.5f,
                                              core::Properties::WindowSize().y * 0.5f);

        prof.create(engine, profTxtr);
        prof.getTransform().setPosition(background.getTransform().getLocalPosition() -
                                        profTxtr->size() * 0.5f);
        prof.getTransform().setDepth(-1.f); // ensure rendered after background w/o parenting
    }

    auto overlay = engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    background.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);
    prof.addToScene(overlay, bl::rc::UpdateSpeed::Dynamic);

    systems.hud().displayMessage("Hello there!");
    systems.hud().displayMessage("I'm the professor in HomeTown. My name is Professor, but you can "
                                 "just call me Professor Professor");
    systems.hud().getInputString(
        "Your mom told me she had a kid, but I wasn't really listening. What's your name?",
        1,
        24,
        std::bind(&NewGame::nameSet, this, std::placeholders::_1));
}

void NewGame::nameSet(const std::string& n) {
    playerName = n;
    systems.hud().displayMessage("Wow your parents must not care for you much");
    systems.hud().promptUser(
        "It's hard to tell with, uh... my bad vision, but are you a boy or a girl?",
        {"Boy", "Girl"},
        std::bind(&NewGame::genderSet, this, std::placeholders::_1));
}

void NewGame::genderSet(const std::string& g) {
    isBoy = g == "Boy";
    systems.hud().displayMessage("........ Really?");
    systems.hud().displayMessage("Right then. Well I have to go do important professor things. I'm "
                                 "sure you'll figure the rest out",
                                 std::bind(&NewGame::fadeOut, this));
}

void NewGame::fadeOut() {
    fadeout = systems.engine()
                  .renderer()
                  .getObserver()
                  .getRenderGraph()
                  .putTask<bl::rc::rgi::FadeEffectTask>(FadeTime);
}

void NewGame::deactivate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().popScene();
    fadeout = nullptr;

    if (!systems.world().switchMaps("Hometown/HometownYourHouseYourRoom.map", 5)) {
        BL_LOG_ERROR << "Failed to load starting map";
        systems.engine().flags().set(bl::engine::Flags::Terminate);
    }
    systems.clock().set({12, 0});
    systems.player().newGame(playerName,
                             isBoy ? core::player::Gender::Boy : core::player::Gender::Girl);
}

void NewGame::update(bl::engine::Engine&, float dt, float) {
    background.getTransform().rotate(RotateSpeed * dt);
    if (fadeout) {
        if (fadeout->complete()) { systems.engine().replaceState(MainGame::create(systems)); }

        if (prof.getTransform().getRotation() > -90.f) {
            prof.getTransform().rotate(dt * ProfRotateSpeed);
        }
        prof.getTransform().move({dt * ProfMoveSpeedX, dt * ProfMoveSpeedY});
    }
}

} // namespace state
} // namespace game
