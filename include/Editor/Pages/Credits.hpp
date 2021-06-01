#ifndef EDITOR_PAGES_CREDITS_HPP
#define EDITOR_PAGES_CREDITS_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for editing the credits
 *
 * @ingroup Pages
 *
 */
class Credits : public Page {
public:
    /**
     * @brief Construct a new Credits page
     *
     * @param systems The primary systems object
     */
    Credits(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Credits() = default;

    /**
     * @brief Returns "Credits"
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
