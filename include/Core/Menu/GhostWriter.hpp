#ifndef CORE_MENU_GHOSTWRITER_HPP
#define CORE_MENU_GHOSTWRITER_HPP

#include <string>
#include <string_view>

/**
 * @addtogroup Menu
 * @ingroup Core
 * @brief Collection of classes and utilities for menus
 *
 */

namespace core
{
/// Collection of classes and utilities for menus
namespace menu
{
/**
 * @brief Basic string wrapper that allows for text to be unhidden over time
 *
 * @ingroup Menu
 *
 */
class GhostWriter {
public:
    /**
     * @brief Construct a new GhostWriter with empty content
     *
     */
    GhostWriter();

    /**
     * @brief Construct a new GhostWriter with the given content
     *
     * @param content The string to show
     */
    GhostWriter(const std::string& content);

    /**
     * @brief Set the content to display. Resets to showing 0 characters
     *
     * @param content The string to slowly reveal
     */
    void setContent(const std::string& content);

    /**
     * @brief Returns the full content of the writer
     *
     */
    const std::string& getContent() const;

    /**
     * @brief Returns the visible portion of the content of the writer
     *
     */
    std::string_view getVisible() const;

    /**
     * @brief Updates the writer and unhides more characters based on time elapsed
     *
     * @param dt Time elapsed, in seconds
     */
    void update(float dt);

    /**
     * @brief Immediately shows all characters
     *
     */
    void showAll();

    /**
     * @brief Returns whether or not all characters are visible
     *
     */
    bool finished() const;

private:
    std::string content;
    unsigned int showing;
    float residual;
};

} // namespace menu
} // namespace core

#endif
