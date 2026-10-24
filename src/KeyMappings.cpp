#include "KeyMappings.h"
#include <USBHIDKeyboard.h>

namespace KeyMappings {
// Key mappings for DuckyScript commands
const std::map<std::string, uint8_t> keyMap = {
    // Letters (case handled by DuckyScriptHandler)
    {"A", HID_KEY_A}, {"B", HID_KEY_B}, {"C", HID_KEY_C}, {"D", HID_KEY_D},
    {"E", HID_KEY_E}, {"F", HID_KEY_F}, {"G", HID_KEY_G}, {"H", HID_KEY_H},
    {"I", HID_KEY_I}, {"J", HID_KEY_J}, {"K", HID_KEY_K}, {"L", HID_KEY_L},
    {"M", HID_KEY_M}, {"N", HID_KEY_N}, {"O", HID_KEY_O}, {"P", HID_KEY_P},
    {"Q", HID_KEY_Q}, {"R", HID_KEY_R}, {"S", HID_KEY_S}, {"T", HID_KEY_T},
    {"U", HID_KEY_U}, {"V", HID_KEY_V}, {"W", HID_KEY_W}, {"X", HID_KEY_X},
    {"Y", HID_KEY_Y}, {"Z", HID_KEY_Z},
    {"a", HID_KEY_A}, {"b", HID_KEY_B}, {"c", HID_KEY_C}, {"d", HID_KEY_D},
    {"e", HID_KEY_E}, {"f", HID_KEY_F}, {"g", HID_KEY_G}, {"h", HID_KEY_H},
    {"i", HID_KEY_I}, {"j", HID_KEY_J}, {"k", HID_KEY_K}, {"l", HID_KEY_L},
    {"m", HID_KEY_M}, {"n", HID_KEY_N}, {"o", HID_KEY_O}, {"p", HID_KEY_P},
    {"q", HID_KEY_Q}, {"r", HID_KEY_R}, {"s", HID_KEY_S}, {"t", HID_KEY_T},
    {"u", HID_KEY_U}, {"v", HID_KEY_V}, {"w", HID_KEY_W}, {"x", HID_KEY_X},
    {"y", HID_KEY_Y}, {"z", HID_KEY_Z},

    // Numbers
    {"0", HID_KEY_0}, {"1", HID_KEY_1}, {"2", HID_KEY_2}, {"3", HID_KEY_3},
    {"4", HID_KEY_4}, {"5", HID_KEY_5}, {"6", HID_KEY_6}, {"7", HID_KEY_7},
    {"8", HID_KEY_8}, {"9", HID_KEY_9},

    // Punctuation
    {"MINUS", HID_KEY_MINUS}, {"-", HID_KEY_MINUS},
    {"EQUAL", HID_KEY_EQUAL}, {"=", HID_KEY_EQUAL},
    {"BRACKET_LEFT", HID_KEY_BRACKET_LEFT}, {"[", HID_KEY_BRACKET_LEFT},
    {"BRACKET_RIGHT", HID_KEY_BRACKET_RIGHT}, {"]", HID_KEY_BRACKET_RIGHT},
    {"BACKSLASH", HID_KEY_BACKSLASH}, {"\\", HID_KEY_BACKSLASH},
    {"SEMICOLON", HID_KEY_SEMICOLON}, {";", HID_KEY_SEMICOLON},
    {"APOSTROPHE", HID_KEY_APOSTROPHE}, {"'", HID_KEY_APOSTROPHE},
    {"GRAVE", HID_KEY_GRAVE}, {"`", HID_KEY_GRAVE},
    {"COMMA", HID_KEY_COMMA}, {",", HID_KEY_COMMA},
    {"PERIOD", HID_KEY_PERIOD}, {".", HID_KEY_PERIOD},
    {"SLASH", HID_KEY_SLASH}, {"/", HID_KEY_SLASH},

    // Special Keys
    {"ENTER", HID_KEY_ENTER},
    {"ESC", HID_KEY_ESCAPE}, {"ESCAPE", HID_KEY_ESCAPE},
    {"BACKSPACE", HID_KEY_BACKSPACE},
    {"TAB", HID_KEY_TAB},
    {"SPACE", HID_KEY_SPACE},
    {"CAPSLOCK", HID_KEY_CAPS_LOCK},
    {"PRINTSCREEN", HID_KEY_PRINT_SCREEN},
    {"SCROLLLOCK", HID_KEY_SCROLL_LOCK},
    {"PAUSE", HID_KEY_PAUSE},
    {"INSERT", HID_KEY_INSERT},
    {"HOME", HID_KEY_HOME},
    {"PAGEUP", HID_KEY_PAGE_UP}, {"PAGE_UP", HID_KEY_PAGE_UP},
    {"DELETE", HID_KEY_DELETE},
    {"END", HID_KEY_END},
    {"PAGEDOWN", HID_KEY_PAGE_DOWN}, {"PAGE_DOWN", HID_KEY_PAGE_DOWN},
    {"MENU", HID_USAGE_DESKTOP_SYSTEM_CONTEXT_MENU}, // Corrected per your note
    {"NUMLOCK", HID_KEY_NUM_LOCK},

    // Arrow Keys (DuckyScript aliases)
    {"UP", HID_KEY_ARROW_UP}, {"UPARROW", HID_KEY_ARROW_UP},
    {"DOWN", HID_KEY_ARROW_DOWN}, {"DOWNARROW", HID_KEY_ARROW_DOWN},
    {"LEFT", HID_KEY_ARROW_LEFT}, {"LEFTARROW", HID_KEY_ARROW_LEFT},
    {"RIGHT", HID_KEY_ARROW_RIGHT}, {"RIGHTARROW", HID_KEY_ARROW_RIGHT},

    // Function Keys
    {"F1", HID_KEY_F1}, {"F2", HID_KEY_F2}, {"F3", HID_KEY_F3}, {"F4", HID_KEY_F4},
    {"F5", HID_KEY_F5}, {"F6", HID_KEY_F6}, {"F7", HID_KEY_F7}, {"F8", HID_KEY_F8},
    {"F9", HID_KEY_F9}, {"F10", HID_KEY_F10}, {"F11", HID_KEY_F11}, {"F12", HID_KEY_F12},

    // Modifiers
    {"CTRL", HID_KEY_CONTROL_LEFT}, {"CONTROL", HID_KEY_CONTROL_LEFT},
    {"SHIFT", HID_KEY_SHIFT_LEFT},
    {"ALT", HID_KEY_ALT_LEFT},
    {"GUI", HID_KEY_GUI_LEFT}, {"WINDOWS", HID_KEY_GUI_LEFT},
    {"CTRL_RIGHT", HID_KEY_CONTROL_RIGHT},
    {"SHIFT_RIGHT", HID_KEY_SHIFT_RIGHT},
    {"ALT_RIGHT", HID_KEY_ALT_RIGHT},
    {"GUI_RIGHT", HID_KEY_GUI_RIGHT},

    // Keypad Keys
    {"KP_0", HID_KEY_KEYPAD_0}, {"KP_1", HID_KEY_KEYPAD_1},
    {"KP_2", HID_KEY_KEYPAD_2}, {"KP_3", HID_KEY_KEYPAD_3},
    {"KP_4", HID_KEY_KEYPAD_4}, {"KP_5", HID_KEY_KEYPAD_5},
    {"KP_6", HID_KEY_KEYPAD_6}, {"KP_7", HID_KEY_KEYPAD_7},
    {"KP_8", HID_KEY_KEYPAD_8}, {"KP_9", HID_KEY_KEYPAD_9},
    {"KP_PLUS", HID_KEY_KEYPAD_ADD}, {"KP_MINUS", HID_KEY_KEYPAD_SUBTRACT},
    {"KP_MULTIPLY", HID_KEY_KEYPAD_MULTIPLY}, {"KP_DIVIDE", HID_KEY_KEYPAD_DIVIDE},
    {"KP_ENTER", HID_KEY_KEYPAD_ENTER}
};

// Optional mouse button mappings
const std::map<std::string, uint8_t> mouseButtonMap = {
    {"LEFT", 1}, {"RIGHT", 2}, {"MIDDLE", 4} // HID mouse button bits
};

// Get HID key code by name
uint8_t getKeyCode(const std::string& keyName) {
    auto it = keyMap.find(keyName);
    return (it != keyMap.end()) ? it->second : 0;
}

// Get mouse button code by name
uint8_t getMouseButtonCode(const std::string& buttonName) {
    auto it = mouseButtonMap.find(buttonName);
    return (it != mouseButtonMap.end()) ? it->second : 0;
}
} // namespace KeyMappings