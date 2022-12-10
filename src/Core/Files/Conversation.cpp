#include <Core/Files/Conversation.hpp>

#include <Core/Items/Item.hpp>
#include <Core/Properties.hpp>
#include <Core/Resources.hpp>

namespace core
{
namespace file
{
namespace
{
constexpr unsigned int EndNode = 9999999;

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
            else { jump = EndNode; }
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
} // namespace

bool Conversation::load(const std::string& file) {
    return ConversationManager::initializeExisting(file, *this);
}

bool Conversation::loadDev(std::istream& input) {
    return bl::serial::json::Serializer<Conversation>::deserializeStream(input, *this);
}

bool Conversation::loadProd(bl::serial::binary::InputStream& input) {
    return bl::serial::binary::Serializer<Conversation>::deserialize(input, *this);
}

bool Conversation::save(const std::string& file) const {
    std::ofstream output(file.c_str());
    return bl::serial::json::Serializer<Conversation>::serializeStream(output, *this, 4, 0);
}

const std::vector<Conversation::Node>& Conversation::nodes() const { return cnodes; }

void Conversation::deleteNode(unsigned int i) {
    if (i < cnodes.size()) {
        shiftDownJumps(cnodes, i);
        cnodes.erase(cnodes.begin() + i);
    }
    else { BL_LOG_WARN << "Tried to delete out of range node: " << i; }
}

void Conversation::appendNode(const Node& node) { cnodes.push_back(node); }

void Conversation::setNode(unsigned int i, const Node& node) {
    if (i < cnodes.size()) { cnodes[i] = node; }
    else { BL_LOG_WARN << "Out of range node assign: " << i; }
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
