#ifndef EDITOR_PAGES_SUBPAGES_LAYERS_HPP
#define EDITOR_PAGES_SUBPAGES_LAYERS_HPP

#include <BLIB/Interfaces/GUI.hpp>
#include <Core/Maps/Map.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Provides controls to add, move, remove, and toggle visibility for layers
 *
 * @ingroup Pages
 *
 */
class Layers {
public:
    using AppendCb       = std::function<void(unsigned int level)>;
    using DeleteCb       = std::function<void(unsigned int, unsigned int)>;
    using RenderFilterCb = std::function<void(const std::vector<std::vector<bool>>&)>;

    /**
     * @brief Construct a new Layers subpage
     *
     * @param bottomAddCb Called when a new bottom layer should be created
     * @param ysortAddCb Called when a new ysort layer should be created
     * @param topAddCb Called when a new top layer should be created
     * @param delCb Called when a layer should be deleted
     * @param filterCb Called when layer visibility changes
     */
    Layers(const AppendCb& bottomAddCb, const AppendCb& ysortAddCb, const AppendCb& topAddCb,
           const DeleteCb& delCb, const RenderFilterCb& filterCb);

    /**
     * @brief Syncs the GUI elements with the map layers that exist
     *
     * @param levels The levels in the map
     */
    void sync(const std::vector<core::map::LayerSet>& levels);

    /**
     * @brief Removes the level at the given index
     *
     * @param level The index of the removed level
     */
    void popLevel(unsigned int level);

    /**
     * @brief Adds a level at the given index
     *
     * @param level The index to insert the new level
     */
    void addLevel(unsigned int level);

    /**
     * @brief Returns the GUI content to pack
     *
     */
    bl::gui::Box::Ptr getContent();

    /**
     * @brief Packs the GUI content
     *
     */
    void pack();

    /**
     * @brief Hides the GUI content. Good for saving space when not active
     *
     */
    void unpack();

private:
    struct LayerRow {
        using VisibleCb = std::function<void(unsigned int layer, bool visible)>;

        bl::gui::Box::Ptr row;
        bl::gui::Label::Ptr name;
        bl::gui::CheckButton::Ptr visibleToggle;
        bl::gui::Button::Ptr upBut;
        bl::gui::Button::Ptr downBut;
        bl::gui::Button::Ptr delBut;
        unsigned int index;

        LayerRow(unsigned int i, const VisibleCb& visibleCb);
        void setIndex(unsigned int i);
    };

    struct LevelTab {
        using VisibleCb = std::function<void(unsigned int level, unsigned int layer, bool visible)>;

        std::vector<LayerRow> items;
        unsigned int index;

        bl::gui::ScrollArea::Ptr page;
        bl::gui::Box::Ptr bottomBox;
        bl::gui::Box::Ptr ysortBox;
        bl::gui::Box::Ptr topBox;

        LevelTab(unsigned int i, const core::map::LayerSet& level, const VisibleCb& visibleCb);
        void setIndex(unsigned int i);
    };

    const AppendCb bottomAdd;
    const AppendCb ysortAdd;
    const AppendCb topAdd;
    const DeleteCb delCb;
    const RenderFilterCb filterCb;

    bl::gui::Box::Ptr contentWrapper;
    bl::gui::Notebook::Ptr content;

    std::vector<LevelTab> pages;
    std::vector<std::vector<bool>> visible;
};

} // namespace page
} // namespace editor

#endif
