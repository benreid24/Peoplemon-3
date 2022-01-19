#include <Core/Files/Conversation.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>

namespace core
{
namespace file
{
namespace
{
void shiftDownJumps(std::vector<Conversation::Node>& nodes, unsigned int i) {
    const auto updateJump = [&nodes, i](std::uint32_t& jump) {
        const auto isSimpleNext = [&nodes](unsigned int i) {
            if (i >= nodes.size()) return false;
            switch (nodes[i].getType()) {
            case Conversation::Node::Talk:
            case Conversation::Node::GiveMoney:
            case Conversation::Node::GiveItem:
            case Conversation::Node::RunScript:
            case Conversation::Node::SetSaveFlag:
                return true;

            default:
                return false;
            }
        };

        if (jump == i) {
            if (isSimpleNext(jump)) {
                const std::uint32_t j = nodes[jump].next();
                jump                  = j > i ? j - 1 : j;
            }
            else {
                jump = 9999999;
            }
        }
        else if (jump > i)
            --jump;
    };

    for (unsigned int j = 0; j < nodes.size(); ++j) {
        Conversation::Node& node = nodes[j];

        // Invalidate or shift down
        updateJump(node.nextOnPass());
        updateJump(node.nextOnReject());

        // Choices
        if (node.getType() == Conversation::Node::Prompt) {
            for (auto& pair : node.choices()) {
                // Invalidate or shift down
                updateJump(pair.second);
            }
        }
    }
}

std::string tolower(const std::string& s) {
    std::string lower(s);
    for (char& c : lower) { c = std::tolower(c); }
    return lower;
}

} // namespace

namespace loader
{
struct LegacyConversationLoader : public bl::serial::binary::SerializerVersion<Conversation> {
    virtual bool read(Conversation& result, bl::serial::binary::InputStream& input) const override {
        std::vector<Conversation::Node>& nodes = result.cnodes;
        nodes.clear();

        // Data for legacy conversaion
        std::unordered_map<std::string, unsigned int> labelPoints;
        std::vector<std::pair<std::string, std::uint32_t*>> namedJumps;

        // Load legacy data
        std::uint16_t nodeCount = 0;
        if (!input.read(nodeCount)) return false;
        nodes.reserve(nodeCount);
        std::uint16_t i = 0;
        for (std::uint16_t ai = 0; ai < nodeCount; ++ai) {
            nodes.emplace_back();
            Conversation::Node& node = nodes.back();
            bool inc                 = true;

            std::uint8_t type;
            if (!input.read(type)) return false;

            switch (type) {
            case 't':
                node.setType(Conversation::Node::Talk);
                if (!input.read(node.message())) return false;
                node.next() = i + 1;
                break;

            case 'o': {
                node.setType(Conversation::Node::Prompt);
                std::uint16_t choiceCount;
                if (!input.read(node.message())) return false;
                if (!input.read(choiceCount)) return false;
                node.choices().resize(choiceCount, std::make_pair("", 9999999));
                for (std::uint16_t j = 0; j < choiceCount; ++j) {
                    std::string jump;
                    if (!input.read(node.choices()[j].first)) return false;
                    if (!input.read(jump)) return false;
                    namedJumps.emplace_back(jump, &node.choices()[j].second);
                }

            } break;

            case 'j':
                node.setType(Conversation::Node::_LEGACY_Jump);
                if (!input.read(node.message())) return false;
                namedJumps.emplace_back(node.message(), &node.next());
                break;

            case 'z':
                node.setType(Conversation::Node::RunScript);
                node.runConcurrently() = false;
                if (!input.read(node.script())) return false;
                node.next() = i + 1;
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
                    node.item().id           = item::Item::cast(d);
                    node.item().beforePrompt = false;
                    node.item().afterPrompt  = false;
                }
                node.nextOnPass() = i + 1;
                namedJumps.emplace_back(node.message(), &node.nextOnReject());
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
                    node.item().id           = item::Item::cast(d);
                    node.item().beforePrompt = false;
                    node.item().afterPrompt  = false;
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
                if (!input.read(node.saveFlag())) return false;
                if (!input.read(jump)) return false;
                node.nextOnPass() = i + 1;
                namedJumps.emplace_back(jump, &nodes.back().nextOnReject());
            } break;

            case 'l':
                if (!input.read(node.message())) return false;
                labelPoints.insert(std::make_pair(node.message(), i));
                nodes.pop_back();
                inc = false;
                break;

            case 'w': {
                node.setType(Conversation::Node::CheckInteracted);
                node.nextOnPass() = i + 1;
                std::string jump;
                if (!input.read(jump)) return false;
                namedJumps.emplace_back(jump, &node.nextOnReject());
            } break;

            default:
                BL_LOG_ERROR << "Unknown legacy conversation node type: " << type;
                return false;
            }

            if (inc) ++i;
        }

        // Resolve named jumps
        for (auto& pair : namedJumps) {
            auto it = labelPoints.find(pair.first);
            if (it != labelPoints.end()) { *pair.second = it->second; }
            else {
                if (tolower(pair.first) != "end") {
                    BL_LOG_WARN << "Invalid jump '" << pair.first << "' in conversation '"
                                << "', jumping to end";
                }
                *pair.second = 9999999;
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
                            pair.second -= 1;
                    }
                }
            }
        };

        for (unsigned int i = 0; i < nodes.size(); ++i) {
            if (nodes[i].getType() == Conversation::Node::_LEGACY_Jump) {
                const unsigned int next =
                    nodes[i].next() > i ? nodes[i].next() - 1 : nodes[i].next();
                nodes.erase(nodes.begin() + i);
                connectJump(i, next);
                --i;
            }
        }

        return true;
    }

    virtual bool write(const Conversation&, bl::serial::binary::OutputStream&) const override {
        // not implemented
        return false;
    }
};

struct ConversationLoader : public bl::serial::binary::SerializerVersion<Conversation> {
    using Serializer = bl::serial::binary::Serializer<Conversation>;

    virtual bool read(Conversation& result, bl::serial::binary::InputStream& input) const override {
        return Serializer::deserialize(input, result);
    }

    virtual bool write(const Conversation& conversation,
                       bl::serial::binary::OutputStream& output) const override {
        return Serializer::serialize(output, conversation);
    }
};

} // namespace loader

namespace
{
using VersionedLoader =
    bl::serial::binary::VersionedSerializer<Conversation, loader::LegacyConversationLoader,
                                            loader::ConversationLoader>;
}

bool Conversation::load(const std::string& file) {
    bl::serial::binary::InputFile input(
        bl::util::FileUtil::joinPath(Properties::ConversationPath(), file));
    return VersionedLoader::read(input, *this);
}

bool Conversation::save(const std::string& file) const {
    bl::serial::binary::OutputFile output(
        bl::util::FileUtil::joinPath(Properties::ConversationPath(), file));
    return VersionedLoader::write(output, *this);
}

const std::vector<Conversation::Node>& Conversation::nodes() const { return cnodes; }

void Conversation::deleteNode(unsigned int i) {
    if (i < cnodes.size()) {
        shiftDownJumps(cnodes, i);
        cnodes.erase(cnodes.begin() + i);
    }
    else {
        BL_LOG_WARN << "Tried to delete out of range node: " << i;
    }
}

void Conversation::appendNode(const Node& node) { cnodes.push_back(node); }

void Conversation::setNode(unsigned int i, const Node& node) {
    if (i < cnodes.size()) { cnodes[i] = node; }
    else {
        BL_LOG_WARN << "Out of range node assign: " << i;
    }
}

Conversation::Node::Node() { setType(Talk); }

Conversation::Node::Node(Type t) { setType(t); }

void Conversation::Node::setType(Type t) {
    type = t;
    prompt.clear();
    next() = nextOnPass() = nextOnReject() = 999999;

    switch (type) {
    case GiveItem:
    case TakeItem:
        data.emplace<Item>();
        break;

    case GiveMoney:
    case TakeMoney:
        data.emplace<std::uint32_t>(0);
        break;

    case Prompt:
        data.emplace<std::vector<std::pair<std::string, std::uint32_t>>>();
        break;

    case RunScript:
        data.emplace<bool>(false);
        break;

    default:
        break;
    }
}

Conversation::Node::Type Conversation::Node::getType() const { return type; }

std::string& Conversation::Node::message() { return prompt; }

std::string& Conversation::Node::script() { return prompt; }

std::string& Conversation::Node::saveFlag() { return prompt; }

bool& Conversation::Node::runConcurrently() {
    static bool null = false;
    bool* r          = std::get_if<bool>(&data);
    if (r) return *r;
    BL_LOG_ERROR << "Bad Node access (concurrent). Type=" << static_cast<int>(type);
    return null;
}

bool Conversation::Node::runConcurrently() const {
    const bool* r = std::get_if<bool>(&data);
    if (r) return *r;
    BL_LOG_ERROR << "Bad Node access (concurrent). Type=" << static_cast<int>(type);
    return false;
}

std::vector<std::pair<std::string, std::uint32_t>>& Conversation::Node::choices() {
    static std::vector<std::pair<std::string, std::uint32_t>> null;
    auto* d = std::get_if<std::vector<std::pair<std::string, std::uint32_t>>>(&data);
    if (d) { return *d; }
    BL_LOG_ERROR << "Bad Node access (choices). Type=" << static_cast<int>(type);
    return null;
}

std::uint32_t& Conversation::Node::money() {
    static std::uint32_t null;
    std::uint32_t* m = std::get_if<std::uint32_t>(&data);
    if (m) return *m;
    BL_LOG_ERROR << "Bad node access (money). Type=" << static_cast<int>(type);
    return null;
}

Conversation::Node::Item& Conversation::Node::item() {
    static Item null;
    Item* i = std::get_if<Item>(&data);
    if (i) return *i;
    BL_LOG_ERROR << "Bad node access (item). Type=" << static_cast<int>(type);
    return null;
}

std::uint32_t& Conversation::Node::next() { return jumps[0]; }

std::uint32_t& Conversation::Node::nextOnPass() { return jumps[0]; }

std::uint32_t& Conversation::Node::nextOnReject() { return jumps[1]; }

const std::string& Conversation::Node::message() const { return prompt; }

const std::string& Conversation::Node::script() const { return prompt; }

const std::string& Conversation::Node::saveFlag() const { return prompt; }

const std::vector<std::pair<std::string, std::uint32_t>>& Conversation::Node::choices() const {
    static std::vector<std::pair<std::string, std::uint32_t>> null;
    const auto* d = std::get_if<std::vector<std::pair<std::string, std::uint32_t>>>(&data);
    if (d) { return *d; }
    BL_LOG_ERROR << "Bad Node access (choices). Type=" << type;
    return null;
}

unsigned int Conversation::Node::money() const {
    static unsigned int null;
    const unsigned int* m = std::get_if<unsigned int>(&data);
    if (m) return *m;
    BL_LOG_ERROR << "Bad node access (money). Type=" << static_cast<int>(type);
    return null;
}

const Conversation::Node::Item& Conversation::Node::item() const {
    static const Item null;
    const Item* i = std::get_if<Item>(&data);
    if (i) return *i;
    BL_LOG_ERROR << "Bad node access (item). Type=" << static_cast<int>(type);
    return null;
}

std::uint32_t Conversation::Node::next() const { return jumps[0]; }

std::uint32_t Conversation::Node::nextOnPass() const { return jumps[0]; }

std::uint32_t Conversation::Node::nextOnReject() const { return jumps[1]; }

Conversation Conversation::makeLoadError(const std::string& f) {
    Conversation conv;
#ifdef PEOPLEMON_DEBUG
    Node node;
    node.setType(Node::Talk);
    node.message() = "WARNING: Failed to load conversation: " + f;
    node.next()    = 1;
    conv.appendNode(node);
#else
    (void)f;
#endif
    return conv;
}

std::string Conversation::Node::typeToString(Type t) {
    switch (t) {
    case Node::Type::Talk:
        return "Talk";
    case Node::Type::Prompt:
        return "Question";
    case Node::Type::GiveItem:
        return "Give Item";
    case Node::Type::TakeItem:
        return "Take Item";
    case Node::Type::GiveMoney:
        return "Give Money";
    case Node::Type::TakeMoney:
        return "Take Money";
    case Node::Type::RunScript:
        return "Run Script";
    case Node::Type::CheckSaveFlag:
        return "Check Flag";
    case Node::Type::SetSaveFlag:
        return "Set Flag";
    case Node::Type::CheckInteracted:
        return "Check Talked";
    default:
        return "Unknown";
    }
}

void Conversation::getNextJumps(const Node& node, std::vector<unsigned int>& next) {
    using T = Node::Type;

    next.clear();
    switch (node.getType()) {
    case T::Talk:
    case T::RunScript:
    case T::SetSaveFlag:
    case T::GiveMoney:
    case T::GiveItem:
        next.push_back(node.next());
        break;
    case T::CheckInteracted:
    case T::CheckSaveFlag:
    case T::TakeMoney:
    case T::TakeItem:
        next.push_back(node.nextOnPass());
        next.push_back(node.nextOnReject());
        break;
    case T::Prompt:
        for (const auto& c : node.choices()) { next.push_back(c.second); }
        break;
    default:
        BL_LOG_ERROR << "Unknown node type: " << node.getType();
        break;
    }
}

} // namespace file
} // namespace core
