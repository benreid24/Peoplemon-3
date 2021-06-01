#ifndef EDITOR_PAGES_PEOPLEMON_HPP
#define EDITOR_PAGES_PEOPLEMON_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Page for editing Peoplemon metadata
 *
 * @ingroup Pages
 *
 */
class Peoplemon : public Page {
public:
    /**
     * @brief Construct a new Peoplemon page
     *
     * @param systems The primary systems object
     */
    Peoplemon(core::system::Systems& systems);

    /**
     * @brief Destroy the Page
     *
     */
    virtual ~Peoplemon() = default;

    /**
     * @brief Returns "Peoplemon"
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
