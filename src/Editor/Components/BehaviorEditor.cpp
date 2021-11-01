#include <Editor/Components/BehaviorEditor.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

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

BehaviorEditor::BehaviorEditor(const OnSetCb& cb, const NotifyWindowCb& op,
                               const NotifyWindowCb& oc)
: onSetCb(cb)
, onOpen(op)
, onClose(oc) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 8), "Behavior Editor");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) { hide(); });

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
    pathEditor.pack(page);
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
    setBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
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
            value.setType(T::FollowingPath);
            value.path() = pathEditor.getValue();
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
    cancelBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
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
    editBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        ogValue = value;
        onOpen();
        parent->pack(window);
        window->setForceFocus(true);
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
        pathEditor.init(behavior.path());
        break;

    case T::Wandering:
        notebook->makePageActive(3);
        radiusEntry->setInput(std::to_string(behavior.wander().radius));
        break;
    }
}

void BehaviorEditor::hide() {
    window->remove();
    window->setForceFocus(false);
    onClose();
}

const core::file::Behavior& BehaviorEditor::getValue() const { return value; }

BehaviorEditor::PathEditor::PathEditor() {
    container = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 6));
    container->setOutlineThickness(1);
    container->setColor(sf::Color::Transparent, sf::Color::Black);
    container->setMaxSize({0, 300});
    container->includeScrollbarsInRequisition(true);
}

void BehaviorEditor::PathEditor::pack(Box::Ptr parent) {
    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    toggle       = CheckButton::create("Reverse Path At End");
    toggle->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        value.reverse = toggle->getValue();
    });
    row->pack(toggle, false, true);
    parent->pack(row);
    parent->pack(Label::create("Paces:"));
    parent->pack(container, true, true);

    Button::Ptr appendBut = Button::create("Add Pace");
    appendBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        value.paces.emplace_back(core::component::Direction::Down, 1);
        sync();
    });
    parent->pack(appendBut);
}

void BehaviorEditor::PathEditor::init(const core::file::Behavior::Path& path) {
    value = path;
    sync();
}

const core::file::Behavior::Path& BehaviorEditor::PathEditor::getValue() const { return value; }

void BehaviorEditor::PathEditor::sync() {
    container->clearChildren(true);

    for (unsigned int i = 0; i < value.paces.size(); ++i) {
        auto& pace           = value.paces[i];
        Box::Ptr row         = Box::create(LinePacker::create(LinePacker::Horizontal));
        ComboBox::Ptr dirBox = ComboBox::create();
        dirBox->addOption("Up");
        dirBox->addOption("Right");
        dirBox->addOption("Down");
        dirBox->addOption("Left");
        dirBox->getSignal(Event::ValueChanged)
            .willAlwaysCall([&pace, dirBox](const Event&, Element*) {
                pace.direction =
                    static_cast<core::component::Direction>(dirBox->getSelectedOption());
            });
        dirBox->setSelectedOption(static_cast<int>(pace.direction));
        row->pack(dirBox, false, true);

        TextEntry::Ptr stepEntry = TextEntry::create();
        stepEntry->setInput(std::to_string(pace.steps));
        stepEntry->setColor(sf::Color::Green, sf::Color::Transparent);
        stepEntry->getSignal(Event::TextEntered)
            .willAlwaysCall([&pace, stepEntry](const Event&, Element*) {
                if (isNumber(stepEntry->getInput())) {
                    stepEntry->setColor(sf::Color::Green, sf::Color::Transparent);
                    pace.steps = std::atoi(stepEntry->getInput().c_str());
                }
                else {
                    stepEntry->setColor(sf::Color::Red, sf::Color::Transparent);
                }
            });
        stepEntry->setRequisition({80, 1});
        row->pack(Label::create("Steps:"), false, true);
        row->pack(stepEntry, true, true);

        Button::Ptr beforeBut = Button::create("Add Before");
        beforeBut->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
            value.paces.insert(value.paces.begin() + i, {core::component::Direction::Down, 1});
            sync();
        });
        row->pack(beforeBut, false, true);

        Button::Ptr afterBut = Button::create("Add After");
        afterBut->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
            value.paces.insert(value.paces.begin() + i + 1, {core::component::Direction::Down, 1});
            sync();
        });
        row->pack(afterBut, false, true);

        Button::Ptr delBut = Button::create("Remove");
        delBut->setColor(sf::Color::Red, sf::Color::Black);
        delBut->getSignal(Event::LeftClicked).willAlwaysCall([this, i](const Event&, Element*) {
            value.paces.erase(value.paces.begin() + i);
            sync();
        });
        row->pack(delBut, false, true);

        container->pack(row, false, true);
    }
}

} // namespace component
} // namespace editor
