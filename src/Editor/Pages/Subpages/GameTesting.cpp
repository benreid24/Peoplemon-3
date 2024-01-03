#include <Editor/Pages/Subpages/GameTesting.hpp>

namespace editor
{
namespace page
{
using namespace bl::gui;

GameTesting::GameTesting()
: window(std::bind(&GameTesting::sync, this)) {
    content = Box::create(LinePacker::create(LinePacker::Vertical, 4.f));

    Box::Ptr row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Save:"), false, true);
    saveSelector = ComboBox::create();
    saveSelector->setMaxHeight(350.f);
    saveSelector->getSignal(Event::ValueChanged).willAlwaysCall([this](const Event&, Element*) {
        if (saveSelector->getSelectedOption() >= 0) {
            auto& save = saves[saveSelector->getSelectedOption()];
            if (!save.load()) {
                bl::dialog::tinyfd_messageBox("Error", "Error loading save", "ok", "error", 0);
                return;
            }
            updatePosLabel(save.world.playerPos->level, save.world.playerPos->position);
        }
    });
    row->pack(saveSelector, true, true);
    Button::Ptr but = Button::create("Edit");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        if (saveSelector->getSelectedOption() >= 0) {
            auto& save = saves[saveSelector->getSelectedOption()];
            save.load();
            window.open(gui, save);
        }
    });
    row->pack(but, false, true);
    but = Button::create("Create");
    but->getSignal(Event::LeftClicked).willAlwaysCall([this](const Event&, Element*) {
        newSave.useLocalData();
        newSave.saveName             = "New Save";
        *newSave.world.prevMap       = "Worldmap";
        *newSave.world.prevPlayerPos = {0, glm::i32vec2(300, 300), bl::tmap::Direction::Down};
        newSave.player.inventory->clear();
        *newSave.player.monei = 1000000;
        newSave.player.peoplemon->clear();
        newSave.player.peoplemon->emplace_back(core::pplmn::Id::PiperB, 100);
        *newSave.player.playerName = "Assturd";
        *newSave.player.sex        = core::player::Gender::Boy;
        newSave.interaction.convFlags->clear();
        newSave.interaction.talkedto->clear();
        newSave.scripts.entries->clear();
        posLabel->setText("<unset>");
        window.open(gui, newSave);
    });
    row->pack(but, false, true);
    content->pack(row, true, false);

    content->pack(Label::create("Click map to set spawn position"));
    row = Box::create(LinePacker::create(LinePacker::Horizontal, 4.f));
    row->pack(Label::create("Spawn position:"), false, true);
    posLabel = Label::create("<unchanged>");
    posLabel->setColor(sf::Color(20, 230, 245), sf::Color::Transparent);
    row->pack(posLabel, false, true);
    content->pack(row, true, false);

    but = Button::create("Launch Game");
    but->getSignal(Event::LeftClicked).willAlwaysCall(std::bind(&GameTesting::launchGame, this));
    but->setColor(sf::Color(30, 230, 40), sf::Color::Black);
    content->pack(but);
    sync();
}

void GameTesting::registerGUI(GUI* g) { gui = g; }

const Box::Ptr& GameTesting::getContent() const { return content; }

void GameTesting::sync() {
    saves.clear();
    core::file::GameSave::listSaves(saves);
    for (auto& save : saves) { save.useLocalData(); }

    saveSelector->clearOptions();
    for (auto& save : saves) { saveSelector->addOption(save.saveName); }
}

void GameTesting::notifyClick(const std::string& mapName, unsigned int level,
                              const sf::Vector2i& pos) {
    if (saveSelector->getSelectedOption() >= 0) {
        auto& save = saves[saveSelector->getSelectedOption()];
        save.load();
        *save.world.currentMap = mapName;
        *save.world.playerPos =
            bl::tmap::Position(level, glm::i32vec2(pos.x, pos.y), bl::tmap::Direction::Down);
        save.editorSave();
        updatePosLabel(level, glm::i32vec2(pos.x, pos.y));
    }
}

void GameTesting::updatePosLabel(unsigned int level, const glm::i32vec2& pos) {
    std::stringstream ss;
    ss << "Lvl: " << level << ". Tile: (" << pos.x << ", " << pos.y << ")";
    posLabel->setText(ss.str());
}

void GameTesting::launchGame() {
    static const std::string Path = "PeoplemonDebug.exe";
    if (bl::util::FileUtil::exists(Path)) {
        if (saveSelector->getSelectedOption() >= 0) {
            auto& save = saves[saveSelector->getSelectedOption()];
            save.useLocalData();
            if (save.load()) {
                const std::string cmd =
                    Path + " \"" + core::file::GameSave::filename(*save.player.playerName) + "\"";
                BL_LOG_INFO << "Launching game: '" << cmd << "'";
                std::system(cmd.c_str());
            }
            else {
                bl::dialog::tinyfd_messageBox(
                    "Invalid Save", "Failed to load save", "ok", "error", 1);
            }
        }
        else {
            bl::dialog::tinyfd_messageBox("Invalid Save", "Please select a save", "ok", "error", 1);
        }
    }
    else {
        bl::dialog::tinyfd_messageBox(
            "Game not found", "PeoplemonDebug.exe not found", "ok", "error", 1);
    }
}

} // namespace page
} // namespace editor
