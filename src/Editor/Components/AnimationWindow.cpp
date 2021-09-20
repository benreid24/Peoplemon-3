#include <Editor/Components/AnimationWindow.hpp>

#include <BLIB/Engine/Resources.hpp>
#include <cstdlib>
#include <sstream>

namespace editor
{
namespace component
{
using namespace bl::gui;

namespace
{
const char* getEditorPath() {
    static const char* WindowsPath = "tools\\AnimationEditor\\AnimationEditor.exe";
    static const char* UnixPath    = "tools/AnimationEditor/AnimationEditor";
    if (bl::file::Util::exists(UnixPath)) { return UnixPath; }
    return WindowsPath;
}

bool validFile(const std::string& f) { return !f.empty() && bl::file::Util::exists(f); }

} // namespace

AnimationWindow::AnimationWindow(bool cm, const ChooseCb& cb)
: characterMode(cm)
, chooseCb(cb) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Animation Picker");
    window->getSignal(Action::Closed).willAlwaysCall([this](const Action&, Element*) { hide(); });

    Box::Ptr row        = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickBut = Button::create("Choose Animation");
    pickBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        filePicker.emplace(path,
                           std::vector<std::string>{"anim"},
                           std::bind(&AnimationWindow::packAnim, this, std::placeholders::_1),
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
        std::system(editor);
    });
    window->pack(editBut, false, false);

    row      = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    auto src = bl::engine::Resources::animations()
                   .load(bl::file::Util::joinPath(path, "4/down.anim"))
                   .data;
    animation = Animation::create(src, true);
    animation->setRequisition({32, 45});
    row->pack(animation, true, true);
    window->pack(row, true, true);

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr chooseBut = Button::create("Use Animation");
    chooseBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        if (fileLabel->getText().empty() || !bl::file::Util::exists(fileLabel->getText())) {
            bl::dialog::tinyfd_messageBox(
                "No File", "Please select an animation", "ok", "error", 1);
            return;
        }

        chooseCb(fileLabel->getText());
        hide();
    });
    chooseBut->setColor(sf::Color::Green, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Action::LeftClicked).willAlwaysCall([this](const Action&, Element*) {
        hide();
    });
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(chooseBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row);
}

void AnimationWindow::open(const GUI::Ptr& p, const std::string& pt, const std::string& file) {
    packAnim(file);
    path = pt;
    p->pack(window);
}

void AnimationWindow::packAnim(const std::string& f) {
    const std::string af = characterMode ? bl::file::Util::getPath(f) : f;
    const std::string vf = characterMode ? bl::file::Util::joinPath(af, "down.anim") : af;
    fileLabel->setText(af);
    animSrc = bl::engine::Resources::animations().load(bl::file::Util::joinPath(path, vf)).data;
    if (animSrc) {
        sf::Vector2f size = animSrc->getMaxSize();
        if (size.x > 400.f) {
            size.y *= 400.f / size.x;
            size.x = 400.f;
            animation->scaleToSize(size);
        }
        if (size.y > 400.f) {
            size.x *= 400.f / size.y;
            size.y = 400.f;
            animation->scaleToSize(size);
        }
        animation->setAnimation(animSrc, true);
    }
}

void AnimationWindow::hide() {
    window->remove();
    if (filePicker.hasValue()) { filePicker.get().close(); }
}

} // namespace component
} // namespace editor
