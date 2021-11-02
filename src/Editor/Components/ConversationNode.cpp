#include <Editor/Components/ConversationNode.hpp>

namespace editor
{
namespace component
{
namespace
{
constexpr unsigned int ConversationEnd = 99999999;
constexpr unsigned int MaxLen          = 24;

std::string typeToString(core::file::Conversation::Node::Type t) {
    using T = core::file::Conversation::Node::Type;
    switch (t) {
    case T::Talk:
        return "Talk";
    // TODO - other types
    default:
        return "Unknown";
    }
}

std::string nodeToString(unsigned int i, const core::file::Conversation::Node& node) {
    using T = core::file::Conversation::Node::Type;

    std::stringstream ss;
    ss << "#" << i << ":" << typeToString(node.getType());
    if (node.getType() == T::Talk || node.getType() == T::Prompt) {
        ss << ":"
           << (node.message().size() > MaxLen ? node.message().substr(0, MaxLen - 3) + "..." :
                                                node.message());
    }
    return ss.str();
}

} // namespace

using core::file::Conversation;
using namespace bl::gui;

ConversationNode::ConversationNode(const NotifyCb& ecb, const NotifyCb& odcb, const NotifyCb& rgtcb,
                                   const CreateNode& cncb, Box::Ptr& container,
                                   const std::vector<core::file::Conversation::Node>* nodes)
: onEdit(ecb)
, onDelete(odcb)
, regenTree(rgtcb)
, createNode(cncb)
, allNodes(nodes)
, nextNode(
      "Next Node:", regenTree, createNode,
      [this](unsigned int nn) {
          current->next() = nn;
          onEdit();
          regenTree();
      },
      nodes) {
    for (unsigned int i = 0; i < 10; ++i) {
        values[i] = Conversation::Node(static_cast<Conversation::Node::Type>(i));
    }

    promptRow   = Box::create(LinePacker::create(LinePacker::Horizontal, 6.f));
    promptEntry = TextEntry::create(3);
    promptEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        current->message() = promptEntry->getInput();
    });
    promptRow->pack(Label::create("Say:"), false, true);
    promptRow->pack(promptEntry, true, true);

    Box::Ptr row =
        Box::create(LinePacker::create(LinePacker::Horizontal, 0.f, LinePacker::Uniform));
    Button::Ptr but = Button::create("Delete Node");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        onDelete();
    });
    but->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(but, false, true);
    container->pack(row, true, false);

    editArea = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    container->pack(editArea, true, true);
}

void ConversationNode::update(const Conversation::Node& node) {
    values[node.getType()] = node;
    current                = &values[node.getType()];

    editArea->clearChildren(true);
    nextNode.sync();
    switch (node.getType()) {
    case Conversation::Node::Type::Talk:
        promptEntry->setInput(node.message());
        nextNode.setSelected(node.next());
        editArea->pack(promptRow, true, false);
        editArea->pack(nextNode.content(), true, false);
        break;

    default:
        BL_LOG_ERROR << "Unimplemented node type: " << node.getType();
        break;
    }
}

const Conversation::Node& ConversationNode::getValue() const { return *current; }

ConversationNode::NodeConnector::NodeConnector(
    const std::string& label, const NotifyCb& regenTree, const CreateNode& createNode,
    const ChangeCb& changeCb, const std::vector<core::file::Conversation::Node>* nodes)
: nodes(nodes) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create(label), false, true);
    entry = ComboBox::create();
    entry->getSignal(Event::ValueChanged).willAlwaysCall([this, changeCb](const Event&, Element*) {
        changeCb(getSelected());
    });
    row->pack(entry, true, true);
    Button::Ptr cb = Button::create("Make New Node");
    cb->getSignal(Event::LeftClicked)
        .willAlwaysCall([this, createNode, regenTree](const Event&, Element*) {
            const unsigned int nn = createNode();
            sync();
            setSelected(nn);
            regenTree();
        });
    row->pack(cb, false, true);
    sync();
}

void ConversationNode::NodeConnector::sync() {
    entry->clearOptions();
    for (unsigned int i = 0; i < nodes->size(); ++i) {
        entry->addOption(nodeToString(i, nodes->at(i)));
    }
    entry->addOption("End Conversation");
}

void ConversationNode::NodeConnector::setSelected(unsigned int i) {
    entry->setSelectedOption(i >= nodes->size() ? nodes->size() : i);
}

unsigned int ConversationNode::NodeConnector::getSelected() const {
    const unsigned int s = static_cast<unsigned int>(entry->getSelectedOption());
    return s < nodes->size() ? s : ConversationEnd;
}

Box::Ptr& ConversationNode::NodeConnector::content() { return row; }

} // namespace component
} // namespace editor
