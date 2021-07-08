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
    using AppendCb = std::function<void(unsigned int level)>;
    using DeleteCb = std::function<void(unsigned int level, unsigned int layer)>;
    using ShiftCb  = std::function<void(unsigned int level, unsigned int layer, bool up)>;
    using RenderFilterCb =
        std::function<void(unsigned int level, unsigned int layer, bool visible)>;

    /**
     * @brief Construct a new Layers subpage
     *
     * @param bottomAddCb Called when a new bottom layer should be created
     * @param ysortAddCb Called when a new ysort layer should be created
     * @param topAddCb Called when a new top layer should be created
     * @param delCb Called when a layer should be deleted
     * @param shiftCb Called when a layer should be shifted
     * @param filterCb Called when layer visibility changes
     */
    Layers(const AppendCb& bottomAddCb, const AppendCb& ysortAddCb, const AppendCb& topAddCb,
           const DeleteCb& delCb, const ShiftCb& shiftCb, const RenderFilterCb& filterCb);

    /**
     * @brief Syncs the GUI elements with the map layers that exist
     *
     * @param levels The levels in the map
     * @param filter The current render filter
     */
    void sync(const std::vector<core::map::LayerSet>& levels,
              const std::vector<std::vector<bool>>& filter);

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
        using DeleteCb  = std::function<void(unsigned int layer)>;
        using ShiftCb   = std::function<void(unsigned int layer, bool up)>;

        bl::gui::Box::Ptr row;
        bl::gui::Label::Ptr name;
        bl::gui::CheckButton::Ptr visibleToggle;
        bl::gui::Button::Ptr upBut;
        bl::gui::Button::Ptr downBut;
        bl::gui::Button::Ptr delBut;
        unsigned int index;

        LayerRow(unsigned int i, bool canUp, bool canDown, bool visible, const VisibleCb& visibleCb,
                 const DeleteCb& delCb, const ShiftCb& shiftCb);
    };

    struct LevelTab {
        std::vector<LayerRow> items;
        unsigned int index;

        bl::gui::ScrollArea::Ptr page;
        bl::gui::Box::Ptr bottomBox;
        bl::gui::Box::Ptr ysortBox;
        bl::gui::Box::Ptr topBox;

        LevelTab(unsigned int i, const core::map::LayerSet& level, const std::vector<bool>& filter,
                 const RenderFilterCb& visibleCb, const AppendCb& bottomAddCb,
                 const AppendCb& ysortAddCb, const AppendCb& topAddCb, const DeleteCb& delCb,
                 const ShiftCb& shiftCb);
    };

    const AppendCb bottomAdd;
    const AppendCb ysortAdd;
    const AppendCb topAdd;
    const DeleteCb delCb;
    const ShiftCb shiftCb;
    const RenderFilterCb filterCb;

    bl::gui::Box::Ptr contentWrapper;
    bl::gui::Notebook::Ptr content;

    std::vector<LevelTab> pages;
};

} // namespace page
} // namespace editor

#endif
