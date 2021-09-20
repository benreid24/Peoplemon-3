#include <BLIB/Files.hpp>
#include <BLIB/Interfaces.hpp>
#include <cstdlib>

namespace editor
{
namespace component
{
namespace
{
const char* getEditorPath() {
    static const char* WindowsPath = "tools\\AnimationEditor\\AnimationEditor.exe";
    static const char* UnixPath    = "tools/AnimationEditor/AnimationEditor";
    if (bl::file::Util::exists(UnixPath)) { return UnixPath; }
    return WindowsPath;
}

} // namespace

void openAnimationEditor() {
    const char* editor = getEditorPath();
    if (!bl::file::Util::exists(editor)) {
        bl::dialog::tinyfd_messageBox(
            "Animation Editor Missing",
            "Please download the animation editor and place it in the tools directory",
            "ok",
            "error",
            1);
        return;
    }
    std::system(editor);
}

} // namespace component
} // namespace editor
