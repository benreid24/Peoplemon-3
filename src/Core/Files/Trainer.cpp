#include <Core/Files/Trainer.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
Trainer::Trainer()
: payout(0) {}

bool Trainer::save(const std::string& file) const {
    std::ofstream output(file.c_str());
    return bl::serial::json::Serializer<Trainer>::serializeStream(output, *this, 4, 0);
}

bool Trainer::load(const std::string& file, component::Direction spawnDir) {
    sourceFile = file;
    if (!TrainerManager::initializeExisting(file, *this)) return false;
    if (behavior.type() == Behavior::StandStill) { behavior.standing().facedir = spawnDir; }
    if (payout == 0) { payout = 40; }
    for (auto& ppl : peoplemon) { ppl.heal(); }
    return true;
}

bool Trainer::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<Trainer>::deserialize(input, *this);
}

bool Trainer::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<Trainer>::deserializeStream(input, *this);
}

} // namespace file
} // namespace core
