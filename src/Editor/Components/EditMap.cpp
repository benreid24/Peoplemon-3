#include <Editor/Components/EditMap.hpp>

#include <Core/Resources.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>

namespace editor
{
namespace component
{
EditMap::Ptr EditMap::create(const ClickCb& clickCb, core::system::Systems& systems) {
    return Ptr(new EditMap(clickCb, systems));
}

EditMap::EditMap(const ClickCb& cb, core::system::Systems& s)
: bl::gui::Element("maps", "editmap")
, Map()
, clickCb(cb)
, camera(EditCamera::Ptr(new EditCamera()))
, changedSinceSave(false)
, controlsEnabled(false) {
    systems = &s;
    getSignal(bl::gui::Action::LeftClicked)
        .willAlwaysCall([this, &s](const bl::gui::Action& a, Element*) {
            if (controlsEnabled) {
                static const float PixelRatio =
                    1.f / static_cast<float>(core::Properties::PixelsPerTile());

                // 0, 25???
                BL_LOG_INFO << getAcquisition().left << ", " << getAcquisition().top;
                sf::Vector2f localPos =
                    a.position + sf::Vector2f(-getAcquisition().left, getAcquisition().top);
                // localPos /= systems->cameras().activeCamera()->getSize();

                const sf::Vector2f pixels =
                    s.engine().window().mapPixelToCoords(sf::Vector2i(localPos), renderView);
                const sf::Vector2i tiles(std::floor(pixels.x * PixelRatio),
                                         std::floor(pixels.y * PixelRatio));
                clickCb(pixels, tiles);
            }
        });
    // TODO - init stuff
}

bool EditMap::editorLoad(const std::string& file) {
    if (!doLoad(file)) {
        doLoad(savefile);
        return false;
    }
    savefile = file;
    return true;
}

bool EditMap::doLoad(const std::string& file) {
    changedSinceSave = false;

    clear();
    systems->engine().entities().clear();
    if (!Map::load(file)) return false;

    size = {static_cast<int>(levels.front().bottomLayers().front().width()),
            static_cast<int>(levels.front().bottomLayers().front().height())};
    systems->engine().eventBus().dispatch<core::event::MapSwitch>({*this});

    camera->reset(size);
    systems->cameras().clearAndReplace(camera);
    systems->cameras().update(0.f);
    weather.activate(systems->cameras().getArea());

    tileset = core::Resources::tilesets().load(tilesetField).data;
    if (!tileset) return false;
    tileset->activate();
    for (core::map::LayerSet& level : levels) { level.activate(*tileset); }

    weather.set(weatherField.getValue());
    lighting.activate(size);
    for (core::map::CatchZone& zone : catchZonesField.getValue()) { zone.activate(); }

    core::script::LegacyWarn::warn(loadScriptField);
    core::script::LegacyWarn::warn(unloadScriptField);

    if (!activated) {
        activated = true;
        lighting.subscribe(systems->engine().eventBus());
    }

    for (const core::map::CharacterSpawn& spawn : characterField.getValue()) {
        if (!systems->entity().spawnCharacter(spawn)) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file.getValue();
        }
    }

    for (const core::map::Item& item : itemsField.getValue()) { systems->entity().spawnItem(item); }

    systems->engine().eventBus().dispatch<core::event::MapEntered>({*this});
    return true;
}

bool EditMap::unsavedChanges() const { return changedSinceSave; }

void EditMap::update(float dt) { Map::update(*systems, dt); }

void EditMap::setControlsEnabled(bool e) {
    controlsEnabled = e;
    camera->enabled = e;
}

EditMap::EditCamera::EditCamera()
: enabled(true) {}

bool EditMap::EditCamera::valid() const { return true; }

void EditMap::EditCamera::update(core::system::Systems&, float dt) {
    if (enabled) {
        float PixelsPerSecond = 0.5f * size * static_cast<float>(core::Properties::WindowWidth());
        static const float ZoomPerSecond = 0.5f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) PixelsPerSecond *= 5.f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { position.y -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { position.x += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { position.y += PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { position.x -= PixelsPerSecond * dt; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
            size -= ZoomPerSecond * dt;
            if (size < 0.1f) size = 0.1f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) { size += ZoomPerSecond * dt; }
    }
}

void EditMap::EditCamera::reset(const sf::Vector2i& t) {
    position = sf::Vector2f(t) * static_cast<float>(core::Properties::PixelsPerTile()) * 0.5f;
    size     = 1.f;
}

sf::Vector2i EditMap::minimumRequisition() const { return {100, 100}; }

void EditMap::doRender(sf::RenderTarget& target, sf::RenderStates, const bl::gui::Renderer&) const {
    const sf::View oldView = target.getView();
    renderView             = bl::gui::Container::computeView(oldView, getAcquisition());
    systems->cameras().configureView(*this, renderView);
    target.setView(renderView);
    systems->render().render(target, *this, 0.f);
    target.setView(oldView);
}

bool EditMap::handleScroll(const bl::gui::RawEvent&) {
    // TODO
    return true;
}

} // namespace component
} // namespace editor
