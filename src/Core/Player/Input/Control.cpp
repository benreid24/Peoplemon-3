#include <Core/Player/Input/Control.hpp>

#include <unordered_map>

namespace core
{
namespace player
{
namespace input
{
namespace
{
std::unordered_map<sf::Keyboard::Key, std::string> keymap;
std::unordered_map<std::string, sf::Keyboard::Key> strKeymap;
std::unordered_map<sf::Mouse::Button, std::string> buttonmap;
std::unordered_map<std::string, sf::Mouse::Button> strButtonmap;
bool inited = false;

void init() {
    // Letters
    for (int i = 0; i < 26; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::A + i);
        keymap[k]                 = 'A' + i;
    }

    // Numbers
    for (int i = 0; i < 10; ++i) {
        const sf::Keyboard::Key row = static_cast<sf::Keyboard::Key>(sf::Keyboard::Num0 + i);
        const sf::Keyboard::Key pad = static_cast<sf::Keyboard::Key>(sf::Keyboard::Numpad0 + i);
        keymap[row]                 = std::to_string(i);
        keymap[pad]                 = std::string("Numpad ") + std::to_string(i);
    }

    // Function keys
    for (int i = 0; i < 15; ++i) {
        const sf::Keyboard::Key k = static_cast<sf::Keyboard::Key>(sf::Keyboard::F1 + i);
        keymap[k]                 = "F" + std::to_string(i + 1);
    }

    // Misc keys
    keymap[sf::Keyboard::Space]     = "Space";
    keymap[sf::Keyboard::Tab]       = "Tab";
    keymap[sf::Keyboard::LControl]  = "Left Ctrl";
    keymap[sf::Keyboard::LShift]    = "Left Shift";
    keymap[sf::Keyboard::LAlt]      = "Left Alt";
    keymap[sf::Keyboard::Tilde]     = "~";
    keymap[sf::Keyboard::Enter]     = "Enter";
    keymap[sf::Keyboard::RAlt]      = "Right Alt";
    keymap[sf::Keyboard::RShift]    = "Right Shift";
    keymap[sf::Keyboard::RControl]  = "Right Ctrl";
    keymap[sf::Keyboard::Semicolon] = ";";
    keymap[sf::Keyboard::Subtract]  = "Numpad -";
    keymap[sf::Keyboard::Add]       = "Numpad +";
    keymap[sf::Keyboard::Backspace] = "Backspace";
    keymap[sf::Keyboard::Backslash] = "\\";
    keymap[sf::Keyboard::Comma]     = ",";
    keymap[sf::Keyboard::Quote]     = "'";
    keymap[sf::Keyboard::Escape]    = "Esc";
    keymap[sf::Keyboard::Equal]     = "=";
    keymap[sf::Keyboard::End]       = "End";
    keymap[sf::Keyboard::Home]      = "Home";
    keymap[sf::Keyboard::Period]    = ".";
    keymap[sf::Keyboard::PageDown]  = "Page Down";
    keymap[sf::Keyboard::PageUp]    = "Page Up";
    keymap[sf::Keyboard::Pause]     = "Pause";
    keymap[sf::Keyboard::Menu]      = "Menu";
    keymap[sf::Keyboard::Slash]     = "/";
    keymap[sf::Keyboard::Hyphen]    = "-";
    keymap[sf::Keyboard::Delete]    = "Delete";
    keymap[sf::Keyboard::Insert]    = "Insert";
    keymap[sf::Keyboard::Multiply]  = "*";
    keymap[sf::Keyboard::Divide]    = "Numpad /";
    keymap[sf::Keyboard::Up]        = "Up";
    keymap[sf::Keyboard::Right]     = "Right";
    keymap[sf::Keyboard::Down]      = "Down";
    keymap[sf::Keyboard::Left]      = "Left";

    // Reverse key lookup
    for (const auto& pair : keymap) { strKeymap[pair.second] = pair.first; }

    // Mouse buttons
    buttonmap[sf::Mouse::Left]     = "Left Mouse";
    buttonmap[sf::Mouse::Right]    = "Right Mouse";
    buttonmap[sf::Mouse::Middle]   = "Middle Mouse";
    buttonmap[sf::Mouse::XButton1] = "Mouse 4";
    buttonmap[sf::Mouse::XButton2] = "Mouse 5";

    // Reverse button lookup
    for (const auto& pair : buttonmap) { strButtonmap[pair.second] = pair.first; }

    inited = true;
}
} // namespace

Control::Control()
: type(Invalid)
, key(sf::Keyboard::Unknown) {}

Control::Control(sf::Keyboard::Key key)
: type(Key)
, key(key) {}

Control::Control(sf::Mouse::Button button)
: type(Mouse)
, button(button) {}

const std::string& Control::toString() const {
    static const std::string unknown = "Unknown";
    if (!inited) init();

    switch (type) {
    case Key: {
        const auto it = keymap.find(key);
        return it == keymap.end() ? unknown : it->second;
    }

    case Mouse: {
        const auto it = buttonmap.find(button);
        return it == buttonmap.end() ? unknown : it->second;
    }

    default:
        return unknown;
    }
}

Control Control::fromString(const std::string& s) {
    if (!inited) init();

    const auto kit = strKeymap.find(s);
    if (kit != strKeymap.end()) return {kit->second};

    const auto mit = strButtonmap.find(s);
    if (mit != strButtonmap.end()) return {mit->second};

    return {};
}

Control Control::fromEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) { return {event.key.code}; }
    if (event.type == sf::Event::MouseButtonPressed) { return {event.mouseButton.button}; }
    return {};
}

bool Control::matches(const sf::Event& event) const {
    switch (type) {
    case Key:
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
            return event.key.code == key;
        }
        return false;

    case Mouse:
        if (event.type == sf::Event::MouseButtonPressed ||
            event.type == sf::Event::MouseButtonReleased) {
            return event.mouseButton.button == button;
        }
        return false;

    default:
        return false;
    }
}

bool Control::activated(const sf::Event& event) const {
    switch (type) {
    case Key:
        if (event.type == sf::Event::KeyPressed) { return event.key.code == key; }
        return false;

    case Mouse:
        if (event.type == sf::Event::MouseButtonPressed) {
            return event.mouseButton.button == button;
        }
        return false;

    default:
        return false;
    }
}

bool Control::deactivated(const sf::Event& event) const {
    switch (type) {
    case Key:
        if (event.type == sf::Event::KeyReleased) { return event.key.code == key; }
        return false;

    case Mouse:
        if (event.type == sf::Event::MouseButtonReleased) {
            return event.mouseButton.button == button;
        }
        return false;

    default:
        return false;
    }
}

} // namespace input
} // namespace player
} // namespace core