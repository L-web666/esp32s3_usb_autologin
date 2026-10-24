#pragma once

#ifdef ENABLE_GFX_HANDLER

#include "Globals.h"
#include <LovyanGFX.hpp>

/*  Call this macro repeatedly.  After each use, the pixel data can be extracted  */
#define HEADER_PIXEL(data,pixel) {\
    pixel[0] = (((data[0] - 33) << 2) | ((data[1] - 33) >> 4)); \
    pixel[1] = ((((data[1] - 33) & 0xF) << 4) | ((data[2] - 33) >> 2)); \
    pixel[2] = ((((data[2] - 33) & 0x3) << 6) | ((data[3] - 33))); \
    data += 4; \
}

// Define the display configuration for LILYGO T-Dongle-S3
class LGFX_LiLyGo_TDongleS3 : public lgfx::LGFX_Device {
    lgfx::Panel_ST7735S _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

public:
    LGFX_LiLyGo_TDongleS3();
};

// Define the handler for LovyanGFX
class GfxHandler {
private:
    static NonBlockingTimer clockTimer;
    static bool showClock;
    static void toggleClock(bool state);
    static LGFX_LiLyGo_TDongleS3 tft;
    static void registerCommands();
    
public:
    static void init();
    static void loop();
    static void printMessage(const String &message);
    static void drawImage(int x, int y, int width, int height, const char *data);
};

#else

#include <Arduino.h> // For String class

class GfxHandler {
public:
    static void init() {}
    static void loop() {}
    static void printMessage(const String &message) {}
    static void drawImage(int x, int y, int width, int height, const char *data) {}
};

#endif // ENABLE_GFX_HANDLER