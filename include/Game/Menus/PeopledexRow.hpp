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
    virtual sf::Vector2f getSize() const override;

protected:
    virtual void render(sf::RenderTarget& target, sf::RenderStates states,
                        const sf::Vector2f& position) const override;

private:
    bl::resource::Resource<sf::Texture>::Ref bgndTxtr;
    bl::resource::Resource<sf::Texture>::Ref activeBgndTxtr;
    sf::Sprite background;

    bl::resource::Resource<sf::Texture>::Ref ballTxtr;
    sf::Sprite ball;

    sf::Text id;
    sf::Text name;

    PeopledexRow(core::pplmn::Id ppl, const core::player::Peopledex& dex);
    void makeActive();
    void makeInactive();
};

} // namespace menu
} // namespace game

#endif
