#ifndef EDITOR_PAGES_PEOPLEMON_HPP
#define EDITOR_PAGES_PEOPLEMON_HPP

#include <Core/Files/PeoplemonDB.hpp>
#include <Editor/Components/PeoplemonEditorWindow.hpp>
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
     * @brief Does nothing
     *
     * @param dt Time elapsed in seconds
     */
    virtual void update(float dt) override;

private:
    core::file::PeoplemonDB peoplemonDb;
    component::PeoplemonEditorWindow window;
    bl::gui::Button::Ptr saveBut;
    bl::gui::ScrollArea::Ptr rowArea;
    bool firstSync;

    void onChange();
    void makeDirty();
    void save();
    void newPeoplemon();
    void editPeoplemon(core::pplmn::Id move);
    void deletePeoplemon(core::pplmn::Id move);
    void sync();
    void reset();
};

} // namespace page
} // namespace editor

#endif
