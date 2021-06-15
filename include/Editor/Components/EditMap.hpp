#ifndef EDITOR_COMPONENTS_EDITMAP_HPP
#define EDITOR_COMPONENTS_EDITMAP_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Systems/Cameras/Camera.hpp>

namespace editor
{
namespace component
{
class EditMap
: public bl::gui::Element
, public core::map::Map {
public:
    typedef std::shared_ptr<EditMap> Ptr;

    using ClickCb = std::function<void(const sf::Vector2f& pixels, const sf::Vector2i& tiles)>;

    static Ptr create(const ClickCb& cb, core::system::Systems& systems);

    virtual ~EditMap() = default;

    bool unsavedChanges() const;

    void newMap(const std::string& filename, const std::string& name, const std::string& tileset,
                unsigned int width, unsigned int height);

    bool editorLoad(const std::string& filename);

    bool editorSave();

    const char* undoDescription() const;

    void undo();

    const char* redoDescription() const;

    void redo();

    void setControlsEnabled(bool enabled);

    void resize(unsigned int width, unsigned int height, bool modLeft, bool modTop);

    void setName(const std::string& name);

    void setPlaylist(const std::string& playlist);

    void setWeather(core::map::Weather::Type weather);

    void setYSortLayer(unsigned int layer);

    void setTopLayer(unsigned int layer);

    void setOnEnterScript(const std::string& script);

    void setOnExitScript(const std::string& script);

    void setAmbientLight(std::uint8_t lower, std::uint8_t upper, bool sunlight);

    void appendLevel();

    void shiftLevel(unsigned int level, bool up);

    void removeLevel(unsigned int level);

    void appendLayer();

    void shiftLayer(unsigned int layer, bool up);

    void removeLayer(unsigned int layer);

    void setTile(unsigned int level, unsigned int layer, const sf::Vector2i& position,
                 core::map::Tile::IdType id, bool isAnim);

    void setTileArea(unsigned int level, unsigned int layer, const sf::IntRect& area,
                     core::map::Tile::IdType id, bool isAnim);

    void setCollision(unsigned int level, const sf::Vector2i& position, core::map::Collision id);

    void setCollisionArea(unsigned int level, const sf::IntRect& area, core::map::Collision id);

    void setCatch(unsigned int level, const sf::Vector2i& position, core::map::Catch id);

    void setCatchArea(unsigned int level, const sf::IntRect& area, core::map::Catch id);

    void setSpawn(const core::map::Spawn& spawn);

    void removeSpawn(const sf::Vector2i& position);

    void addNpcSpawn(const core::map::CharacterSpawn& spawn);

    const core::map::CharacterSpawn* getNpcSpawn(const sf::Vector2i& position);

    void editNpcSpawn(const core::map::CharacterSpawn& orig,
                      const core::map::CharacterSpawn& spawn);

    void removeNpcSpawn(const sf::Vector2i& position);

    void addItem(unsigned int level, const sf::Vector2i& position, core::item::Id item,
                 bool visible);

    void editItem(const sf::Vector2i& position, core::item::Id item, bool visible);

    void removeItem(const sf::Vector2i& position);

    void setLight(const sf::Vector2i& positionPixels, unsigned int radius);

    void removeLight(const sf::Vector2i& positionPixels);

    void createEvent(const core::map::Event& event);

    const core::map::Event* getEvent(const sf::Vector2i& position);

    void editEvent(const core::map::Event* orig, const core::map::Event& event);

    void removeEvent(const sf::Vector2i& position);

    void addCatchZone(const core::map::CatchZone& zone);

    const core::map::CatchZone* getCatchZone(const sf::Vector2i& position);

    void editCatchZone(const core::map::CatchZone* orig, const core::map::CatchZone& zone);

    void removeCatchZone(const sf::Vector2i& position);

private:
    struct Action {
        using Ptr = std::shared_ptr<Action>;

        virtual ~Action()                           = default;
        virtual bool apply(component::EditMap& map) = 0;
        virtual bool undo(component::EditMap& map)  = 0;
        virtual const char* description() const     = 0;
    };
    std::vector<Action::Ptr> history;
    unsigned int historyHead;
    void addAction(const Action::Ptr& action);

    struct EditCamera : public core::system::camera::Camera {
        using Ptr = std::shared_ptr<EditCamera>;
        EditCamera();
        virtual ~EditCamera() = default;
        virtual bool valid() const override;
        virtual void update(core::system::Systems& s, float dt) override;
        void reset(const sf::Vector2i& size);
        bool enabled;
    };

    const ClickCb clickCb;
    EditCamera::Ptr camera;
    bool changedSinceSave;
    bool controlsEnabled;
    std::string savefile;
    mutable sf::View renderView;

    EditMap(const ClickCb& cb, core::system::Systems& systems);
    bool doLoad(const std::string& file);

    virtual sf::Vector2i minimumRequisition() const override;
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const bl::gui::Renderer& renderer) const override;
    virtual bool handleScroll(const bl::gui::RawEvent& scroll) override;
    virtual void update(float dt) override;

    class SetNameAction;
    class SetPlaylistAction;
    class SetWeatherAction;
    class SetYSortLayerAction;
    class SetTopLayerAction;
    class SetEnterScriptAction;
    class SetExitScriptAction;
    class SetAmbientLightAction;
    class AppendLevelAction;
    class ShiftLevelAction;
    class RemoveLevelAction;
    class AppendLayerAction;
    class ShiftLayerAction;
    class RemoveLayerAction;
    class SetTileAction;
    class SetTileAreaAction;
    class SetCollisionAction;
    class SetCollisionAreaAction;
    class SetCatchAction;
    class SetCatchAreaAction;
    class SetSpawnAction;
    class RemoveSpawnAction;
    class AddNpcSpawnAction;
    class EditNpcSpawnAction;
    class RemoveNpcSpawnAction;
    class AddItemAction;
    class EditItemAction;
    class RemoveItemAction;
    class SetLightAction;
    class RemoveLightAction;
    class AddEventAction;
    class EditEventAction;
    class RemoveEventAction;
    class AddCatchZoneAction;
    class EditCatchZoneAction;
    class RemoveCatchZoneAction;
};

} // namespace component
} // namespace editor

#endif
