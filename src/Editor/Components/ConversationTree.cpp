#include <Editor/Components/ConversationTree.hpp>

namespace editor
{
namespace component
{
using namespace bl::gui;

ConversationTree::ConversationTree(const ClickCb& clickCb, Box::Ptr& container)
: onClick(clickCb) {
    container->pack(Label::create("Tree here"));
}

void ConversationTree::update(const std::vector<core::file::Conversation::Node>&) {
    // TODO - generate and render tree
}

} // namespace component
} // namespace editor
