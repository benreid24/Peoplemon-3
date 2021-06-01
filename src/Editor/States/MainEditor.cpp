#include <Editor/States/MainEditor.hpp>

namespace editor
{
namespace state
{
MainEditor::Ptr MainEditor::create(core::system::Systems& s) { return Ptr(new MainEditor(s)); }

MainEditor::MainEditor(core::system::Systems& s)
: systems(s) {
    // TODO create pages and gui
}

MainEditor::~MainEditor() {
    // TODO - free pages
}

const char* MainEditor::name() const { return "MainEditor"; }

void MainEditor::activate(bl::engine::Engine&) {
    // TODO
}

void MainEditor::deactivate(bl::engine::Engine&) {
    // TODO
}

void MainEditor::update(bl::engine::Engine&, float dt) {
    // TODO
}

void MainEditor::render(bl::engine::Engine&, float lag) {
    // TODO
}

} // namespace state
} // namespace editor
