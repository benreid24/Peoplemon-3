#ifndef EDITOR_PAGES_MOVES_HPP
#define EDITOR_PAGES_MOVES_HPP

#include <Editor/Pages/Page.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/MoveDB.hpp>
#include <Editor/Components/MoveEditorWindow.hpp>

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
    core::file::MoveDB moveDb;
    component::MoveEditorWindow moveWindow;
    bl::gui::Button::Ptr saveBut;
    bl::gui::ScrollArea::Ptr rowArea;

    void onChange();
    void makeDirty();
    void save();
    void newMove();
    void editMove(core::pplmn::MoveId move);
    void deleteMove(core::pplmn::MoveId move);
    void sync();
    void reset();
};

} // namespace page
} // namespace editor

#endif
