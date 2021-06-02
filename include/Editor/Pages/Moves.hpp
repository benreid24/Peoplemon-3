#ifndef EDITOR_PAGES_MOVES_HPP
#define EDITOR_PAGES_MOVES_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for editing move metadata
 *
 * @ingroup Moves
 *
 */
class Moves : public Page {
public:
    /**
     * @brief Construct a new Moves page
     *
     * @param systems The primary systems object
     */
    Moves(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Moves() = default;

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
