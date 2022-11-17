#include <Core/Systems/Scripts.hpp>

#include <Core/Files/GameSave.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace system
{
Scripts::Scripts(Systems& s)
: owner(s) {}

void Scripts::init() { bl::event::Dispatcher::subscribe(this); }

const bl::script::Value* Scripts::getEntry(const std::string& name) const {
    const auto it = entries.find(name);
    return it != entries.end() ? &it->second : nullptr;
}

void Scripts::setEntry(const std::string& name, const bl::script::Value& val) {
    entries.emplace(name, val);
}

void Scripts::observe(const event::GameSaveInitializing& save) {
    save.gameSave.scripts.entries = &entries;
}

} // namespace system
} // namespace core
