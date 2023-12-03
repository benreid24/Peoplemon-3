#include <Editor/Components/ScriptSelector.hpp>

#include <BLIB/Scripts.hpp>
#include <Core/Properties.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

ScriptSelector::ScriptSelector(const OnSelect& os, const OnCancel& oc)
: onSelect(os)
, onCancel(oc)
, picker(core::Properties::ScriptPath(), {"psc", "bs"},
         std::bind(&ScriptSelector::onPick, this, std::placeholders::_1), [this]() {
             picker.close();
             window->setForceFocus(true);
         }) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Select Script");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        window->remove();
        onCancel();
    });

    Button::Ptr pickBut = Button::create("Pick File");
    pickBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        picker.open(FilePicker::PickExisting, "Pick Script", parent, false);
    });
    window->pack(pickBut);

    scriptInput = TextEntry::create();
    scriptInput->setRequisition({250, 1});
    scriptInput->getSignal(Event::TextEntered).willAlwaysCall([this](const Event&, Element*) {
        checkSyntax();
    });
    window->pack(scriptInput, true, false);

    errorLabel = Label::create("");
    window->pack(errorLabel, true, false);

    Box::Ptr row       = Box::create(LinePacker::create(LinePacker::Horizontal, 10));
    Button::Ptr choose = Button::create("Select");
    choose->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->remove();
        onSelect(scriptInput->getInput());
    });
    row->pack(choose, false, true);
    Button::Ptr cancel = Button::create("Cancel");
    cancel->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        window->remove();
        onCancel();
    });
    row->pack(cancel, false, true);
    window->pack(row, true, false);
}

void ScriptSelector::open(GUI* p, const std::string& s) {
    parent = p;
    scriptInput->setInput(s);
    parent->pack(window);
    window->setForceFocus(true);
    checkSyntax();
}

void ScriptSelector::onPick(const std::string& s) {
    scriptInput->setInput(s);
    picker.close();
    window->setForceFocus(true);
    checkSyntax();
}

void ScriptSelector::checkSyntax() {
    bl::script::Script script(scriptInput->getInput());
    if (script.valid()) {
        errorLabel->setText("No error");
        errorLabel->setColor(sf::Color::Green, sf::Color::Transparent);
    }
    else {
        errorLabel->setText(script.errorMessage());
        errorLabel->setColor(sf::Color::Red, sf::Color::Transparent);
    }
}

} // namespace component
} // namespace editor
