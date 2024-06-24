#ifndef GAME_MENUS_PEOPLEDEXROW_HPP
#define GAME_MENUS_PEOPLEDEXROW_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Player/Peopledex.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Item row for peoplemon in the peopledex
 *
 * @ingroup Menus
 *
 */
class PeopledexRow : public bl::menu::Item {
public:
    /// @brief Pointer to the row item
    using Ptr = std::shared_ptr<PeopledexRow>;

    /**
     * @brief Create a new peopledex row
     *
     * @param ppl The peoplemon to put in the peopledex
     * @param dex The Peopledex
     * @return Ptr The new row
     */
    static Ptr create(core::pplmn::Id ppl, const core::player::Peopledex& dex);

    /**
     * @brief Destroy the Peopledex Row object
     *
     */
    virtual ~PeopledexRow() = default;

    /**
     * @brief Returns the size that the row takes up
     *
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
    virtual void doSceneAdd(bl::rc::Scene* overlay) override;

    /**
     * @brief Called when the item should be removed from the overlay
     */
    virtual void doSceneRemove() override;

    /**
     * @brief Returns the entity (or top level entity) of the item
     */
    virtual bl::ecs::Entity getEntity() const override;

    /**
     * @brief Renders the item
     *
     * @param ctx The render context
     */
    virtual void draw(bl::rc::scene::CodeScene::RenderContext& ctx) override;

private:
    const core::pplmn::Id ppl;
    const core::player::Peopledex& dex;

    bl::rc::res::TextureRef bgndTxtr;
    bl::rc::res::TextureRef activeBgndTxtr;
    bl::gfx::Sprite background;

    bl::rc::res::TextureRef ballTxtr;
    bl::gfx::Sprite ball;

    bl::gfx::Text id;
    bl::gfx::Text name;

    PeopledexRow(core::pplmn::Id ppl, const core::player::Peopledex& dex);
    void makeActive();
    void makeInactive();
};

} // namespace menu
} // namespace game

#endif
