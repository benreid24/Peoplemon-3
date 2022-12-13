#ifndef EDITOR_COMPONENTS_PEOPLEMONEDITORWINDOW_HPP
#define EDITOR_COMPONENTS_PEOPLEMONEDITORWINDOW_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Files/PeoplemonDB.hpp>
#include <Core/Peoplemon/Peoplemon.hpp>
#include <Editor/Components/MoveSelector.hpp>
#include <Editor/Components/PeoplemonSelector.hpp>
#include <Editor/Components/StatBox.hpp>
#include <Editor/Components/TypeSelector.hpp>
#include <functional>

namespace editor
{
namespace component
{
class PeoplemonEditorWindow {
public:
    using OnChange = std::function<void()>;

    PeoplemonEditorWindow(core::file::PeoplemonDB& db, const OnChange& onChange);

    void open(const bl::gui::GUI::Ptr& parent, core::pplmn::Id ppl);

private:
    core::file::PeoplemonDB& peoplemonDb;
    const OnChange onChange;
    bool doingNewPeoplemon;

    bl::gui::Window::Ptr window;
    bl::gui::TextEntry::Ptr idEntry;
    bl::gui::TextEntry::Ptr nameEntry;
    bl::gui::TextEntry::Ptr descEntry;
    TypeSelector::Ptr type1Select;
    TypeSelector::Ptr type2Select;
    StatBox statEntry;

    std::vector<core::pplmn::MoveId> movePool;
    bl::gui::SelectBox::Ptr movePoolEntry;
    MoveSelector::Ptr movePoolSelect;

    std::vector<std::pair<unsigned int, core::pplmn::MoveId>> learnMoves;
    bl::gui::SelectBox::Ptr learnMoveEntry;
    bl::gui::TextEntry::Ptr moveLevelEntry;
    MoveSelector::Ptr moveLearnSelect;

    bl::gui::TextEntry::Ptr evolveLevelEntry;
    PeoplemonSelector::Ptr evolveIdSelect;

    bl::gui::ComboBox::Ptr abilitySelect;
    bl::gui::ComboBox::Ptr xpGroupSelect;
    bl::gui::TextEntry::Ptr xpRewardMultEntry;
    StatBox evAwardEntry;
    bl::gui::TextEntry::Ptr catchRateEntry;

    bl::resource::Ref<sf::Texture> thumbTxtr;
    bl::resource::Ref<sf::Texture> playerTxtr;
    bl::resource::Ref<sf::Texture> opTxtr;
    bl::gui::Image::Ptr thumbImg;
    bl::gui::Image::Ptr playerImg;
    bl::gui::Image::Ptr opImg;

    core::pplmn::Id openId;
    bool dirty;
    bl::gui::Button::Ptr applyBut;

    void reloadImages();
    void makeDirty();
    void onCancel();
    void onSave();
    void close();

    void onMovePoolAdd();
    void onMovePoolDelete();

    void onLearnMoveAdd();
    void onLearnMoveDelete();
};

} // namespace component
} // namespace editor

#endif
