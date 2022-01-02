#ifndef CORE_SYSTEMS_SCRIPTS_HPP
#define CORE_SYSTEMS_SCRIPTS_HPP

#include <BLIB/Events.hpp>
#include <BLIB/Scripts.hpp>
#include <BLIB/Serialization/JSON.hpp>
#include <Core/Events/GameSave.hpp>
#include <unordered_map>

namespace core
{
namespace system
{
class Systems;

/**
 * @brief System that stores and handles saving/loading of save entries for scripts
 *
 * @ingroup Systems
 *
 */
class Scripts : public bl::event::Listener<event::GameSaving, event::GameLoading> {
public:
    /**
     * @brief Initializes the scripting system
     *
     * @param owner The main game systems
     */
    Scripts(Systems& owner);

    /**
     * @brief Subscribes to the game event bus
     *
     */
    void init();

    /**
     * @brief Returns the save entry with the given name. May be null if not found
     *
     * @param name The name of the save entry to get
     * @return const bl::script::Value* The value if found, or nullptr if not
     */
    const bl::script::Value* getEntry(const std::string& name) const;

    /**
     * @brief Sets a save entry
     *
     * @param name The name to set
     * @param val The value to set to
     */
    void setEntry(const std::string& name, const bl::script::Value& val);

private:
    Systems& owner;
    std::unordered_map<std::string, bl::script::Value> entries;

    virtual void observe(const event::GameLoading& save) override;
    virtual void observe(const event::GameSaving& save) override;

    friend class bl::serial::json::SerializableObject<Scripts>;
};

} // namespace system
} // namespace core

namespace bl
{
namespace serial
{
namespace json
{
template<>
struct SerializableObject<core::system::Scripts> : public SerializableObjectBase {
    using S = core::system::Scripts;
    using M = std::unordered_map<std::string, bl::script::Value>;

    SerializableField<S, M> entries;

    SerializableObject()
    : entries("saveEntries", *this, &S::entries) {}
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
