#ifndef EDITOR_EVENTS_MAPRENDER_HPP
#define EDITOR_EVENTS_MAPRENDER_HPP

/**
 * @addtogroup EditorEvents
 * @ingroup Editor
 * @brief Events fired by the editor
 */

namespace editor
{
namespace event
{
/**
 * @brief Fired when a map rendering is started
 *
 * @ingroup EditorEvents
 */
struct MapRenderStarted {};

/**
 * @brief Fired when a map rendering is complete
 *
 * @ingroup EditorEvents
 */
struct MapRenderCompleted {};

} // namespace event
} // namespace editor

#endif
