#include <Editor/Components/TrainerEditorWindow.hpp>

#include <Core/Files/Trainer.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
namespace
{
const std::string EmptyFile = "<no file selected>";

std::string pplToStr(const core::pplmn::OwnedPeoplemon& ppl) {
    std::stringstream ss;
    ss << "Id: " << ppl.id() << ": " << core::pplmn::Peoplemon::name(ppl.id()) << " (Level "
       << ppl.currentLevel() << ")";
    return ss.str();
}

int parsePayout(const std::string& payout) {
    for (char c : payout) {
        if (c < '0' || c > '9') return -1;
    }
    const int p = std::atoi(payout.c_str());
    return p <= 200 ? p : -1;
}

} // namespace

using namespace bl::gui;
using FileUtil = bl::util::FileUtil;

TrainerEditorWindow::TrainerEditorWindow(const SelectCb& cb, const CloseCb& ccb)
: selectCb(cb)
, closeCb(ccb)
, clean(true)
, itemSelector(ItemSelector::create([this](core::item::Id item) { curItem = item; }))
, pplWindow(
      [this]() {
          window->setForceFocus(true);
          const auto val = pplWindow.getValue();
          if (editPplIndex.has_value()) {
              peoplemon[editPplIndex.value()] = val;
              pplBox->editOptionText(editPplIndex.value(), pplToStr(val));
          }
          else {
              peoplemon.emplace_back(val);
              pplBox->addOption(pplToStr(val));
          }
          makeDirty();
      },
      [this]() { window->setForceFocus(true); })
, filePicker(core::Properties::TrainerPath(), {"tnr"},
             std::bind(&TrainerEditorWindow::onChooseFile, this, std::placeholders::_1),
             [this]() {
                 filePicker.close();
                 window->setForceFocus(true);
             })
, animWindow(true, std::bind(&TrainerEditorWindow::onChooseAnimation, this, std::placeholders::_1),
             std::bind(&TrainerEditorWindow::forceWindowOnTop, this))
, behaviorEditor(
      std::bind(&TrainerEditorWindow::makeDirty, this), [this]() { window->setForceFocus(false); },
      std::bind(&TrainerEditorWindow::forceWindowOnTop, this))
, conversationWindow(
      std::bind(&TrainerEditorWindow::onChooseConversation, this, std::placeholders::_1),
      std::bind(&TrainerEditorWindow::forceWindowOnTop, this)) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "Trainer Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) { hide(); });

    Box::Ptr row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr newBut = Button::create("New");
    newBut->setTooltip("Create a new Trainer file");
    newBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            makingNew = true;
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateNew, "New Trainer", parent, true);
        }
    });
    Button::Ptr setBut = Button::create("Set File");
    setBut->setTooltip("Change the file to save this Trainer to");
    setBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (fileLabel->getText() == EmptyFile || confirmDiscard()) {
            makingNew = true;
            window->setForceFocus(false);
            filePicker.open(FilePicker::CreateOrPick, "Set Trainer File", parent, false);
        }
    });
    Button::Ptr openBut = Button::create("Open");
    openBut->setTooltip("Open a different Trainer file");
    openBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            makingNew = false;
            window->setForceFocus(false);
            filePicker.open(FilePicker::PickExisting, "Open Trainer", parent, false);
        }
    });
    saveBut = Button::create("Save");
    saveBut->setTooltip("Overwrite the current Trainer file");
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    saveBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validate(true)) {
            core::file::Trainer trainer;
            trainer.name                   = nameEntry->getInput();
            trainer.prebattleConversation  = bbLabel->getText();
            trainer.postBattleConversation = abLabel->getText();
            trainer.lostBattleLine         = loseLineEntry->getInput();
            trainer.animation              = animLabel->getText();
            trainer.behavior               = behaviorEditor.getValue();
            trainer.visionRange            = visionRangeEntry->getSelectedOption();
            trainer.items                  = items;
            trainer.peoplemon              = peoplemon;
            trainer.payout                 = parsePayout(payoutEntry->getInput());
            if (!trainer.save(bl::util::FileUtil::joinPath(core::Properties::TrainerPath(),
                                                           fileLabel->getText()))) {
                bl::dialog::tinyfd_messageBox("Error", "Failed to save Trainer", "ok", "error", 1);
            }
            else { makeClean(); }
        }
    });
    fileLabel = Label::create("");
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(newBut, false, true);
    row->pack(setBut, false, true);
    row->pack(openBut, false, true);
    row->pack(saveBut, false, true);
    row->pack(fileLabel, true, true);
    window->pack(row, true, false);

    row              = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Label::Ptr label = Label::create("Name:");
    nameEntry        = TextEntry::create();
    nameEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&TrainerEditorWindow::makeDirty, this));
    row->pack(label, false, true);
    row->pack(nameEntry, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr animBut = Button::create("Select Anim");
    animBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        animWindow.open(parent, core::Properties::CharacterAnimationPath(), animLabel->getText());
    });
    animLabel = Label::create("animation.anim");
    animLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(animBut, false, true);
    row->pack(animLabel, true, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr convBut = Button::create("Before Battle Conversation");
    convBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        selectingBB = true;
        conversationWindow.open(parent, bbLabel->getText());
    });
    bbLabel = Label::create("conversation.conv");
    bbLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(convBut, false, true);
    row->pack(bbLabel, true, true);
    window->pack(row, true, false);

    row     = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    convBut = Button::create("After Battle Conversation");
    convBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        selectingBB = false;
        conversationWindow.open(parent, abLabel->getText());
    });
    abLabel = Label::create("conversation.conv");
    abLabel->setColor(sf::Color::Cyan, sf::Color::Cyan);
    row->pack(convBut, false, true);
    row->pack(abLabel, true, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Lose Line:"), false, true);
    loseLineEntry = TextEntry::create();
    loseLineEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&TrainerEditorWindow::makeDirty, this));
    row->pack(loseLineEntry, true, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    behaviorEditor.pack(row);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Vision Range (tiles):"), false, true);
    visionRangeEntry = ComboBox::create();
    visionRangeEntry->setMaxHeight(300.f);
    for (int i = 0; i <= 20; ++i) { visionRangeEntry->addOption(std::to_string(i)); }
    visionRangeEntry->setSelectedOption(4);
    visionRangeEntry->getSignal(Event::ValueChanged)
        .willAlwaysCall(std::bind(&TrainerEditorWindow::makeDirty, this));
    row->pack(visionRangeEntry, false, true);
    window->pack(row);

    Label::Ptr l = Label::create("Items:");
    l->setHorizontalAlignment(RenderSettings::Alignment::Left);
    window->pack(l, true, false);
    row           = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    itemSelectBox = SelectBox::create();
    itemSelectBox->setMaxSize({1200.f, 90.f});
    itemSelectBox->setRequisition({250.f, 108.f});
    row->pack(itemSelectBox, true, true);
    Box::Ptr column = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    Box::Ptr subRow = Box::create(LinePacker::create(LinePacker::Horizontal));
    subRow->pack(itemSelector);
    Button::Ptr but = Button::create("Add");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (curItem != core::item::Id::Unknown) {
            items.push_back(curItem);
            itemSelectBox->addOption(core::item::Item::getName(curItem));
            makeDirty();
        }
    });
    subRow->pack(but);
    column->pack(subRow, true, false);
    but = Button::create("Remove");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const auto sel = itemSelectBox->getSelectedOption();
        if (sel.has_value()) {
            items.erase(items.begin() + sel.value());
            itemSelectBox->removeOption(sel.value());
            makeDirty();
        }
    });
    but->setColor(sf::Color::Red, sf::Color::Black);
    column->pack(but);
    row->pack(column, true, true);
    window->pack(row, true, false);

    l = Label::create("Peoplemon:");
    l->setHorizontalAlignment(RenderSettings::Alignment::Left);
    window->pack(l, true, false);
    row    = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    pplBox = SelectBox::create();
    pplBox->setMaxSize({1200.f, 90.f});
    pplBox->setRequisition({250.f, 108.f});
    row->pack(pplBox, true, true);
    column = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));
    but    = Button::create("Add");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (peoplemon.size() < 6) {
            editPplIndex.reset();
            window->setForceFocus(false);
            pplWindow.show(parent);
        }
    });
    column->pack(but);
    but = Button::create("Edit");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        editPplIndex = pplBox->getSelectedOption();
        window->setForceFocus(false);
        pplWindow.show(parent,
                       editPplIndex.has_value() ? peoplemon[editPplIndex.value()] :
                                                  core::pplmn::OwnedPeoplemon());
    });
    column->pack(but);
    but = Button::create("Remove");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        const auto sel = pplBox->getSelectedOption();
        if (sel.has_value()) {
            peoplemon.erase(peoplemon.begin() + sel.value());
            pplBox->removeOption(sel.value());
            makeDirty();
        }
    });
    but->setColor(sf::Color::Red, sf::Color::Black);
    column->pack(but);
    row->pack(column, false, true);
    window->pack(row, true, false);

    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Battle Base Payout (0-200):"), false, true);
    payoutEntry = TextEntry::create();
    payoutEntry->setInput("40");
    payoutEntry->setRequisition({60.f, 10.f});
    row->pack(payoutEntry, false, true);
    window->pack(row, true, false);

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr selectBut = Button::create("Use Trainer");
    selectBut->setTooltip("Use the current Trainer file");
    selectBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (confirmDiscard()) {
            if (validate(false)) {
                selectCb(fileLabel->getText());
                hide();
            }
        }
    });
    selectBut->setColor(sf::Color::Blue, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        hide();
    });
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(selectBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void TrainerEditorWindow::show(GUI* p, const std::string& file) {
    parent = p;
    reset();
    if (!file.empty()) {
        fileLabel->setText(file);
        load(file);
    }
    makeClean();
    parent->pack(window);
    window->setForceFocus(true);
    itemSelector->refresh();
}

void TrainerEditorWindow::onChooseFile(const std::string& file) {
    const std::string f =
        bl::util::FileUtil::getExtension(file) == core::Properties::TrainerFileExtension() ?
            file :
            file + "." + core::Properties::TrainerFileExtension();
    filePicker.close();
    window->setForceFocus(true);
    if (makingNew) { makeDirty(); }
    else {
        reset();
        load(f);
        makeClean();
    }
    fileLabel->setText(f);
}

void TrainerEditorWindow::makeClean() {
    saveBut->setColor(sf::Color::Green, sf::Color::Blue);
    clean = true;
}

void TrainerEditorWindow::makeDirty() {
    saveBut->setColor(sf::Color::Yellow, sf::Color::Blue);
    clean = false;
}

void TrainerEditorWindow::reset() {
    fileLabel->setText(EmptyFile);
    nameEntry->setInput("");
    animLabel->setText("<no anim selected>");
    bbLabel->setText("<no conv selected>");
    abLabel->setText("<no conv selected>");
    loseLineEntry->setInput("");
    behaviorEditor.configure(parent, {});
    visionRangeEntry->setSelectedOption(4);
    items.clear();
    itemSelectBox->clearOptions();
    pplBox->clearOptions();
    peoplemon.clear();
    payoutEntry->setInput("40");
}

void TrainerEditorWindow::load(const std::string& file) {
    core::file::Trainer trainer;
    if (trainer.load(FileUtil::joinPath(core::Properties::TrainerPath(), file))) {
        nameEntry->setInput(trainer.name);
        animLabel->setText(trainer.animation);
        bbLabel->setText(trainer.prebattleConversation);
        abLabel->setText(trainer.postBattleConversation);
        loseLineEntry->setInput(trainer.lostBattleLine);
        visionRangeEntry->setSelectedOption(trainer.visionRange);
        behaviorEditor.configure(parent, trainer.behavior);
        items = trainer.items;
        for (const auto item : items) { itemSelectBox->addOption(core::item::Item::getName(item)); }
        peoplemon = trainer.peoplemon;
        for (const auto& ppl : peoplemon) { pplBox->addOption(pplToStr(ppl)); }
        payoutEntry->setInput(std::to_string(trainer.payout));
    }
    else {
        bl::dialog::tinyfd_messageBox(
            "Error", std::string("Failed to load Trainer:\n" + file).c_str(), "ok", "error", 1);
    }
}

bool TrainerEditorWindow::validate(bool saving) const {
    if (saving) {
        if (fileLabel->getText() == EmptyFile) {
            bl::dialog::tinyfd_messageBox("Warning", "Please select a file", "ok", "warning", 1);
            return false;
        }
    }
    else {
        if (!FileUtil::exists(
                FileUtil::joinPath(core::Properties::TrainerPath(), fileLabel->getText()))) {
            bl::dialog::tinyfd_messageBox("Warning", "Bad file selected", "ok", "warning", 1);
            return false;
        }
    }
    if (nameEntry->getInput().empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Enter a name", "ok", "warning", 1);
        return false;
    }
    if (!FileUtil::directoryExists(
            FileUtil::joinPath(core::Properties::CharacterAnimationPath(), animLabel->getText()))) {
        bl::dialog::tinyfd_messageBox("Warning", "Bad animation", "ok", "warning", 1);
        return false;
    }
    std::string p = FileUtil::joinPath(core::Properties::ConversationPath(), bbLabel->getText());
    if (!FileUtil::exists(p)) {
        BL_LOG_INFO << p;
        bl::dialog::tinyfd_messageBox(
            "Warning", "Bad before-battle conversation", "ok", "warning", 1);
        return false;
    }
    p = FileUtil::joinPath(core::Properties::ConversationPath(), abLabel->getText());
    if (!FileUtil::exists(p)) {
        BL_LOG_INFO << p;
        bl::dialog::tinyfd_messageBox(
            "Warning", "Bad afer-battle conversation", "ok", "warning", 1);
        return false;
    }
    if (loseLineEntry->getInput().empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Lose line is empty", "ok", "warning", 1);
        return false;
    }
    if (behaviorEditor.getValue().type() == core::file::Behavior::Type::FollowingPath &&
        behaviorEditor.getValue().path().paces.empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Behavior path is empty", "ok", "warning", 1);
        return false;
    }
    if (peoplemon.empty()) {
        bl::dialog::tinyfd_messageBox("Warning", "Trainer has no peoplemon", "ok", "warning", 1);
        return false;
    }
    if (parsePayout(payoutEntry->getInput()) <= 0) {
        bl::dialog::tinyfd_messageBox(
            "Warning", "Payout must be number in range (0, 200]", "ok", "warning", 1);
        return false;
    }
    return true;
}

bool TrainerEditorWindow::confirmDiscard() const {
    if (!clean) {
        return 1 == bl::dialog::tinyfd_messageBox(
                        "Warning", "Discard unsaved changes?", "yesno", "warning", 0);
    }
    return true;
}

void TrainerEditorWindow::hide() {
    window->remove();
    filePicker.close();
    animWindow.hide();
    behaviorEditor.hide();
    window->setForceFocus(false);
    closeCb();
}

void TrainerEditorWindow::onChooseAnimation(const std::string& f) {
    animLabel->setText(f);
    makeDirty();
}

void TrainerEditorWindow::onChooseConversation(const std::string& c) {
    if (selectingBB) { bbLabel->setText(c); }
    else { abLabel->setText(c); }
    window->setForceFocus(true);
    makeDirty();
}

void TrainerEditorWindow::forceWindowOnTop() { window->setForceFocus(true); }

} // namespace component
} // namespace editor
