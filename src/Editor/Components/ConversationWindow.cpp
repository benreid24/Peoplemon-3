#include <Editor/Components/ConversationWindow.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;
namespace
{
const std::string EmptyFile = "<no file selected>";

const core::file::Conversation DefaultConversation = []() {
    core::file::Conversation val;
    val.appendNode({core::file::Conversation::Node::Type::Talk});
    return val;
}();

} // namespace

ConversationWindow::ConversationWindow(const SelectCb& onSelect, const CancelCb& onCancel)
: selectCb(onSelect)
, cancelCb(onCancel)
, value(DefaultConversation)
, currentNode(0)
, nodeBox(Box::create(LinePacker::create(LinePacker::Vertical, 4.f)))
, nodeComponent(
      [this]() {
          makeDirty();
          value.setNode(currentNode, nodeComponent.getValue());
      },
      [this]() {
          if (value.nodes().size() > 1 &&
              1 == bl::dialog::tinyfd_messageBox(
                       "Warning", "Delete conversation node?", "yesno", "warning", 0)) {
              value.deleteNode(currentNode);
              makeDirty();
              window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
          }
      },
      [this]() { treeComponent->update(value.nodes()); },
      [this]() {
          value.appendNode({core::file::Conversation::Node::Type::Talk});
          return value.nodes().size() - 1;
      },
      nodeBox, &value.nodes())
, filePicker(
      core::Properties::ConversationPath(), {"conv"},
      [this](const std::string& conv) {
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
              if (!value.load(conv)) {
                  const std::string msg = "Failed to load conversation: " + conv;
                  bl::dialog::tinyfd_messageBox("Error", msg.c_str(), "ok", "error", 1);
                  value       = DefaultConversation;
                  currentNode = 0;
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
            if (value.save(fileLabel->getText())) { makeClean(); }
            else {
                bl::dialog::tinyfd_messageBox("Error", "Failed to save", "ok", "error", 1);
            }
        }
    });
    row->pack(saveBut, false, true);
    fileLabel = Label::create(EmptyFile);
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Transparent);
    row->pack(fileLabel, true, true);
    window->pack(row, true, false);

    treeComponent = ConversationTree::create([this](unsigned int i) {
        if (i >= value.nodes().size()) {
            bl::dialog::tinyfd_messageBox("Error", "Invalid node", "ok", "error", 1);
        }
        else {
            currentNode = i;
            window->queueUpdateAction(
                [this]() { nodeComponent.update(value.nodes().at(currentNode)); });
        }
    });
    treeComponent->setRequisition({500.f, 500.f});

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    row->pack(treeComponent, true, true);
    nodeBox->setRequisition({250.f, 500.f});
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
}

void ConversationWindow::sync() {
    nodeComponent.update(value.nodes().at(currentNode));
    treeComponent->update(value.nodes());
}

void ConversationWindow::open(const bl::gui::GUI::Ptr& p, const std::string& current) {
    parent      = p;
    currentNode = 0;
    if (current.empty() || !value.load(current)) { value = DefaultConversation; }

    sync();
    makeClean();

    parent->pack(window);
    window->setForceFocus(true);
}

bool ConversationWindow::confirmDiscard() const {
    return 1 == bl::dialog::tinyfd_messageBox(
                    "Warning", "Discard unsaved changes?", "yesno", "warning", 0);
}

bool ConversationWindow::validate() const {
    // TODO - validate
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

} // namespace component
} // namespace editor
