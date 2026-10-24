#pragma once

#ifdef ENABLE_DEVICE_HANDLER

#include <USBHIDMouse.h>
#include <USBHIDKeyboard.h>

class DeviceHandler
{
private:
    static USBHIDMouse mouse;
    static USBHIDKeyboard keyboard;
    static void registerCommands();
    static void printFile(const char *filePath);
    
public:
    static void sendMouseMovement(int x, int y);
    static USBHIDKeyboard& getKeyboard();
    static int keyPressDelay; // Configurable delay for key presses
    static void setKeyPressDelay(int delay);
    static void loop();
    static void init();
    static void sendKeys(const String& text);
    static void tapKey(const String& key);
    static void processKey(const String& keyName, bool press);
};

#else
class USBHIDKeyboard;
class DeviceHandler
{
public: // No-op implementation of DeviceHandler
    static void sendMouseMovement(int x, int y) {} // No-op
    static USBHIDKeyboard fakeKeyboard;
    static int keyPressDelay;
    static void loop() {} // No-op
    static void init() {} // No-op
    static void sendKeys(const String& text) {} // No-op
    static void tapKey(const String& key) {} // No-op
    static void processKey(const String& keyName, bool press) {} // No-op
};

#endif // ENABLE_DEVICE_HANDLER