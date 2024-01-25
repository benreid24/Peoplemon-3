#include <Core/Player/StorageSystem.hpp>

#include <algorithm>

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

pplmn::StoredPeoplemon* StorageSystem::move(pplmn::StoredPeoplemon& ppl, unsigned int newBox,
                                            const sf::Vector2i& newPos) {
    // find peoplemon being moved
    const auto it = std::find_if(
        boxes[ppl.boxNumber].begin(),
        boxes[ppl.boxNumber].end(),
        [&ppl](const pplmn::StoredPeoplemon& cmp) { return ppl.position == cmp.position; });
    if (it == boxes[ppl.boxNumber].end()) {
        BL_LOG_CRITICAL << "Could not find Peoplemon to move at position: " << ppl.position;
        return nullptr;
    }

    // handle swap
    const auto oit = std::find_if(
        boxes[newBox].begin(), boxes[newBox].end(), [&newPos](const pplmn::StoredPeoplemon& cmp) {
            return newPos == cmp.position;
        });
    if (oit != boxes[newBox].end()) {
        std::swap(it->peoplemon, oit->peoplemon);
        return &*oit;
    }

    // move position
    ppl.position = newPos;

    // change box if different
    if (newBox != ppl.boxNumber) {
        const unsigned int ogBox = ppl.boxNumber;
        ppl.boxNumber            = newBox;
        boxes[newBox].emplace_back(ppl);
        boxes[ogBox].erase(it);
        return &boxes[newBox].back();
    }

    return &*it;
}

bool StorageSystem::spaceFree(int box, int x, int y) const {
    if (box < 0 || static_cast<unsigned int>(box) >= boxes.size()) return false;

    if (boxes[box].size() == BoxWidth * BoxHeight) return false;
    for (const auto& stored : boxes[box]) {
        if (stored.position.x == x && stored.position.y == y) return false;
    }
    return true;
}

pplmn::StoredPeoplemon* StorageSystem::get(unsigned int box, const sf::Vector2i& pos) {
    for (auto& ppl : boxes[box]) {
        if (ppl.position == pos) { return &ppl; }
    }
    return nullptr;
}

const std::vector<pplmn::StoredPeoplemon>& StorageSystem::getBox(unsigned int box) const {
    return boxes[box];
}

} // namespace player
} // namespace core
