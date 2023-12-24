#include <Core/Debug/DebugBanner.hpp>

#include <Core/Properties.hpp>

#ifdef PEOPLEMON_DEBUG

namespace core
{
namespace debug
{
DebugBanner* DebugBanner::banner = nullptr;

DebugBanner::DebugBanner()
: engine(nullptr)
, showing(false)
, displayTime(0.f)
, timeout(2.f) {
    banner = this;
}

void DebugBanner::display(const std::string& m, float time) {
    auto& t = banner->text;

    if (t.entity() == bl::ecs::InvalidEntity) {
        t.create(*banner->engine, Properties::MenuFont(), m, 20, sf::Color::Cyan);
        t.getSection().setOutlineColor(sf::Color::Black);
        t.getSection().setOutlineThickness(2.f);
    }
    else { t.getSection().setString(m); }

    t.getTransform().setPosition(
        static_cast<float>(Properties::WindowWidth() - t.getLocalBounds().width - 10.f), 10.f);
    t.addToScene(banner->engine->renderer().getObserver().getOrCreateSceneOverlay(),
                 bl::rc::UpdateSpeed::Static);
    banner->timeout     = time;
    banner->displayTime = 0.f;
    banner->showing     = true;
}

void DebugBanner::init(bl::engine::Engine& e) { engine = &e; }

void DebugBanner::update(std::mutex&, float dt, float, float, float) {
    if (showing) {
        displayTime += dt;
        if (displayTime >= timeout) {
            showing = false;
            text.removeFromScene();
        }
    }
}

void DebugBanner::cleanup() { text.destroy(); }

} // namespace debug
} // namespace core

#endif
