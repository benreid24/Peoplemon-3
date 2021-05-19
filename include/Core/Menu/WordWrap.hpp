#ifndef CORE_MENU_WORDWRAP_HPP
#define CORE_MENU_WORDWRAP_HPP

#include <SFML/Graphics.hpp>

namespace core
{
namespace menu
{
/**
 * @brief Basic helper function to wrap text to fit within a specified maximum width
 *
 * @param text The text to wrap. Must be initialized with font and size. Modifed in place
 * @param maxWidth The width to constrain the text to
 */
void wordWrap(sf::Text& text, float maxWidth);

} // namespace menu
} // namespace core

#endif
