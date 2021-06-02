#ifndef EDITOR_PAGES_TESTING_HPP
#define EDITOR_PAGES_TESTING_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for creating and modifying game saves and launching the game to test work done
 *
 * @ingroup Pages
 *
 */
class Testing : public Page {
public:
    /**
     * @brief Construct a new Testing page
     *
     * @param systems The primary systems object
     */
    Testing(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Testing() = default;

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
