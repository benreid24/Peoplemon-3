#ifndef CORE_EVENTS_STORAGESYSTEM_HPP
#define CORE_EVENTS_STORAGESYSTEM_HPP

namespace core
{
namespace event
{
/**
 * @brief Fired when the storage system should be opened
 *
 * @ingroup Events
 *
 */
struct StorageSystemOpened {};

/**
 * @brief Fired when the storage system is closed
 *
 * @ingroup Events
 *
 */
struct StorageSystemClosed {};

} // namespace event
} // namespace core

#endif
