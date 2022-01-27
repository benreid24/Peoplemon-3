#include <Editor/States/MainEditor.hpp>

#include <Core/Properties.hpp>

namespace editor
{
namespace state
{
MainEditor::Ptr MainEditor::create(core::system::Systems& s) { return Ptr(new MainEditor(s)); }

MainEditor::MainEditor(core::system::Systems& s)
: systems(s)
, mapPage(s)
, variousEditorsPage(s)
, scriptPage(s)
, testingPage(s)
, peoplemonPage(s)
, movesPage(s)
, itemsPage(s)
, creditsPage(s)
, currentPage(&mapPage) {
    gui = bl::gui::GUI::create(
        bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical, 4, bl::gui::LinePacker::Compact),
        {0.f,
         0.f,
         static_cast<float>(core::Properties::WindowWidth()) + 350.f,
         static_cast<float>(core::Properties::WindowHeight()) + 200.f});
    renderer = bl::gui::DefaultRenderer::create();
    gui->setRenderer(renderer);

    mapPage.registerGui(gui);
    variousEditorsPage.registerGui(gui);
    scriptPage.registerGui(gui);
    testingPage.registerGui(gui);
    peoplemonPage.registerGui(gui);
    movesPage.registerGui(gui);
    itemsPage.registerGui(gui);
    creditsPage.registerGui(gui);

    notebook = bl::gui::Notebook::create();
    notebook->setOutlineThickness(0.f);

    notebook->addPage("maps", "Maps", mapPage.getContent(), [this]() { currentPage = &mapPage; });
    notebook->addPage("editors", "Various Editors", variousEditorsPage.getContent(), [this]() {
        currentPage = &variousEditorsPage;
    });
    notebook->addPage(
        "script", "Scripts", scriptPage.getContent(), [this]() { currentPage = &scriptPage; });
    notebook->addPage(
        "test", "Game Testing", testingPage.getContent(), [this]() { currentPage = &testingPage; });
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
    gui->subscribe(systems.engine().eventBus());
    systems.engine().eventBus().subscribe(this);
}

void MainEditor::deactivate(bl::engine::Engine&) {
    systems.engine().eventBus().unsubscribe(gui.get());
    systems.engine().eventBus().unsubscribe(this);
}

void MainEditor::update(bl::engine::Engine&, float dt) {
    gui->update(dt);
    currentPage->update(dt);

    systems.clock().update(dt);
    systems.cameras().update(dt);
    systems.engine().entities().doDestroy();
}

void MainEditor::render(bl::engine::Engine& engine, float) {
    engine.window().clear();
    engine.window().draw(*gui);
    engine.window().display();
}

void MainEditor::observe(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        systems.engine().window().setView(sf::View({0.f,
                                                    0.f,
                                                    static_cast<float>(event.size.width),
                                                    static_cast<float>(event.size.height)}));
        gui->setRegion({0.f,
                        0.f,
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)});
    }
}

} // namespace state
} // namespace editor
