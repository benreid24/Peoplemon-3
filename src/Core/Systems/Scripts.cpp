#include <Core/Systems/Scripts.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Scripts::Scripts(Systems& s)
: owner(s) {}

void Scripts::init() { owner.engine().eventBus().subscribe(this); }

const bl::script::Value* Scripts::getEntry(const std::string& name) const {
    const auto it = entries.find(name);
    return it != entries.end() ? &it->second : nullptr;
}

void Scripts::setEntry(const std::string& name, const bl::script::Value& val) {
    entries.emplace(name, val);
}

void Scripts::observe(const event::GameSaving& save) {
    bl::serial::json::Serializer<Scripts>::serializeInto(save.saveData, "scripting", *this);
}

void Scripts::observe(const event::GameLoading& save) {
    if (!bl::serial::json::Serializer<Scripts>::deserializeFrom(
            save.saveData, "scripting", *this)) {
        save.failMessage = "Failed to load script data";
    }
}

} // namespace system
} // namespace core
