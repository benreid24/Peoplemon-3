#ifndef EDITOR_PAGES_ITEMS_HPP
#define EDITOR_PAGES_ITEMS_HPP

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
class Items : public Page {
public:
    /**
     * @brief Construct a new Items page
     *
     * @param systems The primary systems object
     */
    Items(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Items() = default;

    /**
     * @brief Returns "Items"
     *
     */
    virtual const char* title() const override;

    /**
     * @brief Returns the content of the page itself
     *
     */
    virtual bl::gui::Box::Ptr content() override;

    /**
     * @brief Does nothing
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    bl::gui::Box::Ptr content;
    // TODO
};

} // namespace page
} // namespace editor

#endif
