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

private:
    sf::Color color;
    bool isSelected;
    bl::resource::Resource<sf::Texture>::Ref txtr;
    bl::resource::Resource<sf::Texture>::Ref faceTxtr;
    sf::Sprite image;
    sf::Sprite face;
    sf::RectangleShape hpBar;
    sf::Text name;
    sf::Text level;
    sf::Text item;
    sf::Text hpText;

    PeoplemonButton(const core::pplmn::OwnedPeoplemon& ppl);
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& pos) const override;
    virtual sf::Vector2f getSize() const override;
};

} // namespace menu
} // namespace game

#endif
