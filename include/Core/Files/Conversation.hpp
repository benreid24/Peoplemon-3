#ifndef CORE_FILES_CONVERSATION_HPP
#define CORE_FILES_CONVERSATION_HPP

#include <BLIB/Files/Binary.hpp>
#include <Core/Items/Id.hpp>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace core
{
namespace file
{
namespace loader
{
class LegacyConversationLoader;
class ConversationLoader;
} // namespace loader

/**
 * @brief Stores a conversation that an NPC or trainer can have with the player
 *
 * @ingroup File
 *
 */
class Conversation : private bl::file::binary::SerializableObject {
public:
    /**
     * @brief Building block of conversations. A conversation is a tree of nodes and each node is an
     *        action that happens in the conversation
     *
     */
    class Node {
    public:
        /**
         * @brief Represents the different effects that nodes can have
         *
         */
        enum Type : std::uint8_t {
            /// This just outputs a message
            Talk = 0,

            /// This will output a message then prompt the player for a choice
            Prompt = 1,

            /// This unconditionally gives the player an item
            GiveItem = 2,

            /// This will ask the player for an item. They may refuse or not have it
            TakeItem = 3,

            /// This will unconditionally give money to the player
            GiveMoney = 4,

            /// This will ask the player for money. They may refuse or not have enough
            TakeMoney = 5,

            /// This will run a script. Must be a file
            RunScript = 6,

            /// This will check if a flag exists and jump to the next node accordingly
            CheckSaveFlag = 7,

            /// This will check if a flag exists and jump to the next node accordingly
            SetSaveFlag = 8,

            /// This will check if interacted with the talking NPC and jump accordingly
            CheckInteracted = 9,

            _LEGACY_Label,
            _LEGACY_Jump
        };

        /**
         * @brief Creates an empty talk node
         *
         */
        Node();

        /**
         * @brief Construct an empty Node of the given type
         *
         * @param type The type to be
         */
        Node(Type type);

        /**
         * @brief Clears node data and updates to a new type
         *
         * @param type The type to be
         */
        void setType(Type type);

        /**
         * @brief Returns the type of this node
         *
         */
        Type getType() const;

        /**
         * @brief Returns the message or prompt for this node
         *
         */
        std::string& message();

        /**
         * @brief Returns the script file of this node
         *
         */
        std::string& script();

        /**
         * @brief Returns the save flag of this node
         *
         */
        std::string& saveFlag();

        /**
         * @brief Returns the choices if this is a prompt node. Undefined behavior if not
         *
         */
        std::vector<std::pair<std::string, std::uint32_t>>& choices();

        /**
         * @brief Returns the money requested or given, undefined behavior if not a money node
         *
         */
        unsigned int& money();

        /**
         * @brief Returns the item to give or take. Undefined behavior if not an item node
         *
         */
        item::Id& item();

        /**
         * @brief Returns the index of the next node in the case of a Talk, Give, and Script nodes
         *
         */
        std::uint32_t& next();

        /**
         * @brief Returns the next node if a check passes (ie take item or money)
         *
         */
        std::uint32_t& nextOnPass();

        /**
         * @brief Returns the next node if a check is rejected (ie not taking money or item)
         *
         */
        std::uint32_t& nextOnReject();

        /**
         * @brief Returns the message or prompt for this node
         *
         */
        const std::string& message() const;

        /**
         * @brief Returns the script file of this node
         *
         */
        const std::string& script() const;

        /**
         * @brief Returns the save flag of this node
         *
         */
        const std::string& saveFlag() const;

        /**
         * @brief Returns the choices if this is a prompt node. Undefined behavior if not
         *
         */
        const std::vector<std::pair<std::string, std::uint32_t>>& choices() const;

        /**
         * @brief Returns the money requested or given, undefined behavior if not a money node
         *
         */
        unsigned int money() const;

        /**
         * @brief Returns the item to give or take. Undefined behavior if not an item node
         *
         */
        item::Id item() const;

        /**
         * @brief Returns the index of the next node in the case of a Talk, Give, and Script nodes
         *
         */
        std::uint32_t next() const;

        /**
         * @brief Returns the next node if a check passes (ie take item or money)
         *
         */
        std::uint32_t nextOnPass() const;

        /**
         * @brief Returns the next node if a check is rejected (ie not taking money or item)
         *
         */
        std::uint32_t nextOnReject() const;

    private:
        Type type;
        std::string prompt; // or script
        std::variant<std::vector<std::pair<std::string, std::uint32_t>>, item::Id, unsigned int>
            data;
        union {
            std::uint32_t nextNode;
            std::uint32_t condNodes[2];
        } jumps;

        friend class bl::file::binary::Serializer<core::file::Conversation::Node, false>;
    };

    /**
     * @brief Creates an empty conversation
     *
     */
    Conversation();

    /**
     * @brief Copy constructs the conversation
     *
     * @param copy The conversation to copy from
     */
    Conversation(const Conversation& copy);

    /**
     * @brief Loads the conversation from the given file
     *
     * @param file The file to load from. This should be relative to the conversation path
     * @return True if loaded, false on error
     */
    bool load(const std::string& file);

    /**
     * @brief Saves the conversation to the given file
     *
     * @param file The file to save to, relative to the conversation path
     * @return True if saved, false on error
     */
    bool save(const std::string& file) const;

    /**
     * @brief Returns the list of nodes in the conversation
     *
     */
    const std::vector<Node>& nodes() const;

    /**
     * @brief Deletes the node at the given index. Jump indexes for other nodes are updated if
     *        affected. Jumps to node i are invalidated and will terminate the conversation
     *
     * @param i The index to erase the node at
     */
    void deleteNode(unsigned int i);

    /**
     * @brief Inserts a new node at the given index. Jumps to other indexes are updated
     *
     * @param i The index to insert at
     * @param node The node to insert
     */
    void insertNode(unsigned int i, const Node& node);

    /**
     * @brief Appends the given node to the list of nodes
     *
     * @param node The node to append
     */
    void appendNode(const Node& node);

    /**
     * @brief Updates the value of the node at the given index
     *
     * @param i The index to update
     * @param node The new node value
     */
    void setNode(unsigned int i, const Node& node);

private:
    bl::file::binary::SerializableField<1, std::vector<Node>> cnodes;

    friend class loader::LegacyConversationLoader;
    friend class loader::ConversationLoader;
};

} // namespace file
} // namespace core

namespace bl
{
namespace file
{
namespace binary
{
template<>
struct Serializer<core::file::Conversation::Node, false> {
    static bool serialize(File& output, const core::file::Conversation::Node& node);
    static bool deserialize(File& input, core::file::Conversation::Node& node);
    static std::uint32_t size(const core::file::Conversation::Node& node);
};

} // namespace binary
} // namespace file
} // namespace bl

#endif
