#include <Editor/Components/ConversationWindow.hpp>

#include <BLIB/Scripts.hpp>
#include <Core/Items/Id.hpp>
#include <Core/Properties.hpp>
#include <queue>

namespace editor
{
namespace component
{
using namespace bl::gui;
namespace
{
const std::string EmptyFile = "<no file selected>";

std::string makePath(const std::string& local) {
    return bl::util::FileUtil::joinPath(core::Properties::ConversationPath(), local);
}

const core::file::Conversation DefaultConversation = []() {
    core::file::Conversation val;
    core::file::Conversation::Node node(core::file::Conversation::Node::Type::Talk);
    node.next() = 9999999;
    val.appendNode(node);
    return val;
}();

bool nodeTerminates(const std::vector<core::file::Conversation::Node>& nodes, unsigned int i) {
    std::queue<unsigned int> toVisit;
    std::vector<bool> visited(nodes.size(), false);

    toVisit.push(i);
    visited[i] = true;

    std::vector<unsigned int> next;
    next.reserve(8);
    while (!toVisit.empty()) {
        const unsigned int n = toVisit.front();
        toVisit.pop();

        core::file::Conversation::getNextJumps(nodes[n], next);
        for (unsigned int j : next) {
            if (j >= nodes.size()) return true;

            if (!visited[j]) {
                visited[j] = true;
                toVisit.push(j);
            }
        }
    }

    return false;
}

} // namespace

ConversationWindow::ConversationWindow(const SelectCb& onSelect, const CancelCb& onCancel)
: selectCb(onSelect)
, cancelCb(onCancel)
, value(DefaultConversation)
, currentNode(0)
, nodeBox(Box::create(LinePacker::create(LinePacker::Vertical, 4.f)))
, nodeComponent([this](bool f) { window->setForceFocus(f); },
                [this]() {
                    makeDirty();
                    value.setNode(currentNode, nodeComponent.getValue());
                },
                [this]() {
                    if (value.nodes().size() > 1 &&
                        1 == bl::dialog::tinyfd_messageBox(
                                 "Warning", "Delete conversation node?", "yesno", "warning", 0)) {
                        value.deleteNode(currentNode);
                        if (currentNode >= value.nodes().size()) {
                            currentNode = value.nodes().size() - 1;
                        }
                        makeDirty();
                        window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
                    }
                },
                [this]() { treeComponent->update(value.nodes()); },
                [this](unsigned int current) {
                    core::file::Conversation::Node node{core::file::Conversation::Node::Type::Talk};
                    node.nextOnReject() = 999999;
                    node.nextOnPass()   = current;
                    value.appendNode(node);
                    return value.nodes().size() - 1;
                },
                std::bind(&ConversationWindow::setSelected, this, std::placeholders::_1), nodeBox,
                &value.nodes())
, filePicker(
      core::Properties::ConversationPath(), {core::Properties::ConversationFileExtension()},
      [this](const std::string& c) {
          std::string conv = c;
          if (bl::util::FileUtil::getExtension(conv) !=
                  core::Properties::ConversationFileExtension() &&
              filePickerMode != FilePickerMode::OpenExisting) {
              conv += "." + core::Properties::ConversationFileExtension();
          }

          filePicker.close();
          fileLabel->setText(conv);
          window->setForceFocus(true);
          switch (filePickerMode) {
          case FilePickerMode::MakeNew:
              value       = DefaultConversation;
              currentNode = 0;
              window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
              makeClean();
              break;
          case FilePickerMode::OpenExisting:
              currentNode = 0;
              if (!value.load(makePath(conv))) {
                  const std::string msg = "Failed to load conversation: " + conv;
                  bl::dialog::tinyfd_messageBox("Error", msg.c_str(), "ok", "error", 1);
                  value = DefaultConversation;
                  fileLabel->setText(EmptyFile);
              }
              makeClean();
              window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
              break;
          case FilePickerMode::SetFile:
          default:
              makeDirty();
              break;
          }
      },
      [this]() {
          filePicker.close();
          window->setForceFocus(true);
      })
, dirty(false) {
    window = Window::create(LinePacker::create(LinePacker::Vertical), "Conversation Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        window->remove();
        window->setForceFocus(false);
        cancelCb();
    });

    Box::Ptr row    = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    Button::Ptr but = Button::create("New");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        filePickerMode = FilePickerMode::MakeNew;
        if (!dirty || confirmDiscard()) {
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateNew, "New Conversation", parent, true);
        }
    });
    row->pack(but, false, true);
    but = Button::create("Open");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        filePickerMode = FilePickerMode::OpenExisting;
        if (!dirty || confirmDiscard()) {
            window->setForceFocus(false);
            filePicker.open(FilePicker::PickExisting, "Open Conversation", parent, true);
        }
    });
    row->pack(but, false, true);
    but = Button::create("Set File");
    but->setTooltip("Change the file to save to");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        filePickerMode = FilePickerMode::SetFile;
        window->setForceFocus(false);
        filePicker.open(FilePicker::CreateNew, "Change File", parent, true);
    });
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate()) {
            if (value.save(makePath(fileLabel->getText()))) { makeClean(); }
            else { bl::dialog::tinyfd_messageBox("Error", "Failed to save", "ok", "error", 1); }
        }
    });
    row->pack(saveBut, false, true);
    fileLabel = Label::create(EmptyFile);
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Transparent);
    row->pack(fileLabel, true, true);
    window->pack(row, true, false);

    treeComponent = ConversationTree::create(
        std::bind(&ConversationWindow::setSelected, this, std::placeholders::_1));
    treeComponent->setRequisition({500.f, 500.f});

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    row->pack(treeComponent, true, true);
    nodeBox->setRequisition({270.f, 500.f});
    nodeBox->setColor(sf::Color::Transparent, sf::Color::Black);
    nodeBox->setOutlineThickness(2.f);
    row->pack(nodeBox, true, true);
    window->pack(row, true, true);

    row = Box::create(LinePacker::create(
        LinePacker::Horizontal, 0.f, LinePacker::Compact, LinePacker::RightAlign));
    but = Button::create("Use Conversation");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate()) {
            if (!dirty || confirmDiscard()) {
                window->setForceFocus(false);
                window->remove();
                selectCb(fileLabel->getText());
            }
        }
    });
    but->setColor(sf::Color(52, 235, 222), sf::Color::Black);
    row->pack(but);
    window->pack(row, true, false);

    window->setPosition({100.f, 100.f});
}

void ConversationWindow::sync() {
    nodeComponent.update(currentNode, value.nodes().at(currentNode));
    treeComponent->update(value.nodes());
    treeComponent->setSelected(currentNode);
}

void ConversationWindow::open(bl::gui::GUI* p, const std::string& current) {
    parent      = p;
    currentNode = 0;
    if (current.empty() || !value.load(makePath(current))) { value = DefaultConversation; }

    nodeComponent.setParent(p);
    sync();
    fileLabel->setText(current);
    makeClean();

    parent->pack(window);
    window->setForceFocus(true);
}

bool ConversationWindow::confirmDiscard() const {
    return !dirty || 1 == bl::dialog::tinyfd_messageBox(
                              "Warning", "Discard unsaved changes?", "yesno", "warning", 0);
}

bool ConversationWindow::validate() const {
    const auto error = [](const std::string& e) {
        bl::dialog::tinyfd_messageBox("Error", e.c_str(), "ok", "error", 1);
    };

    const auto warning = [](const std::string& e) {
        bl::dialog::tinyfd_messageBox("Warning", e.c_str(), "ok", "warning", 1);
    };

    const auto checkNode = [&error, &warning](const core::file::Conversation::Node& n,
                                              unsigned int i) -> bool {
        using T        = core::file::Conversation::Node::Type;
        using OutputCb = std::function<void(const std::string&)>;

        const auto output = [i](const OutputCb& cb, const std::string& m) {
            std::string o = "Node " + std::to_string(i) + ": " + m;
            for (unsigned int i = 0; i < o.size(); ++i) {
                if (o[i] == '\'') o[i] = '`';
                if (o[i] == '"') {
                    o.erase(i, 1);
                    o.insert(i, "<QUOTE>");
                }
            }
            cb(o);
        };

        switch (n.getType()) {
        case T::Talk:
        case T::Prompt:
            if (n.message().empty()) { output(warning, "Message is empty"); }
            if (n.getType() == T::Prompt) {
                if (n.choices().empty()) {
                    output(error, "Prompt has no choices");
                    return false;
                }
                for (const auto& c : n.choices()) {
                    if (c.first.empty()) {
                        output(error, "Choice has empty text");
                        return false;
                    }
                }
            }
            return true;

        case T::TakeItem:
        case T::GiveItem:
            if (n.item().id == core::item::Id::Unknown) {
                output(error, "Invalid item");
                return false;
            }
            return true;

        case T::GiveMoney:
        case T::TakeMoney:
            if (n.money() == 0) {
                output(error, "Invalid money amount");
                return false;
            }
            return true;

        case T::RunScript: {
            bl::script::Script check(n.script());
            if (!check.valid()) {
                output(error, "SyntaxError: " + check.errorMessage());
                return false;
            }
        }
            return true;

        case T::SetSaveFlag:
        case T::CheckSaveFlag:
            if (n.saveFlag().empty()) {
                output(error, "Save flag cannot be empty");
                return false;
            }
            return true;

        case T::CheckInteracted:
            return true;

        default:
            output(error, "Unknown node type");
            return false;
        }
    };

    unsigned int validated = 0;
    std::queue<unsigned int> toVisit;
    std::vector<bool> visited(value.nodes().size(), false);

    toVisit.push(0);
    visited[0] = true;

    std::vector<unsigned int> next;
    next.reserve(8);
    while (!toVisit.empty()) {
        const unsigned int n                       = toVisit.front();
        const core::file::Conversation::Node& node = value.nodes()[n];
        toVisit.pop();
        validated += 1;

        if (!checkNode(node, n)) return false;
        if (!nodeTerminates(value.nodes(), n)) {
            error("Conversation end is unreachable from node " + std::to_string(n));
            return false;
        }

        core::file::Conversation::getNextJumps(node, next);
        if (next.empty()) {
            error("Node " + std::to_string(n) + " has no jumps");
            return false;
        }

        for (unsigned int j : next) {
            if (j >= value.nodes().size()) continue;
            if (!visited[j]) {
                visited[j] = true;
                toVisit.push(j);
            }
        }
    }

    if (validated != value.nodes().size()) {
        warning("There are " + std::to_string(value.nodes().size() - validated) +
                " unreachable nodes");
    }

    return true;
}

void ConversationWindow::makeClean() {
    dirty = false;
    saveBut->setColor(sf::Color::Green, sf::Color::Black);
}

void ConversationWindow::makeDirty() {
    dirty = true;
    saveBut->setColor(sf::Color::Red, sf::Color::Black);
}

void ConversationWindow::setSelected(unsigned int i) {
    if (i >= value.nodes().size()) {
        bl::dialog::tinyfd_messageBox("Error", "Invalid node", "ok", "error", 1);
    }
    else {
        currentNode = i;
        window->queueUpdateAction(
            [this]() { nodeComponent.update(currentNode, value.nodes().at(currentNode)); });
        treeComponent->setSelected(i);
    }
}

} // namespace component
} // namespace editor
