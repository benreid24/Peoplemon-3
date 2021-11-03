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
    case T::Prompt:
        return "Question";
    case T::GiveItem:
        return "Give Item";
    case T::TakeItem:
        return "Take Item";
    case T::GiveMoney:
        return "Give Money";
    case T::TakeMoney:
        return "Take Money";
    case T::RunScript:
        return "Run Script";
    case T::CheckSaveFlag:
        return "Check Flag";
    case T::SetSaveFlag:
        return "Set Flag";
    case T::CheckInteracted:
        return "Check Talked";
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
, createNodeCb(cncb)
, allNodes(nodes)
, nextNode(
      "Next Node:", regenTree, std::bind(&ConversationNode::createNode, this),
      [this](unsigned int nn) {
          current->next() = nn;
          onEdit();
          regenTree();
      },
      nodes) {
    promptRow   = Box::create(LinePacker::create(LinePacker::Horizontal, 6.f));
    promptEntry = TextEntry::create(3);
    promptEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        current->message() = promptEntry->getInput();
        onEdit();
    });
    promptRow->pack(Label::create("Say:"), false, true);
    promptRow->pack(promptEntry, true, true);

    Box::Ptr row =
        Box::create(LinePacker::create(LinePacker::Horizontal, 0.f, LinePacker::Uniform));
    Box::Ptr subrow = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    subrow->pack(Label::create("Node Type:"), false, true);
    typeEntry = ComboBox::create();
    for (unsigned int i = 0; i < 10; ++i) {
        const auto t = static_cast<Conversation::Node::Type>(i);
        values[i]    = Conversation::Node(t);
        typeEntry->addOption(typeToString(t));
    }
    typeEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&ConversationNode::onTypeChange, this));
    subrow->pack(typeEntry, false, true);
    Button::Ptr but = Button::create("Delete Node");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        onDelete();
    });
    but->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(subrow, false, true);
    row->pack(but, false, true);
    container->pack(row, true, false);

    choiceArea   = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    choiceScroll = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    choiceScroll->setColor(sf::Color::Transparent, sf::Color::Black);
    choiceScroll->setOutlineThickness(2.f);
    choiceScroll->setMaxSize({1000.f, 400.f});
    but = Button::create("Add Choice");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        current->choices().emplace_back("", ConversationEnd);
        auto it = choices.emplace(choices.end(),
                                  choices.size(),
                                  "",
                                  ConversationEnd,
                                  std::bind(&ConversationNode::onChoiceChange,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2,
                                            std::placeholders::_3),
                                  std::bind(&ConversationNode::onChoiceDelete,
                                            this,
                                            std::placeholders::_1,
                                            std::placeholders::_2),
                                  regenTree,
                                  std::bind(&ConversationNode::createNode, this),
                                  allNodes);
        it->setIterator(it);
        choiceScroll->pack(it->content(), true, false);
        onEdit();
    });
    choiceArea->pack(choiceScroll, true, true);
    choiceArea->pack(but, false, false);

    editArea = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    container->pack(editArea, true, true);
}

void ConversationNode::update(const Conversation::Node& node) {
    values[node.getType()] = node;
    current                = &values[node.getType()];

    editArea->clearChildren(true);
    nextNode.sync();
    typeEntry->setSelectedOption(static_cast<int>(node.getType()), false);
    switch (node.getType()) {
    case Conversation::Node::Type::Talk:
        promptEntry->setInput(node.message());
        nextNode.setSelected(node.next());
        editArea->pack(promptRow, true, false);
        editArea->pack(nextNode.content(), true, false);
        break;

    case Conversation::Node::Type::Prompt:
        promptEntry->setInput(node.message());
        editArea->pack(promptRow, true, false);
        choiceScroll->clearChildren(true);
        choices.clear();
        for (unsigned int i = 0; i < node.choices().size(); ++i) {
            const auto& c = node.choices()[i];
            auto it       = choices.emplace(choices.end(),
                                      i,
                                      c.first,
                                      c.second,
                                      std::bind(&ConversationNode::onChoiceChange,
                                                this,
                                                std::placeholders::_1,
                                                std::placeholders::_2,
                                                std::placeholders::_3),
                                      std::bind(&ConversationNode::onChoiceDelete,
                                                this,
                                                std::placeholders::_1,
                                                std::placeholders::_2),
                                      regenTree,
                                      std::bind(&ConversationNode::createNode, this),
                                      allNodes);
            it->setIterator(it);
            choiceScroll->pack(choices.back().content(), true, false);
        }
        editArea->pack(choiceArea, true, true);
        break;

    default:
        BL_LOG_ERROR << "Unimplemented node type: " << node.getType();
        break;
    }
}

const Conversation::Node& ConversationNode::getValue() const { return *current; }

void ConversationNode::onChoiceChange(unsigned int j, const std::string& t, unsigned int n) {
    current->choices()[j].first  = t;
    current->choices()[j].second = n;
    onEdit();
}

void ConversationNode::onTypeChange() {
    const Conversation::Node::Type t =
        static_cast<Conversation::Node::Type>(typeEntry->getSelectedOption());
    bool e = false;
    if (current != &values[t]) {
        update(values[t]);
        e = true;
    }
    onEdit();
    if (e) { update(values[t]); }
}

void ConversationNode::onChoiceDelete(unsigned int j, std::list<Choice>::iterator it) {
    it->content()->remove();
    choices.erase(it);
    current->choices().erase(current->choices().begin() + j);
    unsigned int k = 0;
    for (auto& choice : choices) {
        choice.setIndex(k);
        ++k;
    }
    onEdit();
}

unsigned int ConversationNode::createNode() {
    const unsigned int nn = createNodeCb();
    nextNode.sync();
    for (auto& c : choices) { c.syncJump(); }
    return nn;
}

ConversationNode::NodeConnector::NodeConnector(
    const std::string& label, const NotifyCb& regenTree, const CreateNode& createNode,
    const ChangeCb& changeCb, const std::vector<core::file::Conversation::Node>* nodes)
: nodes(nodes) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create(label), false, true);
    entry = ComboBox::create();
    entry->setMaxHeight(150.f);
    entry->getSignal(Event::ValueChanged).willAlwaysCall([this, changeCb](const Event&, Element*) {
        changeCb(getSelected());
    });
    row->pack(entry, true, true);
    Button::Ptr cb = Button::create("Create");
    cb->setTooltip("Create a new node and jump to it");
    cb->getSignal(Event::LeftClicked)
        .willAlwaysCall([this, createNode, regenTree](const Event&, Element*) {
            const unsigned int nn = createNode();
            setSelected(nn);
            regenTree();
        });
    row->pack(cb, false, true);
    sync();
}

void ConversationNode::NodeConnector::sync() {
    const unsigned int oldSel = entry->getSelectedOption() == entry->optionCount() - 1 ?
                                    ConversationEnd :
                                    entry->getSelectedOption();
    entry->clearOptions();
    for (unsigned int i = 0; i < nodes->size(); ++i) {
        entry->addOption(nodeToString(i, nodes->at(i)));
    }
    entry->addOption("End Conversation");
    setSelected(oldSel);
}

void ConversationNode::NodeConnector::setSelected(unsigned int i) {
    entry->setSelectedOption(i >= nodes->size() ? nodes->size() : i, false);
}

unsigned int ConversationNode::NodeConnector::getSelected() const {
    const unsigned int s = static_cast<unsigned int>(entry->getSelectedOption());
    return s < nodes->size() ? s : ConversationEnd;
}

Box::Ptr& ConversationNode::NodeConnector::content() { return row; }

ConversationNode::Choice::Choice(unsigned int i, const std::string& c, unsigned int jp,
                                 const OnChange& changeCb, const OnDelete& delCb,
                                 const NotifyCb& regenTree, const CreateNode& createNode,
                                 const std::vector<core::file::Conversation::Node>* nodes)
: onChange(changeCb)
, onDelete(delCb)
, index(i)
, jump(
      "Jump To:", regenTree, createNode,
      [this](unsigned int nj) { onChange(index, entry->getInput(), nj); }, nodes) {
    jump.setSelected(jp);

    box = Box::create(LinePacker::create(LinePacker::Vertical));
    box->setColor(sf::Color::Transparent, sf::Color::Blue);
    box->setOutlineThickness(1.f);

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Text:"), false, true);
    entry = TextEntry::create();
    entry->setInput(c);
    entry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        onChange(index, entry->getInput(), jump.getSelected());
    });
    row->pack(entry, true, true);
    Button::Ptr but = Button::create("Remove");
    but->setColor(sf::Color::Red, sf::Color::Black);
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        onDelete(index, it);
    });
    row->pack(but, false, true);

    box->pack(row, true, false);
    box->pack(jump.content(), true, false);
}

void ConversationNode::Choice::setIndex(unsigned int i) { index = i; }

void ConversationNode::Choice::setIterator(std::list<Choice>::iterator i) { it = i; }

Box::Ptr& ConversationNode::Choice::content() { return box; }

void ConversationNode::Choice::syncJump() { jump.sync(); }

} // namespace component
} // namespace editor
