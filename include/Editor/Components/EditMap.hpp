#ifndef EDITOR_COMPONENTS_EDITMAP_HPP
#define EDITOR_COMPONENTS_EDITMAP_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Maps/Map.hpp>
#include <Core/Systems/Cameras/Camera.hpp>

namespace editor
{
namespace page
{
class Map;
}

namespace component
{
/**
 * @brief Wrapper over the core::Map class that is directly usable in a bl::gui::GUI
 *
 * @ingroup Components
 *
 */
class EditMap
: public bl::gui::Element
, public core::map::Map {
public:
    /// Pointer to an EditMap
    typedef std::shared_ptr<EditMap> Ptr;

    /// Called when the map is clicked
    using PositionCb = std::function<void(const sf::Vector2f& pixels, const sf::Vector2i& tiles)>;

    /// Called on various event types
    using ActionCb = std::function<void()>;

    /**
     * @brief Optional render overlays depending on editor state
     *
     */
    enum struct RenderOverlay {
        /// No extra overlay
        None,

        /// Renders collisions for the current level
        Collisions,

        /// Renders catch tiles for the current level
        CatchTiles,

        /// Renders events in the map
        Events,

        /// Renders catch zones
        PeoplemonZones
    };

    /**
     * @brief Creates a new EditMap
     *
     * @param clickCb Called when the map is clicked
     * @param moveCb Called when the mouse is moved over a tile
     * @param actionCb Called when editor actions are performed
     * @param syncCb Called when map data has changed and the GUI should update
     * @param systems The primary systems object
     * @return Ptr The new EditMap
     */
    static Ptr create(const PositionCb& clickCb, const PositionCb& moveCb, const ActionCb& actionCb,
                      const ActionCb& syncCb, core::system::Systems& systems);

    /**
     * @brief Destroy the Edit Map object
     *
     */
    virtual ~EditMap() = default;

    /**
     * @brief Returns whether or not the map has been updated since last being saved
     *
     */
    bool unsavedChanges() const;

    /**
     * @brief Clears the current map and creates a new map with the given parameters
     *
     * @param filename The file to save the map into
     * @param name The name of the map
     * @param tileset The tileset to use
     * @param width The width of the map in tiles
     * @param height The height of the map in tiles
     */
    void newMap(const std::string& filename, const std::string& name, const std::string& tileset,
                unsigned int width, unsigned int height);

    /**
     * @brief Loads the map contents from the given file
     *
     * @param filename The map file to load from
     * @return True if loaded without error, false on error
     */
    bool editorLoad(const std::string& filename);

    /**
     * @brief Saves the map to the file it was loaded from or created with
     *
     * @return True on success, false on error
     */
    bool editorSave();

    /**
     * @brief Returns the description of what action will be undone if undo is called
     *
     */
    const char* undoDescription() const;

    /**
     * @brief Undoes the previous action in the edit history
     *
     */
    void undo();

    /**
     * @brief Returns the description of what action will be redone if redo is called
     *
     */
    const char* redoDescription() const;

    /**
     * @brief Reapplies the next action in the edit history
     *
     */
    void redo();

    /**
     * @brief Enables or disables the map camera and click controls
     *
     */
    void setControlsEnabled(bool enabled);

    /**
     * @brief Shows or hides the given level
     *
     * @param level The level to show or hide
     * @param visible True to render, false to hide
     */
    void setLevelVisible(unsigned int level, bool visible);

    /**
     * @brief Shows or hides the given layer
     *
     * @param level The level the layer is on
     * @param layer The index of the layer to show or hide
     * @param visible True to render, false to hide
     */
    void setLayerVisible(unsigned int level, unsigned int layer, bool visible);

    /**
     * @brief Sets which overlay gets rendered on top of the map
     *
     * @param overlay The overlay to render
     * @param level Which level to source the overlay from
     */
    void setRenderOverlay(RenderOverlay overlay, unsigned int level);

    /**
     * @brief Sets the tile selection to render. Set width or height to 0 to hide. Set width or
     *        height to negative values to show a single tile in blue (partial selection)
     *
     * @param selection The selection to render, in tiles
     */
    void showSelection(const sf::IntRect& selection);

    /**
     * @brief Removes all tiles that reference the given id in the tileset. This should be called
     *        before removing a tile from the tileset
     *
     * @param id The id of the tile in the tileset
     * @param isAnim True for an animation, false for a regular tile
     */
    void removeAllTiles(core::map::Tile::IdType id, bool isAnim);

    void resize(unsigned int width, unsigned int height, bool modLeft, bool modTop);

    void setName(const std::string& name);

    /**
     * @brief Sets the playlist of the map
     *
     * @param playlist Path to the playlist to set
     */
    void setPlaylist(const std::string& playlist);

    /**
     * @brief Set the weather in the map
     *
     * @param weather The weather to do
     */
    void setWeather(core::map::Weather::Type weather);

    /**
     * @brief Set the OnEnter script
     *
     * @param script The script to run when the map is entered
     */
    void setOnEnterScript(const std::string& script);

    /**
     * @brief Get the OnEnter script
     *
     */
    const std::string& getOnEnterScript() const;

    /**
     * @brief Set the OnExit script
     *
     * @param script The script to run when the map is left
     */
    void setOnExitScript(const std::string& script);

    /**
     * @brief Get the OnExit script
     *
     */
    const std::string& getOnExitScript() const;

    void setAmbientLight(std::uint8_t lower, std::uint8_t upper, bool sunlight);

    /**
     * @brief Creates a new level
     *
     */
    void appendLevel();

    /**
     * @brief Shifts the given level up or down
     *
     * @param level The level to move
     * @param up True for up, false for down
     */
    void shiftLevel(unsigned int level, bool up);

    /**
     * @brief Removes the given level
     *
     * @param level The level to remove
     */
    void removeLevel(unsigned int level);

    /**
     * @brief Creates a new (empty) bottom layer
     *
     * @param level The level to create the layer on
     */
    void appendBottomLayer(unsigned int level);

    /**
     * @brief Creates a new (empty) y-sort layer
     *
     * @param level The level to create the layer on
     */
    void appendYsortLayer(unsigned int level);

    /**
     * @brief Creates a new (empty) top layer
     *
     * @param level The level to create the layer on
     */
    void appendTopLayer(unsigned int level);

    /**
     * @brief Shifts a layer up or down in the render order. This will also move layers between
     *        bottom, y-sort, and top buckets
     *
     * @param level The level the layer is on
     * @param layer The layer to shift
     * @param up True to move up (render earlier), false to move down (render later)
     */
    void shiftLayer(unsigned int level, unsigned int layer, bool up);

    /**
     * @brief Deletes the given layer
     *
     * @param level The level the layer is on
     * @param layer Index of the layer to remove
     */
    void removeLayer(unsigned int level, unsigned int layer);

    /**
     * @brief Sets a single tile
     *
     * @param level The level to modify
     * @param layer The layer to modify
     * @param position The position of the tile to modify
     * @param id The new tile id
     * @param isAnim True for animation, false for texture
     */
    void setTile(unsigned int level, unsigned int layer, const sf::Vector2i& position,
                 core::map::Tile::IdType id, bool isAnim);

    /**
     * @brief Sets a range of tiles to a given value. Tries to avoid overcrowding for large tiles
     *
     * @param level The level to modify
     * @param layer The layer to modify
     * @param area Region of tiles to set
     * @param id New tile id
     * @param isAnim True for animation, false for texture
     */
    void setTileArea(unsigned int level, unsigned int layer, const sf::IntRect& area,
                     core::map::Tile::IdType id, bool isAnim);

    /**
     * @brief Sets a single collision tile
     *
     * @param level The level to modify
     * @param position The position of the collision to set
     * @param id The new collision value
     */
    void setCollision(unsigned int level, const sf::Vector2i& position, core::map::Collision id);

    /**
     * @brief Sets a range of collision tiles to a single value
     *
     * @param level The level to modify
     * @param area The region to set
     * @param id The value to set all tiles to
     */
    void setCollisionArea(unsigned int level, const sf::IntRect& area, core::map::Collision id);

    /**
     * @brief Sets a single catch tile
     *
     * @param level The level to modify
     * @param position The position of the catch to set
     * @param id The new catch value
     */
    void setCatch(unsigned int level, const sf::Vector2i& position, core::map::Catch id);

    /**
     * @brief Sets a range of catch tiles to a single value
     *
     * @param level The level to modify
     * @param area The region to set
     * @param id The value to set all tiles to
     */
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

    /**
     * @brief Creates a new map event
     *
     * @param event The event to create
     */
    void createEvent(const core::map::Event& event);

    /**
     * @brief Returns a pointer to an event that overlaps the given position
     *
     * @param position The position to search
     * @return const core::map::Event* Pointer to event or nullptr if none overlap
     */
    const core::map::Event* getEvent(const sf::Vector2i& position);

    /**
     * @brief Alters the value of the given event
     *
     * @param orig Pointer to the event to update
     * @param event New value to set
     */
    void editEvent(const core::map::Event* orig, const core::map::Event& event);

    /**
     * @brief Deletes the given event
     *
     * @param event Pointer to the event to delete
     */
    void removeEvent(const core::map::Event* event);

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
    unsigned int saveHead;
    void addAction(const Action::Ptr& action);

    struct EditCamera : public core::system::camera::Camera {
        using Ptr = std::shared_ptr<EditCamera>;
        EditCamera();
        virtual ~EditCamera() = default;
        virtual bool valid() const override;
        virtual void update(core::system::Systems& s, float dt) override;
        void reset(const sf::Vector2i& size);
        void zoom(float z);
        bool enabled;
    };

    const PositionCb clickCb;
    const PositionCb moveCb;
    const ActionCb actionCb;
    const ActionCb syncCb;
    EditCamera::Ptr camera;
    bool controlsEnabled;
    std::string savefile;
    std::vector<bool> levelFilter;
    std::vector<std::vector<bool>> layerFilter;
    sf::IntRect selection;
    mutable sf::View renderView;
    mutable sf::RectangleShape selectRect;
    mutable sf::Sprite overlaySprite;
    RenderOverlay renderOverlay;
    unsigned int overlayLevel;

    EditMap(const PositionCb& cb, const PositionCb& moveCb, const ActionCb& actionCb,
            const ActionCb& syncCb, core::system::Systems& systems);
    bool doLoad(const std::string& file);
    bool editorActivate();

    virtual sf::Vector2i minimumRequisition() const override;
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const bl::gui::Renderer& renderer) const override;
    virtual bool handleScroll(const bl::gui::RawEvent& scroll) override;
    virtual void update(float dt) override;
    virtual void render(sf::RenderTarget& target, float residual,
                        const core::map::Map::EntityRenderCallback& entityCb) const override;

    friend class page::Map;

    class SetNameAction;
    class SetPlaylistAction;
    class SetWeatherAction;
    class SetYSortLayerAction;
    class SetTopLayerAction;
    class SetScriptAction;
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
