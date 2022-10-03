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

void StorageSystem::add(unsigned int box, const sf::Vector2i& pos,
                        const pplmn::OwnedPeoplemon& ppl) {
    boxes[box].emplace_back(ppl, box, pos);
    boxes[box].back().peoplemon.heal();
}

void StorageSystem::remove(unsigned int b, const sf::Vector2i& pos) {
    auto& box = boxes[b];
    for (unsigned int i = 0; i < box.size(); ++i) {
        if (box[i].position == pos) {
            box.erase(box.begin() + i);
            --i;
        }
    }
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
