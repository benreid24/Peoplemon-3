#include <Editor/Pages/Scripts.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

Scripts::Scripts(core::system::Systems& s)
: Page(s)
, started(false) {
    content = Box::create(LinePacker::create(LinePacker::Vertical));

    Button::Ptr rbut = Button::create("Refresh");
    rbut->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&Scripts::refresh, this));
    content->pack(rbut);

    scriptArea = ScrollArea::create(LinePacker::create(LinePacker::Vertical, 4.f));
    scriptArea->setColor(sf::Color::White, sf::Color::Black);
    scriptArea->setOutlineThickness(2.f);
    content->pack(scriptArea, true, true);

    window = Window::create(LinePacker::create(LinePacker::Vertical, 6.f), "Peoplemon Script");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) {
        window->setForceFocus(false);
        window->remove();
    });
    file = Label::create("");
    file->setColor(sf::Color(20, 230, 245), sf::Color::Transparent);
    window->pack(file);
    error = Label::create("");
    window->pack(error);
    ScrollArea::Ptr sourceScroll = ScrollArea::create(LinePacker::create());
    sourceScroll->setRequisition({300.f, 200.f});
    sourceScroll->setMaxSize({700.f, 700.f});
    sourceScroll->setColor(sf::Color::White, sf::Color::Black);
    sourceScroll->setOutlineThickness(2.f);
    Box::Ptr padded = Box::create(LinePacker::create());
    padded->setColor(sf::Color::White, sf::Color::Black);
    source = Label::create("");
    padded->pack(source);
    sourceScroll->pack(padded);
    window->pack(sourceScroll, true, true);
}

void Scripts::update(float) {
    if (!started) {
        started = true;
        refresh();
    }
}

void Scripts::refresh() {
    scripts.clear();
    scriptArea->clearChildren(true);

    const std::vector<std::string> files =
        bl::util::FileUtil::listDirectory(core::Properties::ScriptPath(), "", true);
    scripts.reserve(files.size());
    for (const std::string& file : files) {
        scripts.emplace_back();
        scripts.back().file = file;
        if (!bl::util::FileUtil::readFile(file, scripts.back().source)) {
            scripts.back().error.emplace("Failed to read file");
        }
        else {
            bl::script::Script test(file);
            if (!test.valid()) { scripts.back().error.emplace(test.errorMessage()); }
        }
    }

    std::sort(scripts.begin(), scripts.end());

    for (unsigned int i = 0; i < scripts.size(); ++i) {
        Label::Ptr label = Label::create(scripts[i].file);
        if (scripts[i].error.has_value()) {
            label->setColor(sf::Color::Red, sf::Color::Transparent);
        }
        else {
            label->setColor(sf::Color::Green, sf::Color::Transparent);
        }
        label->getSignal(Event::LeftClicked)
            .willAlwaysCall(std::bind(&Scripts::openWindow, this, i));
        label->setHorizontalAlignment(RenderSettings::Left);
        scriptArea->pack(label, true, false);
    }
}

void Scripts::openWindow(unsigned int i) {
    file->setText(scripts[i].file);
    source->setText(scripts[i].source);
    error->setText("Error: " + scripts[i].error.value_or("No errors detected"));
    if (scripts[i].error.has_value()) { error->setColor(sf::Color::Red, sf::Color::Transparent); }
    else {
        error->setColor(sf::Color::Green, sf::Color::Transparent);
    }
    parent->pack(window);
    window->setForceFocus(true);
}

bool Scripts::Script::operator<(const Script& rhs) const {
    if (error.has_value()) {
        if (rhs.error.has_value()) { return file < rhs.file; }
        return false;
    }
    else if (rhs.error.has_value()) {
        return false;
    }
    return file < rhs.file;
}

} // namespace page
} // namespace editor
