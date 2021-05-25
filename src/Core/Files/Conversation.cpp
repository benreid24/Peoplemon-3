#include <Core/Files/Conversation.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace file
{
namespace
{
void shiftUpJumps(std::vector<Conversation::Node>& nodes, unsigned int i) {
    for (Conversation::Node& node : nodes) {
        if (node.nextOnPass() >= i) node.nextOnPass() += 1;
        if (node.nextOnReject() >= i) node.nextOnReject() += 1;

        if (node.getType() == Conversation::Node::Prompt) {
            for (auto& pair : node.choices()) {
                if (pair.second >= i) pair.second += 1;
            }
        }
    }
}

void shiftDownJumps(std::vector<Conversation::Node>& nodes, unsigned int i) {
    for (Conversation::Node& node : nodes) {
        // Invalidate or shift down
        if (node.nextOnReject() == i)
            node.nextOnReject() = nodes.size();
        else if (node.nextOnReject() > i)
            node.nextOnReject() -= 1;

        if (node.nextOnPass() == i)
            node.nextOnPass() = nodes.size();
        else if (node.nextOnPass() > i)
            node.nextOnPass() -= 1;

        // Choices
        if (node.getType() == Conversation::Node::Prompt) {
            for (auto& pair : node.choices()) {
                // Invalidate or shift down
                if (pair.second == i)
                    pair.second = nodes.size();
                else if (pair.second > i)
                    pair.second -= 1;
            }
        }
    }
}
} // namespace

namespace loader
{
struct LegacyConversationLoader : public bl::file::binary::VersionedPayloadLoader<Conversation> {
    virtual bool read(Conversation& result, bl::file::binary::File& input) const override {
        std::vector<Conversation::Node>& nodes = result.cnodes.getValue();
        nodes.clear();

        // Data for legacy conversaion
        std::unordered_map<std::string, unsigned int> labelPoints;
        std::vector<std::pair<std::string, std::uint32_t*>> namedJumps;

        // Load legacy data
        std::uint16_t nodeCount = 0;
        if (!input.read(nodeCount)) return false;
        nodes.reserve(nodeCount);
        for (std::uint16_t i = 0; i < nodeCount; ++i) {
            Conversation::Node node;

            std::uint8_t type;
            if (!input.read(type)) return false;

            switch (type) {
            case 't':
                node.setType(Conversation::Node::Talk);

                if (!input.read(node.message())) return false;
                node.next() = i + 1;
                nodes.push_back(node);
                break;

            case 'o': {
                node.setType(Conversation::Node::Prompt);
                nodes.push_back(node);
                Conversation::Node& n = nodes.back();

                std::uint16_t choiceCount;
                if (!input.read(node.message())) return false;
                if (!input.read(choiceCount)) return false;
                n.choices().resize(choiceCount);
                for (std::uint16_t j = 0; j < choiceCount; ++j) {
                    std::string choice, jump;
                    if (!input.read(n.choices()[j].first)) return false;
                    if (!input.read(jump)) return false;
                    namedJumps.push_back(std::make_pair(jump, &n.choices()[i].second));
                }

            } break;

            case 'j':
                node.setType(Conversation::Node::_LEGACY_Jump);
                if (!input.read(node.message())) return false;
                nodes.push_back(node);
                namedJumps.push_back(std::make_pair(node.message(), &nodes.back().next()));
                break;

            case 'z':
                node.setType(Conversation::Node::RunScript);
                if (!input.read(node.script())) return false;
                node.next() = i + 1;
                nodes.push_back(node);
                break;

            case 'r': {
                std::uint8_t t;
                std::uint16_t d;
                if (!input.read(t)) return false;
                if (!input.read(d)) return false;
                if (!input.read(node.message())) return false;
                if (t == 0) { // take money
                    node.setType(Conversation::Node::TakeMoney);
                    node.money() = d;
                }
                else { // take item
                    node.setType(Conversation::Node::TakeItem);
                    node.item() = item::Item::cast(d);
                }
                node.next() = i + 1;
            } break;

            case 'g': {
                std::uint8_t t;
                std::uint16_t d;
                if (!input.read(t)) return false;
                if (!input.read(d)) return false;
                if (t == 0) { // give money
                    node.setType(Conversation::Node::GiveMoney);
                    node.money() = d;
                }
                else { // give item
                    node.setType(Conversation::Node::GiveItem);
                    node.item() = item::Item::cast(d);
                }
                node.next() = i + 1;
            } break;

            case 's':
                node.setType(Conversation::Node::SetSaveFlag);
                if (!input.read(node.saveFlag())) return false;
                node.next() = i + 1;
                break;

            case 'c': {
                std::string jump;
                node.setType(Conversation::Node::CheckSaveFlag);
                if (!input.read(jump)) return false;
                if (!input.read(node.saveFlag())) return false;
                node.nextOnPass() = i + 1;
                nodes.push_back(node);
                namedJumps.push_back(std::make_pair(jump, &nodes.back().nextOnReject()));
            } break;

            case 'l':
                if (!input.read(node.message())) return false;
                labelPoints.insert(std::make_pair(node.message(), i));
                break;

            case 'w': {
                node.setType(Conversation::Node::CheckInteracted);
                node.nextOnPass() = i + 1;
                nodes.push_back(node);
                std::string jump;
                if (!input.read(jump)) return false;
                namedJumps.push_back(std::make_pair(jump, &nodes.back().nextOnReject()));
            } break;

            default:
                BL_LOG_ERROR << "Unknown legacy conversation node type: " << type;
                return false;
            }
        }

        // Resolve named jumps
        for (auto& pair : namedJumps) {
            auto it = labelPoints.find(pair.first);
            if (it != labelPoints.end()) { *pair.second = it->second; }
            else {
                if (pair.first != "end") {
                    BL_LOG_WARN << "Invalid jump '" << pair.first << "' in conversation '"
                                << input.filename() << "', jumping to end";
                }
                *pair.second = nodes.size();
            }
        }

        // Remove legacy jump nodes
        const auto connectJump = [&nodes](unsigned int orig, unsigned int next) {
            for (Conversation::Node& node : nodes) {
                if (node.nextOnPass() == orig)
                    node.nextOnPass() = next;
                else if (node.nextOnPass() > orig)
                    node.nextOnPass() -= 1;
                if (node.nextOnReject() == orig)
                    node.nextOnReject() = next;
                else if (node.nextOnReject() > orig)
                    node.nextOnReject() -= 1;

                if (node.getType() == Conversation::Node::Prompt) {
                    for (auto& pair : node.choices()) {
                        if (pair.second == orig)
                            pair.second = next;
                        else if (pair.second > orig)
                            pair.second -= orig;
                    }
                }
            }
        };

        for (unsigned int i = 0; i < nodes.size(); ++i) {
            if (nodes[i].getType() == Conversation::Node::_LEGACY_Jump) {
                const unsigned int next = nodes[i].next();
                nodes.erase(nodes.begin() + i);
                connectJump(i, next);
                --i;
            }
        }

        return true;
    }

    virtual bool write(const Conversation&, bl::file::binary::File&) const override {
        // not implemented
        return false;
    }
};

struct ConversationLoader : public bl::file::binary::VersionedPayloadLoader<Conversation> {
    virtual bool read(Conversation& result, bl::file::binary::File& input) const override {
        return result.deserialize(input);
    }

    virtual bool write(const Conversation& conversation,
                       bl::file::binary::File& output) const override {
        return conversation.serialize(output);
    }
};

} // namespace loader

namespace
{
using VersionedLoader =
    bl::file::binary::VersionedFile<Conversation, loader::LegacyConversationLoader,
                                    loader::ConversationLoader>;
}

Conversation::Conversation()
: cnodes(*this) {}

bool Conversation::load(const std::string& file) {
    bl::file::binary::File input(bl::file::Util::joinPath(Properties::ConversationPath(), file),
                                 bl::file::binary::File::Read);
    VersionedLoader loader;
    return loader.read(input, *this);
}

bool Conversation::save(const std::string& file) const {
    bl::file::binary::File output(bl::file::Util::joinPath(Properties::ConversationPath(), file),
                                  bl::file::binary::File::Write);
    return serialize(output);
}

const std::vector<Conversation::Node>& Conversation::nodes() const { return cnodes.getValue(); }

void Conversation::deleteNode(unsigned int i) {
    if (i < cnodes.getValue().size()) {
        cnodes.getValue().erase(cnodes.getValue().begin() + i);
        shiftDownJumps(cnodes.getValue(), i);
    }
    else {
        BL_LOG_WARN << "Tried to delete out of range node: " << i;
    }
}

void Conversation::insertNode(unsigned int i, const Node& node) {
    if (i < cnodes.getValue().size()) {
        shiftUpJumps(cnodes.getValue(), i);
        cnodes.getValue().insert(cnodes.getValue().begin() + i, node);
        cnodes.getValue()[i].nextOnPass()   = i + 1;
        cnodes.getValue()[i].nextOnReject() = cnodes.getValue().size();
    }
    else {
        BL_LOG_WARN << "Out of range node insert: " << i;
    }
}

void Conversation::setNode(unsigned int i, const Node& node) {
    if (i < cnodes.getValue().size()) { cnodes.getValue()[i] = node; }
    else {
        BL_LOG_WARN << "Out of range node assign: " << i;
    }
}

Conversation::Node::Node() { setType(Talk); }

Conversation::Node::Node(Type t) { setType(t); }

void Conversation::Node::setType(Type t) {
    type = t;
    prompt.clear();

    switch (type) {
    case GiveItem:
    case TakeItem:
        data.emplace<item::Id>(item::Id::Unknown);
        break;

    case GiveMoney:
    case TakeMoney:
        data.emplace<std::uint32_t>(0);
        break;

    case Prompt:
        data.emplace<std::vector<std::pair<std::string, std::uint32_t>>>();
        break;

    default:
        break;
    }
}

Conversation::Node::Type Conversation::Node::getType() const { return type; }

std::string& Conversation::Node::message() { return prompt; }

std::string& Conversation::Node::script() { return prompt; }

std::string& Conversation::Node::saveFlag() { return prompt; }

std::vector<std::pair<std::string, std::uint32_t>>& Conversation::Node::choices() {
    static std::vector<std::pair<std::string, std::uint32_t>> null;
    auto* d = std::get_if<std::vector<std::pair<std::string, std::uint32_t>>>(&data);
    if (d) { return *d; }
    BL_LOG_ERROR << "Bad Node access (choices). Type=" << type;
    return null;
}

unsigned int& Conversation::Node::money() {
    static unsigned int null;
    unsigned int* m = std::get_if<unsigned int>(&data);
    if (m) return *m;
    BL_LOG_ERROR << "Bad node access (money). Type=" << static_cast<int>(type);
    return null;
}

item::Id& Conversation::Node::item() {
    static item::Id null = item::Id::Unknown;
    item::Id* i          = std::get_if<item::Id>(&data);
    if (i) return *i;
    BL_LOG_ERROR << "Bad node access (item). Type=" << static_cast<int>(type);
    return null;
}

std::uint32_t& Conversation::Node::next() { return jumps.nextNode; }

std::uint32_t& Conversation::Node::nextOnPass() { return jumps.condNodes[0]; }

std::uint32_t& Conversation::Node::nextOnReject() { return jumps.condNodes[1]; }

} // namespace file
} // namespace core
