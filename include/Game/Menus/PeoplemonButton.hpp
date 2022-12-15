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
    virtual sf::Vector2f getSize() const override;

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

private:
    sf::Color color;
    bool isSelected;
    bl::resource::Ref<sf::Texture> txtr;
    bl::resource::Ref<sf::Texture> faceTxtr;
    bl::resource::Ref<sf::Texture> ailTxtr;
    sf::Sprite image;
    sf::Sprite face;
    sf::RectangleShape hpBar;
    sf::Text name;
    sf::Text level;
    sf::Text item;
    sf::Text hpText;
    sf::Texture blank;
    sf::Sprite ailment;
    float hpBarTarget;

    PeoplemonButton(const core::pplmn::OwnedPeoplemon& ppl);
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& pos) const override;
    sf::Texture& ailmentTexture(core::pplmn::Ailment ail);
};

} // namespace menu
} // namespace game

#endif
