#ifndef GAME_MENUS_PEOPLEMONBUTTON_HPP
#define GAME_MENUS_PEOPLEMONBUTTON_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Menu item for peoplemon
 *
 * @ingroup Menus
 *
 */
class PeoplemonButton : public bl::menu::Item {
public:
    using Ptr = std::shared_ptr<PeoplemonButton>;

    /**
     * @brief Destroy the Peoplemon Button object
     *
     */
    virtual ~PeoplemonButton() = default;

    /**
     * @brief Creates a new peoplemon button from the peoplemon
     *
     * @param ppl The peoplemon to represent
     * @return Ptr The new item
     */
    static Ptr create(const core::pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Set the highlight color
     *
     */
    void setHighlightColor(const sf::Color& color);

    /**
     * @brief Returns the size of the button
     *
     */
    virtual glm::vec2 getSize() const override;

    /**
     * @brief Syncs the HP bar and ailment texture with the peoplemon
     *
     * @param ppl The peoplemon to sync with
     */
    void sync(const core::pplmn::OwnedPeoplemon& ppl);

    /**
     * @brief Updates the HP bar
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Returns whether or not this button is synced
     *
     */
    bool synced() const;

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
    const core::pplmn::OwnedPeoplemon& ppl;
    bl::engine::Engine* enginePtr;
    bl::rc::Overlay* overlay;
    sf::Color color;
    bool isSelected;
    bl::rc::res::TextureRef txtr;
    bl::rc::res::TextureRef faceTxtr;
    bl::rc::res::TextureRef ailTxtr;
    bl::gfx::Sprite image;
    bl::gfx::Sprite face;
    bl::gfx::Rectangle hpBar;
    bl::gfx::Text name;
    bl::gfx::Text level;
    bl::gfx::Text item;
    bl::gfx::Text hpText;
    bl::gfx::Sprite ailment;
    float hpBarTarget;

    PeoplemonButton(const core::pplmn::OwnedPeoplemon& ppl);
    void updateAilment(core::pplmn::Ailment ail);
};

} // namespace menu
} // namespace game

#endif
