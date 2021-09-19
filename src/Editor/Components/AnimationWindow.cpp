#include <Editor/Components/AnimationWindow.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <cstdlib>
#include <sstream>

namespace editor
{
namespace component
{
namespace
{
const char* getEditorPath() {
    static const char* WindowsPath = "tools/AnimationEditor/AnimationEditor.exe";
    static const char* UnixPath    = "tools/AnimationEditor/AnimationEditor";
    if (bl::file::Util::exists(UnixPath)) { return UnixPath; }
    return WindowsPath;
}

bool validFile(const std::string& f) { return !f.empty() && bl::file::Util::exists(f); }

} // namespace

using namespace bl::gui;
AnimationWindow::AnimationWindow(const ChooseCb& cb)
: chooseCb(cb) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Animation Picker");

    Box::Ptr row        = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickBut = Button::create("Choose Animation");
    pickBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        filePicker.emplace(
            path,
            std::vector<std::string>{"anim"},
            [this](const std::string& f) {
                fileLabel->setText(f);
                animSrc = bl::engine::Resources::animations()
                              .load(bl::file::Util::joinPath(path, f))
                              .data;
                if (animSrc) {
                    Animation::Ptr anim = Animation::create(animSrc);
                    sf::Vector2f size   = animSrc->getMaxSize();
                    if (size.x > 400.f) {
                        size.y *= 400.f / size.x;
                        size.x = 400.f;
                        anim->scaleToSize(size);
                    }
                    if (size.y > 400.f) {
                        size.x *= 400.f / size.y;
                        size.y = 400.f;
                        anim->scaleToSize(size);
                    }
                    animBox->clearChildren(true);
                    animBox->pack(anim);
                }
            },
            [this]() {
                filePicker.get().close();
                filePicker.destroy();
            });
    });
    fileLabel = Label::create("file here");
    fileLabel->setColor(sf::Color::Blue, sf::Color::Transparent);
    row->pack(pickBut, false, true);
    row->pack(fileLabel, true, true);
    window->pack(row);

    Button::Ptr editBut = Button::create("Open Editor");
    editBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
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
        std::stringstream cmd;
        cmd << editor << " ";
        if (validFile(fileLabel->getText())) { cmd << fileLabel->getText(); }
        std::system(cmd.str().c_str());
    });
    window->pack(editBut, false, false);

    animBox = Box::create(LinePacker::create());
    window->pack(animBox);

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr chooseBut = Button::create("Use Animation");
    chooseBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (fileLabel->getText().empty() || !bl::file::Util::exists(fileLabel->getText())) {
            bl::dialog::tinyfd_messageBox(
                "No File", "Please select an animation", "ok", "error", 1);
            return;
        }

        chooseCb(fileLabel->getText());
        window->remove();
    });
    chooseBut->setColor(sf::Color::Green, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        window->remove();
    });
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(chooseBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row);
}

void AnimationWindow::open(const GUI::Ptr& p, const std::string& pt, const std::string& file) {
    fileLabel->setText(file);
    path   = pt;
    parent = p;
    parent->pack(window);
}

} // namespace component
} // namespace editor
