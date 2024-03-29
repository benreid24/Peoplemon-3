#ifndef GAME_MENUS_STORAGECURSOR_HPP
#define GAME_MENUS_STORAGECURSOR_HPP

#include <BLIB/Graphics.hpp>
#include <BLIB/Resources.hpp>
#include <Core/Input/Control.hpp>
#include <Core/Peoplemon/Id.hpp>
#include <SFML/Graphics.hpp>

namespace game
{
namespace menu
{
/**
 * @brief Cursor that is used to navigate the storage system boxes
 *
 * @ingroup Menus
 *
 */
class StorageCursor {
public:
    /**
     * @brief Construct a new Storage Cursor
     *
     */
    StorageCursor();

    /**
     * @brief Updates the cursor motion
     *
     * @param dt Time elapsed in seconds
     */
    void update(float dt);

    /**
     * @brief Processes user input to move the cursor
     *
     * @param cmd The user input
     * @param skipAnim True to skip the rest of an in-progress animation and move right away
     * @return True if the cursor moved, false otherwise
     */
    bool process(core::input::EntityControl cmd, bool skipAnim);

    /**
     * @brief Sets a Peoplemon to render with the cursor. Pass Unknown to clear
     *
     * @param peoplemon The peoplemon thumbnail to hold
     */
    void setHolding(core::pplmn::Id peoplemon);

    /**
     * @brief Returns whether or not the cursor position is being interpolated
     *
     */
    bool moving() const;

    /**
     * @brief Returns the position of the cursor in the box
     *
     */
    const sf::Vector2i& getPosition() const;

    /**
     * @brief Sets the x position of the cursor
     *
     * @param x The new x position
     */
    void setX(int x);

    /**
     * @brief Updates the cursor position if the box to the left is switched to
     *
     */
    void pageLeft();

    /**
     * @brief Updates the cursor position if the box to the right is switched to
     *
     */
    void pageRight();

    /**
     * @brief Renders the cursor
     *
     * @param target The target to render to
     */
    void render(sf::RenderTarget& target) const;

    /**
     * @brief Returns the size of a square on the grid
     *
     */
    static float TileSize();

private:
    sf::Vector2i position;
    bl::rc::res::TextureRef cursorTxtr;
    bl::gfx::Sprite cursor;
    bl::rc::res::TextureRef pplTxtr;
    bl::gfx::Sprite peoplemon;
    core::input::EntityControl moveDir;
    float offset;
    float moveVel;

    static float size;

    void syncPos();
};

} // namespace menu
} // namespace game

#endif
