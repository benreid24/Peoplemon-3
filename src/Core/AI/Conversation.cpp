#include <Core/AI/Conversation.hpp>

#include <Core/Scripts/ConversationContext.hpp>
#include <Core/Scripts/LegacyWarn.hpp>
#include <Core/Systems/Systems.hpp>

namespace core
{
namespace ai
{
namespace
{
file::Conversation::Node errorNode() {
    file::Conversation::Node node;
    node.setType(file::Conversation::Node::Talk);
    node.message() = "<ERROR: Current node is invalid>";
    return node;
}
} // namespace

Conversation::Conversation(system::Systems& s)
: systems(s)
, nodes(nullptr)
, current(0) {}

void Conversation::setConversation(const file::Conversation& conv, bl::entity::Entity e, bool t) {
    nodes         = &conv.nodes();
    current       = 0;
    entity        = e;
    alreadyTalked = t;
    followNodes();
}

bool Conversation::finished() const { return nodes && current >= nodes->size(); }

const file::Conversation::Node& Conversation::currentNode() const {
    if (!finished()) return nodes->at(current);
    static const file::Conversation::Node garbage = errorNode();
    return garbage;
}

void Conversation::notifyNext() {
    if (!finished()) {
        current = nodes->at(current).next();
        followNodes();
    }
}

void Conversation::notifyCheckPassed() {
    if (!finished()) {
        current = nodes->at(current).nextOnPass();
        followNodes();
    }
}

void Conversation::notifyCheckFailed() {
    if (!finished()) {
        current = nodes->at(current).nextOnReject();
        followNodes();
    }
}

void Conversation::notifyChoiceMade(const std::string& choice) {
    if (currentNode().getType() == file::Conversation::Node::Prompt) {
        for (const auto& pair : nodes->at(current).choices()) {
            if (pair.first == choice) {
                current = pair.second;
                followNodes();
                break;
            }
        }
    }
}

void Conversation::followNodes() {
    using E               = file::Conversation::Node::Type;
    const auto isBlocking = [this](unsigned int i) {
        switch (nodes->at(i).getType()) {
        case E::CheckInteracted:
        case E::CheckSaveFlag:
        case E::RunScript:
        case E::SetSaveFlag:
            return false;

        default:
            return true;
        }
    };

    while (!finished() && !isBlocking(current)) {
        switch (nodes->at(current).getType()) {
        case E::CheckInteracted:
            current =
                alreadyTalked ? nodes->at(current).nextOnPass() : nodes->at(current).nextOnReject();
            break;

        case E::SetSaveFlag:
            // TODO - set save flag in gamesave
            current = nodes->at(current).next();
            break;

        case E::CheckSaveFlag:
            // TODO - check save flag in gamesave
            current = nodes->at(current).nextOnReject();
            break;

        case E::RunScript: {
            const auto cb = [this](bl::entity::Entity e) {
                if (e != entity) return true;
                return finished();
            };

            script::LegacyWarn::warn(nodes->at(current).script());
            bl::script::Script script(nodes->at(current).script(),
                                      script::ConversationContext(systems, entity, cb));
            if (nodes->at(current).runConcurrently()) {
                script.runBackground(&systems.engine().scriptManager());
            }
            else {
                script.run(&systems.engine().scriptManager());
            }
            current = nodes->at(current).next();
        } break;

        default:
            BL_LOG_ERROR << "Unhandled nonblocking node type " << nodes->at(current).getType()
                         << ", terminating";
            current = nodes->size();
            break;
        }
    }
}

} // namespace ai
} // namespace core
