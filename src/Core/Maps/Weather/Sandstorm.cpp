#include "Sandstorm.hpp"

#include <BLIB/Util/Random.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace map
{
namespace weather
{
namespace
{
const sf::Vector2f SandVelocity(-1100.f, 345.f);
const sf::Vector2f SwirlVelocity(-680.f, 325.f);
constexpr std::uint8_t MaxAlpha   = 230;
constexpr float SwirlRatio        = 0.3f;
constexpr float AlphaRate         = 85.f;
constexpr unsigned int SwirlCount = 40;

float computeAlpha(float current, float target, float dt) {
    const float rate = target > current ? AlphaRate : -AlphaRate;
    const float na   = current + rate * dt;
    if (na < 0) return 0.f;
    if (na > 255) return 255.f;
    if (na > target && target > current) return target;
    return na;
}
} // namespace

Sandstorm::Sandstorm()
: targetAlpha(0)
, alpha(0) {
    sandTxtr  = TextureManager::load(Properties::SandPatchFile());
    swirlTxtr = TextureManager::load(Properties::SandSwirlFile());
    sand.setTexture(*sandTxtr, true);
    swirl.setTexture(*swirlTxtr, true);
    sand.setOrigin(sandTxtr->getSize().x / 2, sandTxtr->getSize().y / 2);
    swirl.setOrigin(swirlTxtr->getSize().x / 2, swirlTxtr->getSize().y / 2);
}

Weather::Type Sandstorm::type() const { return Weather::SandStorm; }

void Sandstorm::stop() { targetAlpha = 0; }

bool Sandstorm::stopped() const { return static_cast<int>(alpha) == 0; }

void Sandstorm::start(const sf::FloatRect& a) {
    area        = a;
    targetAlpha = MaxAlpha;
    alpha       = 0;

    const sf::Vector2f corner(area.left - area.width / 2.f, area.top - area.height / 2.f);
    const float w             = sandTxtr->getSize().x / 1.2;
    const float h             = sandTxtr->getSize().y / 1.2;
    const unsigned int width  = area.width * 2 / w;
    const unsigned int height = area.height * 2 / h;

    sandPatches.clear();
    sandPatches.reserve(width * height);
    for (unsigned int x = 0; x < width; ++x) {
        for (unsigned int y = 0; y < height; ++y) {
            sandPatches.emplace_back(corner.x + w * x, corner.y + h * y);
        }
    }

    swirlParticles.clear();
    swirlParticles.reserve(SwirlCount);
    for (unsigned int i = 0; i < SwirlCount; ++i) {
        swirlParticles.emplace_back();
        swirlParticles.back().set(
            bl::util::Random::get<float>(corner.x, area.left + area.width * 1.5f),
            bl::util::Random::get<float>(corner.y, area.top + area.height * 1.5f));
    }
}

void Sandstorm::update(float dt) {
    if (static_cast<std::uint8_t>(alpha) != targetAlpha) {
        alpha = computeAlpha(alpha, targetAlpha, dt);
    }

    const float leftBound   = area.left - area.width / 2.f;
    const float rightBound  = area.left + area.width * 1.5;
    const float topBound    = area.top - area.height / 2.f;
    const float bottomBound = area.top + area.height * 1.5;
    const float width       = area.width * 2.f;
    const float height      = area.height * 2.f;
    for (sf::Vector2f& sp : sandPatches) {
        sp += SandVelocity * dt;
        if (sp.x < leftBound) sp.x += width;
        if (sp.x > rightBound) sp.x -= width;
        if (sp.y < topBound) sp.y += height;
        if (sp.y > bottomBound) sp.y -= height;
    }

    for (Swirl& s : swirlParticles) {
        s.position += SwirlVelocity * dt;
        s.angle += s.angularVel * dt;
        if (s.position.x < leftBound || s.position.y > bottomBound) {
            s.set(bl::util::Random::get<float>(rightBound - 100.f, rightBound),
                  bl::util::Random::get<float>(topBound, topBound + width));
        }
    }
}

void Sandstorm::render(sf::RenderTarget& target, float lag) const {
    area = {target.getView().getCenter() - target.getView().getSize() / 2.f,
            target.getView().getSize()};

    const float a  = computeAlpha(alpha, targetAlpha, lag);
    const float sa = a * SwirlRatio;

    sand.setColor(sf::Color(255, 255, 255, a));
    for (const sf::Vector2f& sp : sandPatches) {
        sand.setPosition(sp + SandVelocity * lag);
        target.draw(sand);
    }

    swirl.setColor(sf::Color(255, 255, 255, sa));
    for (const Swirl& s : swirlParticles) {
        const sf::Vector2f interp(s.position + SwirlVelocity * lag);
        swirl.setPosition(interp.x, interp.y);
        swirl.setRotation(s.angle + s.angularVel * lag);
        swirl.setScale(s.scale, s.scale);
        target.draw(swirl);
    }
}

void Sandstorm::Swirl::set(float x, float y) {
    position.x = x;
    position.y = y;
    angle      = bl::util::Random::get<float>(0.f, 360.f);
    angularVel = bl::util::Random::get<float>(560.f, 820.f);
    if (bl::util::Random::chance(1, 2)) angularVel *= -1;
    scale = bl::util::Random::get<float>(0.75f, 1.2f);
}

} // namespace weather
} // namespace map
} // namespace core
