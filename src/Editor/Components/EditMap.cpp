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

EditMap::EditMap(const ClickCb& clickCb, core::system::Systems& s)
: bl::gui::Element("maps", "editmap")
, Map()
, clickCb(clickCb)
, changedSinceSave(false)
, camera(EditCamera::Ptr(new EditCamera())) {
    systems = &s;
    // TODO - init stuff
}

bool EditMap::editorLoad(const std::string& file) {
    changedSinceSave = false;

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

    lighting.subscribe(systems->engine().eventBus());

    for (const core::map::CharacterSpawn& spawn : characterField.getValue()) {
        if (!systems->entity().spawnCharacter(spawn)) {
            BL_LOG_WARN << "Failed to spawn character: " << spawn.file.getValue();
        }
    }

    for (const core::map::Item& item : itemsField.getValue()) { systems->entity().spawnItem(item); }

    systems->engine().eventBus().dispatch<core::event::MapEntered>({*this});
    return true;
}

EditMap::EditCamera::EditCamera()
: enabled(true) {}

bool EditMap::EditCamera::valid() const { return true; }

void EditMap::EditCamera::update(core::system::Systems& s, float dt) {
    // TODO - move and stuff
}

void EditMap::EditCamera::reset(const sf::Vector2i& t) {
    position = sf::Vector2f(t) * static_cast<float>(core::Properties::PixelsPerTile()) * 0.5f;
    size     = 1.f;
}

sf::Vector2i EditMap::minimumRequisition() const { return {100, 100}; }

void EditMap::doRender(sf::RenderTarget& target, sf::RenderStates states,
                       const bl::gui::Renderer& renderer) const {
    const sf::View oldView = target.getView();
    target.setView(bl::gui::Container::computeView(oldView, getAcquisition()));
    systems->render().render(target, *this, 0.f);
    target.setView(oldView);
}

bool EditMap::handleScroll(const bl::gui::Action& scroll) {
    // TODO
    return true;
}

} // namespace component
} // namespace editor
