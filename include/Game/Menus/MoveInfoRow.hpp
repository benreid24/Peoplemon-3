#ifndef GAME_MENUS_MOVEINFOROW_HPP
#define GAME_MENUS_MOVEINFOROW_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Peoplemon/MoveId.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Menu row item for moves on the peoplemon info screen
 *
 * @ingroup Menus
 */
class MoveInfoRow : public bl::menu::Item {
public:
    /**
     * @brief Create a new move row
     *
     * @param move The move to represent
     * @return bl::menu::Item::Ptr The new row
     */
    static bl::menu::Item::Ptr create(core::pplmn::MoveId move);

    /**
     * @brief Destroy the Move Info Row object
     */
    virtual ~MoveInfoRow() = default;

    /**
     * @brief Returns the size of the menu item
     */
    virtual glm::vec2 getSize() const override;

protected:
    /**
     * @brief Called at least once when the item is added to a menu. Should create required graphics
     *        primitives and return the transform to use
     *
     * @param engine The game engine instance
     */
    virtual void doCreate(bl::engine::Engine& engine) override;

    /**
     * @brief Called when the item should be added to the overlay
     *
     * @param overlay The overlay to add to
     */
    virtual void doSceneAdd(bl::rc::Overlay* overlay) override;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual bl::ecs::Entity getEntity() const override;

private:
    const core::pplmn::MoveId move;
    bl::rc::res::TextureRef bgndTxtr;
    bl::rc::res::TextureRef activeBgndTxtr;
    bl::gfx::Sprite background;
    bl::gfx::Text name;

    MoveInfoRow(core::pplmn::MoveId move);
    void makeActive();
    void makeInactive();
};

} // namespace menu
} // namespace game

#endif
