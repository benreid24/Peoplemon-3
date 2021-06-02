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
, testingPage(s)
, peoplemonPage(s)
, movesPage(s)
, itemsPage(s)
, creditsPage(s)
, todoPage(s)
, currentPage(&mapPage) {
    gui = bl::gui::GUI::create(
        bl::gui::LinePacker::create(bl::gui::LinePacker::Vertical, 4, bl::gui::LinePacker::Compact),
        {0, 0, core::Properties::WindowWidth() + 300, core::Properties::WindowHeight() + 150},
        "editor",
        "gui");
    renderer = bl::gui::DefaultRenderer::create();
    gui->setRenderer(renderer);

    notebook = bl::gui::Notebook::create("editor", "main_nav");

    notebook->addPage("maps", "Maps", mapPage.getContent());
    notebook->addPage("test", "Game Testing", testingPage.getContent());
    notebook->addPage("peoplemon", "Peoplemon DB", peoplemonPage.getContent());
    notebook->addPage("moves", "Move DB", movesPage.getContent());
    notebook->addPage("items", "Item DB", itemsPage.getContent());
    notebook->addPage("credits", "Credits", creditsPage.getContent());
    notebook->addPage("todo", "TODO", todoPage.getContent());

    gui->pack(notebook, true, true);
}

MainEditor::~MainEditor() {
    // TODO - free pages
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

void MainEditor::update(bl::engine::Engine&, float dt) { gui->update(dt); }

void MainEditor::render(bl::engine::Engine& engine, float lag) {
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
        gui->setRegion(
            {0, 0, static_cast<int>(event.size.width), static_cast<int>(event.size.height)});
    }
}

} // namespace state
} // namespace editor
