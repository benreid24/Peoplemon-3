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
}

ConversationWindow::ConversationWindow(const SelectCb& onSelect, const CancelCb& onCancel)
: selectCb(onSelect)
, cancelCb(onCancel)
, filePicker(
      core::Properties::ConversationPath(), {"conv"},
      [this](const std::string& conv) {
          window->setForceFocus(true);
          switch (filePickerMode) {
          case FilePickerMode::MakeNew:
              // TODO - clear value
              window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
              break;
          case FilePickerMode::OpenExisting:
              if (!value.load(
                      bl::file::Util::joinPath(core::Properties::ConversationPath(), conv))) {
                  // TODO - report error and clear
              }
              window->queueUpdateAction(std::bind(&ConversationWindow::sync, this));
              break;
          case FilePickerMode::SetFile:
          default:
              fileLabel->setText(conv);
              makeDirty();
              break;
          }
      },
      [this]() { window->setForceFocus(true); })
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
            filePicker.open(FilePicker::CreateNew, "Open Conversation", parent, true);
        }
    });
    row->pack(but, false, true);
    but = Button::create("Set File");
    but->setTooltip("Change the file to save to");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        filePickerMode = FilePickerMode::SetFile;
        if (!dirty || confirmDiscard()) {
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateNew, "Change File", parent, true);
        }
    });
    row->pack(but, false, true);
    saveBut = Button::create("Save");
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate()) {
            value.save(fileLabel->getText());
            makeClean();
        }
    });
    row->pack(saveBut, false, true);
    fileLabel = Label::create(EmptyFile);
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Transparent);
    row->pack(fileLabel, true, true);
    window->pack(row, true, false);

    row                  = Box::create(LinePacker::create(LinePacker::Horizontal, 8.f));
    Box::Ptr placeholder = Box::create(LinePacker::create());
    placeholder->setRequisition({500.f, 500.f});
    placeholder->setColor(sf::Color::Cyan, sf::Color::Black);
    placeholder->setOutlineThickness(2.f);
    row->pack(placeholder, true, true);
    placeholder = Box::create(LinePacker::create());
    placeholder->setRequisition({200.f, 500.f});
    placeholder->setColor(sf::Color::Magenta, sf::Color::Black);
    placeholder->setOutlineThickness(2.f);
    row->pack(placeholder, true, true);
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
    // TODO - sync gui
}

void ConversationWindow::open(const bl::gui::GUI::Ptr& p, const std::string& current) {
    parent = p;
    if (!current.empty()) { value.load(current); }
    else {
        // TODO - clear value
    }

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
