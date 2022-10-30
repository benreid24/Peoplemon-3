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
 *
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
     *
     */
    virtual ~MoveInfoRow() = default;

    /**
     * @brief Returns the size of the menu item
     *
     */
    virtual sf::Vector2f getSize() const override;

protected:
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    bl::resource::Resource<sf::Texture>::Ref activeBgndTxtr;
    sf::Sprite background;
    sf::Text name;

    MoveInfoRow(core::pplmn::MoveId move);
    void makeActive();
    void makeInactive();
};

} // namespace menu
} // namespace game

#endif
