#pragma once

#ifdef ENABLE_LED_HANDLER

#include <FastLED.h>

#define NUM_LEDS 1
#define LED_TYPE APA102
#define COLOR_ORDER BGR

class LedHandler
{
public:
    static void init();
    static void clear();
    static void setDefaultBrightness(uint8_t brightness);
    static void setColorByName(const String &colorName, uint8_t brightness = defaultBrightness);
private:
    static CRGB leds[NUM_LEDS];       // Static array for LED data
    static uint8_t defaultBrightness; // Default brightness level
    static void setColor(const CRGB &color, uint8_t brightness);
    static void registerCommands();
};

#else

#include <string>

// No-op implementation of LedHandler
class LedHandler
{
public:
    static void init() {}                                           // No-op
    static void clear() {}                                          // No-op
    static void setDefaultBrightness(uint8_t) {}                    // No-op
    static void setColorByName(const String &, uint8_t = 0) {} // No-op
};

#endif // ENABLE_LED_HANDLER