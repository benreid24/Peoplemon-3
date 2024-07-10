#include <Editor/States/MainEditor.hpp>

#include <Core/Properties.hpp>
#include <Editor/Components/ConversationTree.hpp>
#include <Editor/Components/EditMap.hpp>
#include <Editor/Components/HighlightRadioButton.hpp>
#include <Editor/Components/Render/ConversationTreeComponent.hpp>
#include <Editor/Components/Render/EditMapComponent.hpp>
#include <Editor/Components/Render/HighlightRadioButtonComponent.hpp>

namespace editor
{
namespace state
{
MainEditor::Ptr MainEditor::create(core::system::Systems& s) { return Ptr(new MainEditor(s)); }

MainEditor::MainEditor(core::system::Systems& s)
: State(bl::engine::StateMask::Editor)
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
         static_cast<float>(s.engine().window().getSfWindow().getSize().x),
         static_cast<float>(s.engine().window().getSfWindow().getSize().y)});

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

void MainEditor::activate(bl::engine::Engine& engine) {
    engine.renderer().getObserver().pushScene<bl::rc::Overlay>();
    engine.renderer().getObserver().setClearColor(bl::sfcol(sf::Color{90, 90, 90}));
    gui->addToOverlay();
    bl::event::Dispatcher::subscribe(this);
}

void MainEditor::deactivate(bl::engine::Engine& engine) {
    gui->removeFromScene();
    engine.renderer().getObserver().popScene();
    bl::event::Dispatcher::unsubscribe(this);
}

void MainEditor::update(bl::engine::Engine&, float dt, float) {
    gui->update(dt);
    currentPage->update(dt);
}

void MainEditor::observe(const sf::Event& event) {
    if (event.type == sf::Event::Resized) {
        gui->setRegion({0.f,
                        0.f,
                        static_cast<float>(event.size.width),
                        static_cast<float>(event.size.height)});
    }
}

void MainEditor::registerCustomGuiComponents() {
    using namespace component;
    auto& table = bl::gui::rdr::FactoryTable::getDefaultTable();

    table.registerFactoryForElement<HighlightRadioButton, rdr::HighlightRadioButtonComponent>();
    table.registerFactoryForElement<ConversationTree, rdr::ConversationTreeComponent>();
    table.registerFactoryForElement<EditMap, rdr::EditMapComponent>();
}

} // namespace state
} // namespace editor
