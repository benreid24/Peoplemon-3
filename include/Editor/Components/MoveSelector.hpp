#ifndef EDITOR_COMPONENTS_MOVESELECTOR_HPP
#define EDITOR_COMPONENTS_MOVESELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/MoveId.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>

namespace editor
{
namespace component
{
/**
 * @brief Combobox for selecting moves
 *
 * @ingroup EditorComponents
 *
 */
class MoveSelector : public bl::gui::Box {
public:
    /// Pointer to this component
    using Ptr = std::shared_ptr<MoveSelector>;

    /// Called when a move is selected
    using ChangeCb = std::function<void(core::pplmn::MoveId)>;

    /**
     * @brief Create a new MoveSelector
     *
     * @param enableFilter True to add radio buttons to filter on peoplemon, false to show all
     * @param changeCb Called when a move is selected
     * @return Ptr The new MoveSelector
     */
    static Ptr create(bool enableFilter, const ChangeCb& changeCb = {});

    /**
     * @brief Returns the currently selected move
     *
     * @return core::pplmn::MoveId The currently selected move
     */
    core::pplmn::MoveId currentMove() const;

    /**
     * @brief Set the currently selected move
     *
     * @param move The move to select
     */
    void setCurrentMove(core::pplmn::MoveId move);

    /**
     * @brief Notifies the selector of the peoplemon being selected for
     *
     * @param ppl The peoplemon to select for
     * @param level The level of the peoplemon
     */
    void notifyPeoplemon(core::pplmn::Id ppl, unsigned int level);

    /**
     * @brief Selects a random move
     *
     */
    void selectRandom();

    /**
     * @brief Refreshes the list of moves
     *
     */
    void refresh();

private:
    core::pplmn::Id peoplemon;
    unsigned int level;
    std::vector<core::pplmn::MoveId> validMoves;
    bl::gui::ComboBox::Ptr selector;
    bl::gui::RadioButton::Ptr noMoveFilter;
    bl::gui::RadioButton::Ptr levelMoveFilter;
    bl::gui::RadioButton::Ptr poolMoveFilter;

    MoveSelector(bool enableFilter, const ChangeCb& ccb);
};

} // namespace component
} // namespace editor

#endif
