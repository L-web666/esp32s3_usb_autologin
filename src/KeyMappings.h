#ifndef KEY_MAPPINGS_H
#define KEY_MAPPINGS_H

#include <map>
#include <string>

namespace KeyMappings {
// Map for friendly command names to HID key codes
extern const std::map<std::string, uint8_t> keyMap;

// Map for mouse buttons (optional)
extern const std::map<std::string, uint8_t> mouseButtonMap;

// Utility function to get HID key code by name
uint8_t getKeyCode(const std::string& keyName);

// Utility function to get mouse button code by name
uint8_t getMouseButtonCode(const std::string& buttonName);
} // namespace KeyMappings

#endif // KEY_MAPPINGS_H