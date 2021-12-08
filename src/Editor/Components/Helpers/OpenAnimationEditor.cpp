#include <BLIB/Interfaces.hpp>
#include <BLIB/Util/FileUtil.hpp>
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
    if (bl::util::FileUtil::exists(UnixPath)) { return UnixPath; }
    return WindowsPath;
}

} // namespace

void openAnimationEditor() {
    const char* editor = getEditorPath();
    if (!bl::util::FileUtil::exists(editor)) {
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
