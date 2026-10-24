#ifdef ENABLE_LED_HANDLER

#include "LedHandler.h"
#include "LedColorMap.h"
#include "Globals.h"

// Define static members
CRGB LedHandler::leds[NUM_LEDS];
uint8_t LedHandler::defaultBrightness = 100; // Default brightness

// Initialize the LED handler
void LedHandler::init() {
  FastLED.addLeds<LED_TYPE, LED_DI_PIN, LED_CI_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  clear(); // Turn off all LEDs initially
  registerCommands();
  debugI("LEDHandler initialized with %d LED(s)", NUM_LEDS);
}

// Set the color and brightness of the LEDs
void LedHandler::setColor(const CRGB &color, uint8_t brightness) {
  FastLED.setBrightness(brightness);
  leds[0] = color;
  FastLED.show();
  debugD("LED color set to R:%d G:%d B:%d with brightness %d", color.r, color.g, color.b, brightness);
}

// Clear all LEDs
void LedHandler::clear() {
  FastLED.clear();
  FastLED.show();
  debugI("LEDs cleared");
}

// Set the default brightness level
void LedHandler::setDefaultBrightness(uint8_t brightness) {
  defaultBrightness = brightness;
  FastLED.setBrightness(defaultBrightness);
  FastLED.show();
  debugI("Default brightness set to %d", brightness);
}

// Set LED color by name
void LedHandler::setColorByName(const String &colorName, uint8_t brightness) {
  String colorNameLower = colorName;
  colorNameLower.toLowerCase();

  auto it = colorMap.find(colorNameLower.c_str()); // Use c_str() for std::string lookup
  if (it != colorMap.end()) {
    setColor(it->second, brightness);
  } else {
    debugW("Unknown color: %s", colorName.c_str());
  }
}

// Register LED-related commands
void LedHandler::registerCommands() {
  CommandHandler::registerCommand("led",[](const String &command) {String cmd, args;
      CommandHandler::parseCommand(command, cmd, args);

      if (cmd == "color") {
        setColorByName(args.c_str(), defaultBrightness);
      } else if (cmd == "clear") {
        clear();
      } else if (cmd == "brightness") {
        uint8_t brightness = static_cast<uint8_t>(args.toInt());
        setDefaultBrightness(brightness);
      } else {
        debugW("Unknown led subcommand: %s", cmd.c_str());
      }
    },
    "Handles LED commands. Usage: led <subcommand> [args]\n"
    "  Subcommands:\n"
    "  color <color> - Set LED color by name (e.g., Red, Green, Blue)\n"
    "  clear - Clear all LEDs\n"
    "  brightness <level> - Set default brightness level (0-255)"
  );
}

#endif // ENABLE_LED_HANDLER