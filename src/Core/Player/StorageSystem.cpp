#include <Core/Player/StorageSystem.hpp>

namespace core
{
namespace player
{
StorageSystem::StorageSystem() {}

bool StorageSystem::add(const pplmn::OwnedPeoplemon& ppl) {
    for (int b = 0; b < BoxCount; ++b) {
        for (int x = 0; x < BoxWidth; ++x) {
            for (int y = 0; y < BoxHeight; ++y) {
                if (spaceFree(b, x, y)) {
                    boxes[b].emplace_back(ppl, b, sf::Vector2i(x, y));
                    boxes[b].back().peoplemon.heal();
                    return true;
                }
            }
        }
    }
    return false;
}

bool StorageSystem::spaceFree(int box, int x, int y) const {
    if (box < 0 || static_cast<unsigned int>(box) >= boxes.size()) return false;

    if (boxes[box].size() == BoxWidth * BoxHeight) return false;
    for (const auto& stored : boxes[box]) {
        if (stored.position.x == x && stored.position.y == y) return false;
    }
    return true;
}

} // namespace player
} // namespace core
