#include <Editor/Components/AnimationWindow.hpp>

#include "Helpers/OpenAnimationEditor.hpp"
#include <Core/Resources.hpp>
#include <cstdlib>
#include <sstream>

namespace editor
{
namespace component
{
using namespace bl::gui;

namespace
{
bool validFile(const std::string& f) { return !f.empty() && bl::util::FileUtil::exists(f); }
} // namespace

AnimationWindow::AnimationWindow(bool cm, const ChooseCb& cb, const CloseCb& ccb)
: characterMode(cm)
, chooseCb(cb)
, closeCb(ccb)
, parent(nullptr) {
    window = Window::create(LinePacker::create(LinePacker::Vertical, 4), "Animation Picker");
    window->getSignal(Event::Closed).willAlwaysCall([this](const Event&, Element*) { hide(); });

    Box::Ptr row        = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr pickBut = Button::create("Choose Animation");
    pickBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        filePicker.emplace(path,
                           std::vector<std::string>{"anim"},
                           std::bind(&AnimationWindow::packAnim, this, std::placeholders::_1),
                           [this]() {
                               filePicker.value().close();
                               filePicker.reset();
                               window->setForceFocus(true);
                           });
        window->setForceFocus(false);
        filePicker.value().open(FilePicker::PickExisting, "Select Animation", parent);
    });
    fileLabel = Label::create("file here");
    fileLabel->setColor(sf::Color::Cyan, sf::Color::Transparent);
    row->pack(pickBut, false, true);
    row->pack(fileLabel, true, true);
    window->pack(row);

    Button::Ptr editBut = Button::create("Open Editor");
    editBut->getSignal(Event::LeftClicked).willAlwaysCall([](const Event&, Element*) {
        openAnimationEditor();
    });
    window->pack(editBut, false, false);

    row       = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    auto src  = AnimationManager::load(bl::util::FileUtil::joinPath(path, "4/down.anim"));
    animation = Animation::create(src);
    animation->setRequisition({32, 45});
    row->pack(animation, true, true);
    window->pack(row, true, true);

    row                   = Box::create(LinePacker::create(LinePacker::Horizontal, 4));
    Button::Ptr chooseBut = Button::create("Use Animation");
    chooseBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (validFile(fileLabel->getText())) {
            bl::dialog::tinyfd_messageBox(
                "No File", "Please select an animation", "ok", "error", 1);
            return;
        }

        chooseCb(fileLabel->getText());
        hide();
    });
    chooseBut->setColor(sf::Color::Green, sf::Color::Black);
    Button::Ptr cancelBut = Button::create("Cancel");
    cancelBut->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        hide();
    });
    cancelBut->setColor(sf::Color::Red, sf::Color::Black);
    row->pack(chooseBut, false, true);
    row->pack(cancelBut, false, true);
    window->pack(row);
}

void AnimationWindow::open(GUI* p, const std::string& pt, const std::string& file) {
    path   = pt;
    parent = p;
    p->pack(window);
    packAnim(file);
    window->setForceFocus(true);
}

void AnimationWindow::packAnim(const std::string& f) {
    const std::string af = characterMode ? bl::util::FileUtil::getPath(f) : f;
    const std::string vf = characterMode ? bl::util::FileUtil::joinPath(af, "down.anim") : af;
    fileLabel->setText(af);
    animSrc = AnimationManager::load(bl::util::FileUtil::joinPath(path, vf));
    BL_LOG_INFO << bl::util::FileUtil::joinPath(path, vf);
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
        animation->setAnimation(animSrc);
    }
    if (filePicker.has_value()) {
        filePicker.value().close();
        filePicker.reset();
        window->setForceFocus(true);
    }
}

void AnimationWindow::hide() {
    window->remove();
    window->setForceFocus(false);
    closeCb();
    if (filePicker.has_value()) {
        filePicker.value().close();
        filePicker.reset();
    }
}

} // namespace component
} // namespace editor
