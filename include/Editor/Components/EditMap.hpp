#ifndef EDITOR_COMPONENTS_EDITMAP_HPP
#define EDITOR_COMPONENTS_EDITMAP_HPP

#include <BLIB/Cameras/2D/Camera2D.hpp>
#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Maps/Map.hpp>
#include <Editor/Components/Render/EditMapComponent.hpp>
#include <Editor/Components/RenderMapWindow.hpp>

namespace editor
{
namespace page
{
class Map;
class Towns;
} // namespace page

namespace component
{
/**
 * @brief Wrapper over the core::Map class that is directly usable in a bl::gui::GUI
 *
 * @ingroup UIComponents
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
     */
    enum struct RenderOverlay {
        /// No extra overlay
        None,

        /// Renders collisions for the current level
        Collisions,

        /// Renders catch tiles for the current level
        CatchTiles,

        /// Renders colored tiles to indicate towns/routes
        Towns,

        /// Renders events in the map
        Events,

        /// Renders spawns in the map
        Spawns,

        /// Renders level transitions
        LevelTransitions
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
     */
    virtual ~EditMap();

    /**
     * @brief Returns whether or not the map has been updated since last being saved
     */
    bool unsavedChanges() const;

    /**
     * @brief Returns the current file the map is saving to
     */
    const std::string& currentFile() const;

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
     */
    const char* undoDescription() const;

    /**
     * @brief Undoes the previous action in the edit history
     */
    void undo();

    /**
     * @brief Returns the description of what action will be redone if redo is called
     */
    const char* redoDescription() const;

    /**
     * @brief Reapplies the next action in the edit history
     */
    void redo();

    /**
     * @brief Enables or disables the map camera and click controls
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
     * @brief Set whether or not to render a grid between tiles
     */
    void showGrid(bool show);

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

    /**
     * @brief Set the name of the map
     *
     * @param name The new name of the map
     */
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
     */
    const std::string& getOnExitScript() const;

    /**
     * @brief Set the maps default ambient light settings
     *
     * @param lower The lower light level at night
     * @param upper The upper light level during the day (or always if no sunlight)
     * @param sunlight True to adjust between lower and upper for time of day, false for upper only
     */
    void setAmbientLight(std::uint8_t lower, std::uint8_t upper, bool sunlight);

    /**
     * @brief Creates a new level
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
     * @brief Performs a bucket fill of tiles starting from the given position
     *
     * @param level The level to fill on
     * @param layer The layer to fill on
     * @param position The position to fill from
     * @param id The tile to fill with
     * @param isAnim True to fill with animations, false with sprites
     */
    void fillTile(unsigned int level, unsigned int layer, const sf::Vector2i& position,
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
     * @brief Performs a bucket fill of collisions from the given starting position
     *
     * @param level The level to fill on
     * @param position The position to fill from
     * @param id The collision to fill with
     */
    void fillCollision(unsigned int level, const sf::Vector2i& position, core::map::Collision id);

    /**
     * @brief Sets a single catch tile
     *
     * @param level The level to modify
     * @param position The position of the catch to set
     * @param id The new catch value
     */
    void setCatch(unsigned int level, const sf::Vector2i& position, std::uint8_t id);

    /**
     * @brief Sets a range of catch tiles to a single value
     *
     * @param level The level to modify
     * @param area The region to set
     * @param id The value to set all tiles to
     */
    void setCatchArea(unsigned int level, const sf::IntRect& area, std::uint8_t id);

    /**
     * @brief Performs a bucket fill of catch tiles from the given position
     *
     * @param level The level to fill
     * @param position The position to fill from
     * @param id The id to fill with
     */
    void fillCatch(unsigned int level, const sf::Vector2i& position, std::uint8_t id);

    /**
     * @brief Tells whether or not the given id is in use
     *
     * @param id The id to check
     * @return True if unused, false if used
     */
    bool spawnIdUnused(unsigned int id) const;

    /**
     * @brief Adds a new player spawn to the map
     *
     * @param level The level to place the spawn on
     * @param tile The position the spawn is at
     * @param id The id of the spawn
     * @param dir The direction of the spawn
     */
    void addSpawn(unsigned int level, const sf::Vector2i& tile, unsigned int id,
                  bl::tmap::Direction dir);

    /**
     * @brief Rotates a player spawn
     *
     * @param level The level the spawn is on
     * @param tile The position the spawn is at
     */
    void rotateSpawn(unsigned int level, const sf::Vector2i& tile);

    /**
     * @brief Removes the spawn at the given position
     *
     * @param level The level the spawn is on
     * @param position The position the spawn is at
     */
    void removeSpawn(unsigned int level, const sf::Vector2i& position);

    /**
     * @brief Adds a character spawn to the map
     *
     * @param spawn The spawn to add
     */
    void addNpcSpawn(const core::map::CharacterSpawn& spawn);

    /**
     * @brief Get the Npc Spawn at the given position, if any
     *
     * @param The level the spawn is on
     * @param position The position to get the spawn at
     * @return const core::map::CharacterSpawn* The spawn or nullptr if none
     */
    const core::map::CharacterSpawn* getNpcSpawn(unsigned int level,
                                                 const sf::Vector2i& position) const;

    /**
     * @brief Edits an existing character spawn
     *
     * @param orig The address of the existing spawn
     * @param spawn The new spawn value
     */
    void editNpcSpawn(const core::map::CharacterSpawn* orig,
                      const core::map::CharacterSpawn& spawn);

    /**
     * @brief Removes a character spawn
     *
     * @param spawn The address of the spawn to remove
     */
    void removeNpcSpawn(const core::map::CharacterSpawn* spawn);

    /**
     * @brief Returns spawn info the item at the given position, if any
     *
     * @param level The level to search
     * @param position The position to check
     * @return std::pair<core::item::Id, bool> The item id and hidden state. Unknown if none
     */
    std::pair<core::item::Id, bool> getItem(unsigned int level, const sf::Vector2i& position);

    /**
     * @brief Spawns or modifies an item at the given position and level
     *
     * @param level The level for the item to be on
     * @param position The position to spawn at
     * @param item The item to spawn
     * @param visible True if the item should be visible, false for hidden
     */
    void addOrEditItem(unsigned int level, const sf::Vector2i& position, core::item::Id item,
                       bool visible);

    /**
     * @brief Removes the item from the given position if one is present
     *
     * @param level The level to search
     * @param position The position to clear
     */
    void removeItem(unsigned int level, const sf::Vector2i& position);

    /**
     * @brief Spawn or modify the light near the given position to the new size
     *
     * @param positionPixels The light radius in pixels
     * @param radius The position of the light in pixels
     */
    void setLight(const sf::Vector2i& positionPixels, unsigned int radius);

    /**
     * @brief Removes the light near the given position if one is there
     *
     * @param positionPixels The position of the light to remove, in pixels
     */
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

    /**
     * @brief Adds a new catch region to the map
     */
    void addCatchRegion();

    /**
     * @brief Returns a reference to all catch regions
     */
    const std::vector<core::map::CatchRegion>& catchRegions() const;

    /**
     * @brief Modifies the catch region at the given index
     *
     * @param index The index to modify
     * @param zone The new value
     */
    void editCatchRegion(std::uint8_t index, const core::map::CatchRegion& zone);

    /**
     * @brief Removes the catch region at the given index
     *
     * @param index The catch region to remove
     */
    void removeCatchRegion(std::uint8_t index);

    /**
     * @brief Adds a town
     */
    void addTown();

    /**
     * @brief Modifies an existing town
     *
     * @param i Index of the town to modify
     * @param town The new town value
     */
    void editTown(std::uint8_t i, const core::map::Town& town);

    /**
     * @brief Removes a town
     *
     * @param i Index of the town to remove
     */
    void removeTown(std::uint8_t i);

    /**
     * @brief Sets the town tile at the given position
     *
     * @param position The position of the tile to set
     * @param id The id of the town to set to
     */
    void setTownTile(const sf::Vector2i& position, std::uint8_t id);

    /**
     * @brief Sets a region of town tiles to the given town
     *
     * @param area The region to fill
     * @param id The town to fill with
     */
    void setTownTileArea(const sf::IntRect& area, std::uint8_t id);

    /**
     * @brief Performs a bucket fill of town tiles from the given position
     *
     * @param position The position to fill from
     * @param id The town to fill with
     */
    void fillTownTiles(const sf::Vector2i& position, std::uint8_t id);

    /**
     * @brief Sets the given level tile
     *
     * @param position The position to set
     * @param lt The level transition to occur on that tile
     */
    void setLevelTile(const sf::Vector2i& position, core::map::LevelTransition lt);

    /**
     * @brief Sets a selection of level tiles
     *
     * @param area The region to update
     * @param lt The level transition to populate the region with
     */
    void setLevelTileArea(const sf::IntRect& area, core::map::LevelTransition lt);

    /**
     * @brief Renders the map contents
     *
     * @param params Parameters for the rendering
     */
    void staticRender(const RenderMapWindow& params);

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

    struct EditCameraController : public bl::cam::CameraController2D {
        EditCameraController(EditMap* owner);
        virtual ~EditCameraController() = default;
        virtual void update(float dt) override;
        void reset(const sf::Vector2i& size);
        void zoom(float z);

        EditMap* owner;
        bool enabled;
        glm::vec2 mapSize;
        float zoomAmount;
    };

    const PositionCb clickCb;
    const PositionCb moveCb;
    const ActionCb actionCb;
    const ActionCb syncCb;
    EditCameraController* camera;
    bool controlsEnabled;
    std::string savefile;
    std::vector<bool> levelFilter;
    std::vector<std::vector<bool>> layerFilter;
    sf::IntRect selection;

    struct SpawnGraphics {
        bl::gfx::Sprite arrow;
        bl::gfx::Text label;
    };

    bl::gfx::Rectangle selectRect;
    bl::ctr::Vector2D<bl::gfx::BatchRectangle> townSquares;
    bl::gfx::BatchedShapes2D townSquareBatch;
    bl::gfx::VertexBuffer2D grid;
    std::unordered_map<unsigned int, SpawnGraphics> spawnSprites;

    RenderOverlay renderOverlay;
    unsigned int overlayLevel;
    unsigned int nextItemId;
    bool setRenderTarget;

    std::vector<bl::resource::Ref<sf::Texture>> colGfx;
    bl::resource::Ref<sf::Texture> arrowGfx;
    std::vector<bl::resource::Ref<sf::Texture>> ltGfx;

    EditMap(const PositionCb& cb, const PositionCb& moveCb, const ActionCb& actionCb,
            const ActionCb& syncCb, core::system::Systems& systems);
    bool doLoad(const std::string& file);
    bool editorActivate();
    void loadResources();
    void setupOverlay();

    void addSpawnGfx(const core::map::Spawn& spawn);
    void updateSpawnRotation(std::uint16_t id);
    void updateLayerVisibility(unsigned int level, unsigned int layer, bool hide);

    virtual sf::Vector2f minimumRequisition() const override;
    virtual bl::gui::rdr::Component* doPrepareRender(bl::gui::rdr::Renderer& renderer) override;
    virtual bool handleScroll(const bl::gui::Event& scroll) override;
    virtual void update(float dt) override;

    friend class page::Map;
    friend class page::Towns;
    friend class rdr::EditMapComponent;

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
    class FillTileAction;
    class SetCollisionAction;
    class SetCollisionAreaAction;
    class FillCollisionAction;
    class SetCatchAction;
    class SetCatchAreaAction;
    class FillCatchAction;
    class AddSpawnAction;
    class RotateSpawnAction;
    class RemoveSpawnAction;
    class AddNpcSpawnAction;
    class EditNpcSpawnAction;
    class RemoveNpcSpawnAction;
    class AddOrEditItemAction;
    class RemoveItemAction;
    class SetLightAction;
    class RemoveLightAction;
    class AddEventAction;
    class EditEventAction;
    class RemoveEventAction;
    class AddCatchRegionAction;
    class EditCatchRegionAction;
    class RemoveCatchRegionAction;
    class AddTownAction;
    class EditTownAction;
    class RemoveTownAction;
    class SetTownTileAction;
    class SetTownTileAreaAction;
    class FillTownTileAction;
    class SetLevelTileAction;
    class SetLevelTileAreaAction;
};

} // namespace component
} // namespace editor

#endif
