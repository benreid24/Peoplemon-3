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

} // namespace component
} // namespace editor

#endif
