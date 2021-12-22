#include <Game/States/NewGame.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <Core/Properties.hpp>
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
, fadeTime(-1.f) {
    cover.setFillColor(sf::Color::Transparent);
    bgndTxtr = bl::engine::Resources::textures()
                   .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                      "NewGame/newGameBgnd.png"))
                   .data;
    profTxtr = bl::engine::Resources::textures()
                   .load(bl::util::FileUtil::joinPath(core::Properties::MenuImagePath(),
                                                      "NewGame/professor.png"))
                   .data;

    background.setTexture(*bgndTxtr, true);
    background.setOrigin(sf::Vector2f(bgndTxtr->getSize()) * 0.5f);
    background.setPosition(static_cast<float>(core::Properties::WindowWidth()) * 0.5f,
                           static_cast<float>(core::Properties::WindowHeight()) * 0.5f);

    prof.setTexture(*profTxtr, true);
    prof.setPosition(background.getPosition() - sf::Vector2f(profTxtr->getSize()) * 0.5f);
}

const char* NewGame::name() const { return "NewGame"; }

void NewGame::activate(bl::engine::Engine&) {
    systems.hud().displayMessage("Hello there!");
    systems.hud().displayMessage("I'm the professor in HomeTown. My name is Professor, but you can "
                                 "just call me Professor Professor");
    systems.hud().getInputString(
        "Your mom told me she had a kid, but I wasn't really listnening. What's your name?",
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
    fadeTime = 0.f;
    cover.setPosition(systems.engine().window().getView().getCenter());
    cover.setOrigin(systems.engine().window().getView().getSize() * 0.5f);
    cover.setSize(systems.engine().window().getView().getSize());
}

void NewGame::deactivate(bl::engine::Engine&) {
    if (!systems.world().switchMaps("Hometown/HometownYourHouseYourRoom.map", 5)) {
        BL_LOG_ERROR << "Failed to load world map";
        systems.engine().flags().set(bl::engine::Flags::Terminate);
    }
    systems.clock().set({12, 0});
    systems.player().newGame(playerName,
                             isBoy ? core::player::Gender::Boy : core::player::Gender::Girl);
}

void NewGame::update(bl::engine::Engine&, float dt) {
    systems.hud().update(dt);
    systems.player().update();
    background.rotate(RotateSpeed * dt);
    if (fadeTime >= 0.f) {
        fadeTime += dt;
        cover.setFillColor(sf::Color(0, 0, 0, std::min(255.f, 255.f * fadeTime / FadeTime)));
        if (fadeTime >= FadeTime) { systems.engine().replaceState(MainGame::create(systems)); }

        if (prof.getRotation() > -90.f) { prof.rotate(dt * ProfRotateSpeed); }
        prof.move(dt * ProfMoveSpeedX, dt * ProfMoveSpeedY);
    }
}

void NewGame::render(bl::engine::Engine&, float lag) {
    systems.engine().window().clear();
    systems.engine().window().draw(background);
    systems.engine().window().draw(prof);
    systems.hud().render(systems.engine().window(), lag);
    if (fadeTime >= 0.f) { systems.engine().window().draw(cover); }
    systems.engine().window().display();
}

} // namespace state
} // namespace game
