#include <Core/Player/Bag.hpp>

namespace core
{
namespace player
{
Bag::Item::Item()
: id(item::Id::Unknown)
, qty(0) {}

Bag::Item::Item(item::Id id, unsigned int qty)
: id(id)
, qty(qty) {}

void Bag::getByCategory(item::Category c, std::vector<Item>& result) const {
    result.clear();
    for (const Item& i : items) {
        if (item::Item::getCategory(i.id) == c) result.push_back(i);
    }
}

void Bag::getByType(item::Type t, std::vector<Item>& result) const {
    result.clear();
    for (const Item& i : items) {
        if (item::Item::getType(i.id) == t) result.push_back(i);
    }
}

void Bag::getAll(std::vector<Item>& result) const {
    result.clear();
    result.reserve(items.size());
    for (const Item& i : items) { result.push_back(i); }
}

unsigned int Bag::itemCount(item::Id id) const {
    const unsigned int i = find(id);
    return i < items.size() ? items[i].qty : 0;
}

bool Bag::hasItem(item::Id item) const { return itemCount(item) > 0; }

void Bag::addItem(item::Id id, unsigned int qty) {
    const unsigned int i = find(id);
    if (i < items.size()) { items[i].qty += qty; }
    else {
        items.emplace_back(id, qty);
    }
}

bool Bag::removeItem(item::Id id, unsigned int qty) {
    const unsigned int i = find(id);
    if (i < items.size()) {
        if (items[i].qty >= qty) {
            items[i].qty -= qty;
            if (items[i].qty == 0) items.erase(items.begin() + i);
            return true;
        }
    }
    return false;
}

void Bag::clear() { items.clear(); }

unsigned int Bag::find(item::Id id) const {
    for (unsigned int i = 0; i < items.size(); ++i) {
        if (items[i].id == id) return i;
    }
    return items.size();
}

} // namespace player
} // namespace core
