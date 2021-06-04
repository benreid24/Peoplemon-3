#ifndef EDITOR_PAGES_PLAYLISTS_HPP
#define EDITOR_PAGES_PLAYLISTS_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for editing item names, descriptions, and values
 *
 * @ingroup Pages
 *
 */
class Playlists : public Page {
public:
    /**
     * @brief Construct a new Playlists page
     *
     * @param systems The primary systems object
     */
    Playlists(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Playlists() = default;

    /**
     * @brief Does nothing
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
