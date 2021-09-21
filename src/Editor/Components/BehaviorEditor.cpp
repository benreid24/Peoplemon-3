#include <Editor/Components/BehaviorEditor.hpp>

namespace editor
{
namespace component
{
namespace
{
std::string getLabel(core::file::Behavior::Type t) {
    using T = core::file::Behavior::Type;

    std::stringstream ss;
    ss << "Behavior: ";
    switch (t) {
    case T::FollowingPath:
        ss << "Fixed Path";
        break;
    case T::SpinInPlace:
        ss << "Spin";
        break;
    case T::StandStill:
        ss << "Stand Still";
        break;
    case T::Wandering:
        ss << "Wander";
        break;
    default:
        ss << "<unset>";
        break;
    }
    return ss.str();
}

bool isNumber(const std::string& i) {
    if (i.empty()) return false;
    for (char c : i) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

} // namespace

using namespace bl::gui;

BehaviorEditor::BehaviorEditor(const OnSetCb& cb)
: onSetCb(cb) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "NPC Editor");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) { hide(); });

    typeLabel = Label::create("");
    notebook  = Notebook::create();

    Box::Ptr page = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    Box::Ptr row  = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    dirBox        = ComboBox::create();
    dirBox->addOption("Up");
    dirBox->addOption("Right");
    dirBox->addOption("Down");
    dirBox->addOption("Left");
    dirBox->setSelectedOption(0);
    row->pack(Label::create("Direction:"));
    row->pack(dirBox);
    page->pack(row, true, true);
    notebook->addPage("stand", "Stand Still", page);

    page    = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row     = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    spinBox = ComboBox::create();
    spinBox->addOption("Clockwise");
    spinBox->addOption("Counter-Clockwise");
    spinBox->addOption("Random");
    spinBox->setSelectedOption(0);
    row->pack(Label::create("Spin Direction:"));
    row->pack(spinBox);
    page->pack(row, true, true);
    notebook->addPage("spin", "Spin", page);

    page = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    page->pack(Label::create("TODO")); // TODO - path editor
    notebook->addPage("path", "Path", page);

    page = Box::create(LinePacker::create(LinePacker::Vertical, 4));
    row  = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    row->pack(Label::create("Radius (tiles):"), false, true);
    radiusEntry = TextEntry::create();
    radiusEntry->setInput("7");
    row->pack(radiusEntry, true, true);
    page->pack(row, true, true);
    notebook->addPage("wander", "Wander", page);

    row                = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr setBut = Button::create("Save");
    setBut->setColor(sf::Color::Green, sf::Color::Black);
    setBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        using T = core::file::Behavior::Type;
        switch (notebook->getActivePageIndex()) {
        case 0:
            value.setType(T::StandStill);
            value.standing().facedir =
                static_cast<core::component::Direction>(dirBox->getSelectedOption());
            break;

        case 1:
            value.setType(T::SpinInPlace);
            value.spinning().spinDir = static_cast<core::file::Behavior::Spinning::Direction>(
                spinBox->getSelectedOption());
            break;

        case 2:
            // TODO - path
            break;

        case 3:
            if (isNumber(radiusEntry->getInput())) {
                value.setType(T::Wandering);
                value.wander().radius = std::atoi(radiusEntry->getInput().c_str());
            }
            else {
                bl::dialog::tinyfd_messageBox(
                    "Error", "Radius must be a positive integer", "ok", "error", 1);
                return;
            }
        }

        typeLabel->setText(getLabel(value.type()));
        onSetCb();
        hide();
    });
    row->pack(setBut, false, true);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        value = ogValue;
        hide();
    });
    row->pack(cancelBut, false, true);
    window->pack(notebook, true, true);
    window->pack(row, true, false);
}

void BehaviorEditor::pack(Box::Ptr row) {
    row->pack(typeLabel, true, true);
    Button::Ptr editBut = Button::create("Edit Behavior");
    editBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        ogValue = value;
        parent->pack(window);
    });
    row->pack(editBut, false, true);
}

void BehaviorEditor::configure(GUI::Ptr p, const core::file::Behavior& behavior) {
    using T = core::file::Behavior::Type;

    parent = p;
    value = ogValue = behavior;

    typeLabel->setText(getLabel(behavior.type()));
    switch (behavior.type()) {
    case T::StandStill:
        notebook->makePageActive(0);
        dirBox->setSelectedOption(static_cast<int>(behavior.standing().facedir));
        break;

    case T::SpinInPlace:
        notebook->makePageActive(1);
        spinBox->setSelectedOption(static_cast<int>(behavior.spinning().spinDir));
        break;

    case T::FollowingPath:
        notebook->makePageActive(2);
        // TODO - path
        break;

    case T::Wandering:
        notebook->makePageActive(3);
        radiusEntry->setInput(std::to_string(behavior.wander().radius));
        break;
    }
}

void BehaviorEditor::hide() { window->remove(); }

const core::file::Behavior& BehaviorEditor::getValue() const { return value; }

} // namespace component
} // namespace editor
