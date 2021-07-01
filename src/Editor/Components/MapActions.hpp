#ifndef EDITOR_COMPONENTS_MAPACTIONS_HPP
#define EDITOR_COMPONENTS_MAPACTIONS_HPP

#include <Editor/Components/EditMap.hpp>

namespace editor
{
namespace component
{
class EditMap::SetTileAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, unsigned int layer,
                                       const sf::Vector2i& pos, bool isAnim,
                                       core::map::Tile::IdType value, const EditMap& map);

    virtual ~SetTileAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const unsigned int layer;
    const sf::Vector2i position;
    const core::map::Tile::IdType prev;
    const bool wasAnim;
    const core::map::Tile::IdType updated;
    const bool isAnim;

    SetTileAction(unsigned int level, unsigned int layer, const sf::Vector2i& pos,
                  core::map::Tile::IdType prev, bool wasAnim, core::map::Tile::IdType value,
                  bool isAnim);
};

class EditMap::SetTileAreaAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, unsigned int layer,
                                       const sf::IntRect& area, bool isAnim,
                                       core::map::Tile::IdType value, const EditMap& map);

    virtual ~SetTileAreaAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const unsigned int layer;
    const sf::IntRect area;
    const bl::container::Vector2D<core::map::Tile::IdType> prev;
    const bl::container::Vector2D<std::uint8_t> wasAnim;
    const core::map::Tile::IdType updated;
    const bool isAnim;
    const int w;
    const int h;

    SetTileAreaAction(unsigned int level, unsigned int layer, const sf::IntRect& area,
                      bl::container::Vector2D<core::map::Tile::IdType>&& prev,
                      bl::container::Vector2D<std::uint8_t>&& wasAnim,
                      core::map::Tile::IdType value, bool isAnim, int w, int h);
};

class EditMap::SetCollisionAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, const sf::Vector2i& pos,
                                       core::map::Collision value, const EditMap& map);

    virtual ~SetCollisionAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const sf::Vector2i pos;
    const core::map::Collision value;
    const core::map::Collision ogVal;

    SetCollisionAction(unsigned int level, const sf::Vector2i& pos, core::map::Collision value,
                       core::map::Collision ogVal);
};

class EditMap::SetCollisionAreaAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, const sf::IntRect& area,
                                       core::map::Collision value, const EditMap& map);

    virtual ~SetCollisionAreaAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const sf::IntRect area;
    const core::map::Collision value;
    const bl::container::Vector2D<core::map::Collision> ogVals;

    SetCollisionAreaAction(unsigned int level, const sf::IntRect& area, core::map::Collision value,
                           bl::container::Vector2D<core::map::Collision>&& ogcols);
};

class EditMap::SetPlaylistAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(const std::string& playlist, const EditMap& editMap);

    virtual ~SetPlaylistAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const std::string orig;
    const std::string playlist;

    SetPlaylistAction(const std::string& orig, const std::string& playlist);
};

class EditMap::SetWeatherAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(core::map::Weather::Type type, const EditMap& map);

    virtual ~SetWeatherAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const core::map::Weather::Type type;
    const core::map::Weather::Type orig;

    SetWeatherAction(core::map::Weather::Type type, core::map::Weather::Type orig);
};

class EditMap::AppendLayerAction : public EditMap::Action {
public:
    enum Location { Bottom, YSort, Top };

    static EditMap::Action::Ptr create(unsigned int level, Location location);

    virtual ~AppendLayerAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const Location location;

    AppendLayerAction(unsigned int level, Location location);
};

class EditMap::RemoveLayerAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, unsigned int layer, const EditMap& map);

    virtual ~RemoveLayerAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const unsigned int layer;
    const core::map::TileLayer removedLayer;

    RemoveLayerAction(unsigned int level, unsigned int layer,
                      const core::map::TileLayer& removedLayer);
};

class EditMap::ShiftLayerAction : public EditMap::Action {
public:
    static EditMap::Action::Ptr create(unsigned int level, unsigned int layer, bool up);

    virtual ~ShiftLayerAction() = default;
    virtual bool apply(EditMap& map) override;
    virtual bool undo(EditMap& map) override;
    virtual const char* description() const override;

private:
    const unsigned int level;
    const unsigned int layer;
    const bool up;

    ShiftLayerAction(unsigned int level, unsigned int layer, bool up);
};

} // namespace component
} // namespace editor

#endif
