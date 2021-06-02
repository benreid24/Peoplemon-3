#ifndef EDITOR_PAGES_MAP_HPP
#define EDITOR_PAGES_MAP_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for map editing. Tiles, levels, events, NPC's, weather, lighting, everything
 *
 * @ingroup Pages
 *
 */
class Map : public Page {
public:
    /**
     * @brief Construct a new Map page
     *
     * @param systems The primary systems object
     */
    Map(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Map() = default;

    /**
     * @brief Updates the game world and systems
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    // TODO
};

} // namespace page
} // namespace editor

#endif
