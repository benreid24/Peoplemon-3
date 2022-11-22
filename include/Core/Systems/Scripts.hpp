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
class Scripts : public bl::event::Listener<event::GameSaveInitializing> {
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

    virtual void observe(const event::GameSaveInitializing& save) override;

    friend struct bl::serial::SerializableObject<Scripts>;
};

} // namespace system
} // namespace core

#endif
