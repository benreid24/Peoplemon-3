#ifndef EDITOR_PAGES_SCRIPTS_HPP
#define EDITOR_PAGES_SCRIPTS_HPP

#include <Editor/Pages/Page.hpp>

namespace editor
{
namespace page
{
/**
 * @brief Scripting validation tab
 *
 * @ingroup Pages
 *
 */
class Scripts : public Page {
public:
    /**
     * @brief Creates a new scripting validation tab
     *
     * @param systems The main game systems
     */
    Scripts(core::system::Systems& systems);

    /**
     * @brief Destroy the Scripts object
     *
     */
    virtual ~Scripts() = default;

    /**
     * @brief Refreshes if called for first time
     *
     */
    virtual void update(float dt) override;

private:
    struct Script {
        std::string file;
        std::string source;
        std::optional<std::string> error;

        bool operator<(const Script& rhs) const;
    };

    bl::gui::Window::Ptr window;
    bl::gui::Label::Ptr file;
    bl::gui::Label::Ptr source;
    bl::gui::Label::Ptr error;

    bl::gui::ScrollArea::Ptr scriptArea;
    std::vector<Script> scripts;
    bool started;

    void refresh();
    void openWindow(unsigned int i);
};

} // namespace page
} // namespace editor

#endif
