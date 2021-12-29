#ifndef CORE_MENUS_PEOPLEMONBUTTON_HPP
#define CORE_MENUS_PEOPLEMONBUTTON_HPP

#include <BLIB/Interfaces/Menu.hpp>
#include <Core/Peoplemon/OwnedPeoplemon.hpp>

namespace core
{
namespace menu
{
class PeoplemonButton : public bl::menu::Item {
public:
    using Ptr = std::shared_ptr<PeoplemonButton>;

    static Ptr create(const pplmn::OwnedPeoplemon& ppl);

private:
    bl::resource::Resource<sf::Texture>::Ref txtr;
    bl::resource::Resource<sf::Texture>::Ref faceTxtr;
    sf::Sprite image;
    sf::Sprite face;
    sf::RectangleShape hpBar;
    sf::Text name;
    sf::Text level;
    sf::Text item;
    sf::Text hpText;

    PeoplemonButton(const pplmn::OwnedPeoplemon& ppl);
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& pos) const override;
    virtual sf::Vector2f getSize() const override;
};

} // namespace menu
} // namespace core

#endif
