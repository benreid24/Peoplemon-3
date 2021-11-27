#ifndef EDITOR_COMPONENTS_MOVESELECTOR_HPP
#define EDITOR_COMPONENTS_MOVESELECTOR_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Peoplemon/MoveId.hpp>

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
class MoveSelector : public bl::gui::ComboBox {
public:
    /// Pointer to this component
    using Ptr = std::shared_ptr<MoveSelector>;

    /// Called when a move is selected
    using ChangeCb = std::function<void(core::pplmn::MoveId)>;

    /**
     * @brief Create a new MoveSelector
     *
     * @param changeCb Called when a move is selected
     * @return Ptr The new MoveSelector
     */
    static Ptr create(const ChangeCb& changeCb = {});

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
     * @brief Refreshes the list of available moves from the move database
     *
     */
    void refresh();

private:
    static std::vector<core::pplmn::MoveId> idLookup;

    MoveSelector(const ChangeCb& ccb);
};

} // namespace component
} // namespace editor

#endif
