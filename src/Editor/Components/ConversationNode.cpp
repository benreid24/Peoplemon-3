#include <Editor/Components/ConversationNode.hpp>

namespace editor
{
namespace component
{
namespace
{
constexpr unsigned int ConversationEnd = 99999999;
constexpr unsigned int MaxLen          = 24;

std::string nodeToString(unsigned int i, const core::file::Conversation::Node& node) {
    using T = core::file::Conversation::Node::Type;

    std::stringstream ss;
    ss << "#" << i << ":" << core::file::Conversation::Node::typeToString(node.getType());
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

ConversationNode::ConversationNode(const FocusCb& fcb, const NotifyCb& ecb, const NotifyCb& odcb,
                                   const NotifyCb& rgtcb, const CreateNode& cncb,
                                   const SelectCb& selectCb, Box::Ptr& container,
                                   const std::vector<core::file::Conversation::Node>* nodes)
: focusCb(fcb)
, onEdit(ecb)
, onDelete(odcb)
, regenTree(rgtcb)
, createNodeCb(cncb)
, onJump(selectCb)
, allNodes(nodes)
, passNext(
      "Pass:", regenTree, createNodeCb, std::bind(&ConversationNode::syncJumps, this),
      [this](unsigned int nn) {
          current.nextOnPass() = nn;
          onEdit();
          regenTree();
      },
      onJump, nodes)
, failNext(
      "Fail:", regenTree, createNodeCb, std::bind(&ConversationNode::syncJumps, this),
      [this](unsigned int nn) {
          current.nextOnReject() = nn;
          onEdit();
          regenTree();
      },
      onJump, nodes)
, nextNode(
      "Next:", regenTree, createNodeCb, std::bind(&ConversationNode::syncJumps, this),
      [this](unsigned int nn) {
          current.next() = nn;
          onEdit();
          regenTree();
      },
      onJump, nodes)
, scriptSelector(
      [this](const std::string& s) {
          current.script() = s;
          scriptLabel->setText(s);
          onEdit();
      },
      [this]() { focusCb(true); }) {
    nodeTitle = Label::create("");
    nodeTitle->setCharacterSize(32);
    nodeTitle->setColor(sf::Color::Cyan, sf::Color::Transparent);
    container->pack(nodeTitle, true, false);

    promptRow   = Box::create(LinePacker::create(LinePacker::Horizontal, 6.f));
    promptEntry = TextEntry::create(3, true);
    promptEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        current.message() = promptEntry->getInput();
        onEdit();
    });
    promptRow->pack(Label::create("Say:"), false, true);
    promptRow->pack(promptEntry, true, true);

    Box::Ptr row =
        Box::create(LinePacker::create(LinePacker::Horizontal, 4.f, LinePacker::Uniform));
    Box::Ptr subrow = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    subrow->pack(Label::create("Node Type:"), false, true);
    typeEntry = ComboBox::create();
    for (unsigned int i = 0; i < 10; ++i) {
        const auto t = static_cast<Conversation::Node::Type>(i);
        typeEntry->addOption(core::file::Conversation::Node::typeToString(t));
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
    choiceScroll->includeScrollbarsInRequisition(true);
    but = Button::create("Add Choice");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        current.choices().emplace_back("", ConversationEnd);
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
                                  createNodeCb,
                                  std::bind(&ConversationNode::syncJumps, this),
                                  onJump,
                                  allNodes);
        it->setIterator(it);
        choiceScroll->pack(it->content(), true, false);
        onEdit();
        regenTree();
    });
    choiceArea->pack(choiceScroll, true, true);
    choiceArea->pack(but, false, false);

    itemSelector = component::ItemSelector::create(
        std::bind(&ConversationNode::onItemChange, this, std::placeholders::_1));
    itemRow = Box::create(LinePacker::create(LinePacker::Horizontal, 2.f));
    itemRow->pack(Label::create("Item:"), false, true);
    itemRow->pack(itemSelector, false, true);

    moneyRow   = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    moneyEntry = TextEntry::create();
    moneyEntry->setMode(TextEntry::Mode::Integer);
    moneyEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&ConversationNode::onMoneyChange, this));
    moneyRow->pack(Label::create("Money Amount:"), false, true);
    moneyRow->pack(moneyEntry, true, true);

    flagRow   = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    flagEntry = TextEntry::create();
    flagEntry->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        current.saveFlag() = flagEntry->getInput();
        onEdit();
    });
    flagRow->pack(Label::create("Flag Name:"), false, true);
    flagRow->pack(flagEntry, true, true);

    editArea = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    container->pack(editArea, true, true);

    scriptRow              = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    ScrollArea::Ptr scroll = ScrollArea::create(LinePacker::create());
    scroll->setMaxSize({450.f, 550.f});
    scroll->setColor(sf::Color(30, 180, 240, 130), sf::Color::Blue);
    scriptLabel = Label::create("<no script selected>");
    scriptLabel->setColor(sf::Color(30, 180, 60), sf::Color::Transparent);
    scroll->pack(scriptLabel);
    scriptRow->pack(Label::create("Script:"));
    scriptRow->pack(scroll, true, false);
    but = Button::create("Edit Script");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        focusCb(false);
        scriptSelector.open(parent, current.script());
    });
    scriptRow->pack(but);
}

void ConversationNode::setParent(const GUI::Ptr& p) { parent = p; }

void ConversationNode::update(unsigned int i, const Conversation::Node& node) {
    current = node;
    index   = i;

    nodeTitle->setText(nodeToString(index, node));
    editArea->clearChildren(true);
    nextNode.sync();
    passNext.sync();
    failNext.sync();
    typeEntry->setSelectedOption(static_cast<int>(node.getType()), false);
    nextNode.setSelected(node.next());
    passNext.setSelected(node.nextOnPass());
    failNext.setSelected(node.nextOnReject());

    switch (node.getType()) {
    case Conversation::Node::Type::Talk:
        promptEntry->setInput(node.message());
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
                                      createNodeCb,
                                      std::bind(&ConversationNode::syncJumps, this),
                                      onJump,
                                      allNodes);
            it->setIterator(it);
            choiceScroll->pack(choices.back().content(), true, false);
        }
        editArea->pack(choiceArea, true, true);
        break;

    case Conversation::Node::GiveItem:
        editArea->pack(itemRow, true, false);
        itemSelector->setItem(node.item().id);
        editArea->pack(nextNode.content(), true, false);
        break;

    case Conversation::Node::TakeItem:
        editArea->pack(itemRow, true, false);
        itemSelector->setItem(node.item().id);
        editArea->pack(passNext.content(), true, false);
        editArea->pack(failNext.content(), true, false);
        break;

    case Conversation::Node::GiveMoney:
        editArea->pack(moneyRow, true, false);
        moneyEntry->setInput(std::to_string(node.money()));
        editArea->pack(nextNode.content(), true, false);
        break;

    case Conversation::Node::TakeMoney:
        editArea->pack(moneyRow, true, false);
        moneyEntry->setInput(std::to_string(node.money()));
        editArea->pack(passNext.content(), true, false);
        editArea->pack(failNext.content(), true, false);
        break;

    case Conversation::Node::SetSaveFlag:
        editArea->pack(flagRow, true, false);
        flagEntry->setInput(node.saveFlag());
        editArea->pack(nextNode.content(), true, false);
        break;

    case Conversation::Node::CheckSaveFlag:
        editArea->pack(flagRow, true, false);
        flagEntry->setInput(node.saveFlag());
        editArea->pack(passNext.content(), true, false);
        editArea->pack(failNext.content(), true, false);
        break;

    case Conversation::Node::CheckInteracted:
        editArea->pack(passNext.content(), true, false);
        editArea->pack(failNext.content(), true, false);
        break;

    case Conversation::Node::RunScript:
        editArea->pack(scriptRow, true, false);
        scriptLabel->setText(node.script().empty() ? "<no script selected>" : node.script());
        editArea->pack(nextNode.content(), true, false);
        break;

    default:
        BL_LOG_ERROR << "Unimplemented node type: " << node.getType();
        break;
    }
}

const Conversation::Node& ConversationNode::getValue() const { return current; }

void ConversationNode::onItemChange(core::item::Id item) {
    current.item().id = item;
    onEdit();
}

void ConversationNode::onMoneyChange() {
    const std::string& val = moneyEntry->getInput();
    current.money()        = val.empty() ? 0 : std::atoi(val.c_str());
    onEdit();
}

void ConversationNode::onChoiceChange(unsigned int j, const std::string& t, unsigned int n) {
    current.choices()[j].first  = t;
    current.choices()[j].second = n;
    onEdit();
    regenTree();
}

void ConversationNode::onTypeChange() {
    const Conversation::Node::Type t =
        static_cast<Conversation::Node::Type>(typeEntry->getSelectedOption());

    Conversation::Node val(t);
    val.next()         = current.next();
    val.nextOnPass()   = current.nextOnPass();
    val.nextOnReject() = current.nextOnReject();

    bool e = false;
    if (current.getType() != t) {
        update(index, val);
        e = true;
    }

    onEdit();
    regenTree();
    if (e) {
        update(index, val);
        syncJumps();
        regenTree();
        if (t == Conversation::Node::GiveItem || t == Conversation::Node::TakeItem) {
            current.item().id = itemSelector->currentItem();
        }
        onEdit();
    }
}

void ConversationNode::onChoiceDelete(unsigned int j, std::list<Choice>::iterator it) {
    it->content()->remove();
    choices.erase(it);
    current.choices().erase(current.choices().begin() + j);
    unsigned int k = 0;
    for (auto& choice : choices) {
        choice.setIndex(k);
        ++k;
    }
    onEdit();
    regenTree();
}

void ConversationNode::syncJumps() {
    nextNode.sync();
    passNext.sync();
    failNext.sync();
    for (auto& c : choices) { c.syncJump(); }
}

ConversationNode::NodeConnector::NodeConnector(
    const std::string& prompt, const NotifyCb& regenTree, const CreateNode& createNode,
    const NotifyCb& syncJumpCb, const ChangeCb& changeCb, const SelectCb& selectCb,
    const std::vector<core::file::Conversation::Node>* nodes)
: nodes(nodes) {
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create(prompt), false, true);
    entry = ComboBox::create();
    entry->setMaxHeight(150.f);
    entry->getSignal(Event::ValueChanged).willAlwaysCall([this, changeCb](const Event&, Element*) {
        changeCb(getSelected());
    });
    row->pack(entry, true, true);
    Button::Ptr cb = Button::create("Create");
    cb->setTooltip("Create a new node and jump to it");
    cb->getSignal(Event::LeftClicked)
        .willAlwaysCall(
            [this, createNode, regenTree, syncJumpCb, changeCb](const Event&, Element*) {
                const unsigned int nn = createNode();
                syncJumpCb();
                setSelected(nn);
                changeCb(nn);
                regenTree();
            });
    row->pack(cb, false, true);
    cb = Button::create("Goto");
    cb->setTooltip("Select the node this points to");
    cb->getSignal(Event::LeftClicked).willAlwaysCall([this, selectCb](const Event&, Element*) {
        selectCb(getSelected());
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
                                 const NotifyCb& syncJumpCb, const SelectCb& selectCb,
                                 const std::vector<core::file::Conversation::Node>* nodes)
: onChange(changeCb)
, onDelete(delCb)
, index(i)
, jump(
      "Next:", regenTree, createNode, syncJumpCb,
      [this](unsigned int nj) { onChange(index, entry->getInput(), nj); }, selectCb, nodes) {
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
