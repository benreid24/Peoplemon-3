#include <Editor/Components/EventEditor.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

EventEditor::EventEditor(const OnEdit& oe)
: onEdit(oe)
, scriptSelector(std::bind(&EventEditor::onScriptChosen, this, std::placeholders::_1)) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 10), "Event Editor");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 8));
    scriptLabel  = Label::create("");
    scriptLabel->setColor(sf::Color::Blue, sf::Color::Transparent);
    row->pack(scriptLabel, true, true);
    Button::Ptr pickButton = Button::create("Pick/Edit Script");
    pickButton->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        scriptSelector.open(parent, scriptLabel->getText());
    });
    row->pack(pickButton, false, true);
    window->pack(row, true, false);

    row            = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Box::Ptr input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("X:"), false, true);
    xInput = TextEntry::create();
    input->pack(xInput, true, true);
    row->pack(input, true, true);
    input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("Y:"), false, true);
    yInput = TextEntry::create();
    input->pack(yInput, true, true);
    row->pack(input, true, true);
    window->pack(row, true, false);

    row   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("W:"), false, true);
    wInput = TextEntry::create();
    input->pack(wInput, true, true);
    row->pack(input, true, true);
    input = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    input->pack(Label::create("H:"), false, true);
    hInput = TextEntry::create();
    input->pack(hInput, true, true);
    row->pack(input, true, true);
    window->pack(row, true, false);

    row           = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    triggerSelect = ComboBox::create();
    triggerSelect->addOption("OnEnter");
    triggerSelect->addOption("OnExit");
    triggerSelect->addOption("OnEnterOrExit");
    triggerSelect->addOption("WhileIn");
    triggerSelect->addOption("OnInteract");
    row->pack(Label::create("Trigger:"), false, true);
    row->pack(triggerSelect, false, true);
    window->pack(row, true, false);

    row                 = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr editBut = Button::create("Confirm");
    editBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (valid()) {
            onEdit(orig, makeEvent());
            window->remove();
        }
    });
    row->pack(editBut, false, true);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
    row->pack(cancelBut, false, true);
    window->pack(row, true, false);
}

void EventEditor::open(const GUI::Ptr& p, const core::map::Event* source,
                       const sf::Vector2i& tiles) {
    parent = p;
    p->pack(window);
    orig = source;

    if (source) {
        scriptLabel->setText(source->script.getValue());
        xInput->setInput(std::to_string(source->position.getValue().x));
        yInput->setInput(std::to_string(source->position.getValue().y));
        wInput->setInput(std::to_string(source->areaSize.getValue().x));
        hInput->setInput(std::to_string(source->areaSize.getValue().y));
        triggerSelect->setSelectedOption(static_cast<int>(source->trigger.getValue()) - 1);
    }
    else {
        scriptLabel->setText("");
        xInput->setInput(std::to_string(tiles.x));
        yInput->setInput(std::to_string(tiles.y));
        wInput->setInput("1");
        hInput->setInput("1");
        triggerSelect->setSelectedOption(0);
    }
}

bool EventEditor::valid() const {
    const auto isNum = [](const std::string& s) -> bool {
        for (const char c : s) {
            if (c < '0' || c > '9') return false;
        }
        return true;
    };

    const auto err = [](const std::string& e) {
        bl::dialog::tinyfd_messageBox("Error", e.c_str(), "ok", "error", 1);
    };

    if (!isNum(xInput->getInput())) {
        err("X tile position is invalid");
        return false;
    }
    if (!isNum(yInput->getInput())) {
        err("Y tile position is invalid");
        return false;
    }
    if (!isNum(wInput->getInput())) {
        err("Region tile width is invalid");
        return false;
    }
    if (!isNum(hInput->getInput())) {
        err("Region tile height is invalid");
        return false;
    }
    return true;
}

core::map::Event EventEditor::makeEvent() const {
    core::map::Event e;
    e.script                = scriptLabel->getText();
    e.position.getValue().x = std::atoi(xInput->getInput().c_str());
    e.position.getValue().y = std::atoi(yInput->getInput().c_str());
    e.areaSize.getValue().x = std::atoi(wInput->getInput().c_str());
    e.areaSize.getValue().y = std::atoi(hInput->getInput().c_str());
    e.trigger = static_cast<core::map::Event::Trigger>(triggerSelect->getSelectedOption() + 1);
    return e;
}

void EventEditor::onScriptChosen(const std::string& s) { scriptLabel->setText(s); }

} // namespace component
} // namespace editor
