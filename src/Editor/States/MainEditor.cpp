#include <Editor/States/MainEditor.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace state
{
MainEditor::Ptr MainEditor::create(core::system::Systems& s) { return Ptr(new MainEditor(s)); }

MainEditor::MainEditor(core::system::Systems& s)
: State(bl::engine::StateMask::Paused)
, systems(s)
, mapPage(s)
, variousEditorsPage(s)
, scriptPage(s)
, peoplemonPage(s)
, movesPage(s)
, itemsPage(s)
, creditsPage(s)
, currentPage(&mapPage) {
    gui = bl::gui::GUI::create(
        s.engine(),
        s.engine().renderer().getObserver(),
        bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical, 4, bl::gui::LinePacker::Compact),
        {0.f,
         0.f,
         static_cast<float>(core::Properties::WindowWidth()) + 350.f,
         static_cast<float>(core::Properties::WindowHeight()) + 200.f});

    // TODO - BLIB_UPGRADE - add custom gui components for rendering

    mapPage.registerGui(gui.get());
    variousEditorsPage.registerGui(gui.get());
    scriptPage.registerGui(gui.get());
    peoplemonPage.registerGui(gui.get());
    movesPage.registerGui(gui.get());
    itemsPage.registerGui(gui.get());
    creditsPage.registerGui(gui.get());

    mapPage.syncGui();

    notebook = bl::gui::Notebook::create();
    notebook->setOutlineThickness(0.f);

    notebook->addPage("maps", "Maps", mapPage.getContent(), [this]() { currentPage = &mapPage; });
    notebook->addPage("editors", "Various Editors", variousEditorsPage.getContent(), [this]() {
        currentPage = &variousEditorsPage;
    });
    notebook->addPage(
        "script", "Scripts", scriptPage.getContent(), [this]() { currentPage = &scriptPage; });
    notebook->addPage("peoplemon", "Peoplemon DB", peoplemonPage.getContent(), [this]() {
        currentPage = &peoplemonPage;
    });
    notebook->addPage(
        "moves", "Move DB", movesPage.getContent(), [this]() { currentPage = &movesPage; });
    notebook->addPage(
        "items", "Item DB", itemsPage.getContent(), [this]() { currentPage = &itemsPage; });
    notebook->addPage(
        "credits", "Credits", creditsPage.getContent(), [this]() { currentPage = &creditsPage; });

    gui->pack(notebook, true, true);
}

const char* MainEditor::name() const { return "MainEditor"; }

void MainEditor::activate(bl::engine::Engine&) {
    // gui->subscribe();
    bl::event::Dispatcher::subscribe(this);
}

void MainEditor::deactivate(bl::engine::Engine&) {
    bl::event::Dispatcher::unsubscribe(gui.get());
    bl::event::Dispatcher::unsubscribe(this);
}

void MainEditor::update(bl::engine::Engine&, float dt, float) {
    gui->update(dt);
    currentPage->update(dt);
}

// void MainEditor::render(bl::engine::Engine& engine, float) {
//     engine.window().clear();
//     engine.window().draw(*gui);
//     engine.window().display();
// }

void MainEditor::observe(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        /*systems.engine().window().setView(sf::View({0.f,
                                                    0.f,
                                                    static_cast<float>(event.size.width),
                                                    static_cast<float>(event.size.height)}));*/
        gui->setRegion({0.f,
                        0.f,
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)});
    }
}

} // namespace state
} // namespace editor
