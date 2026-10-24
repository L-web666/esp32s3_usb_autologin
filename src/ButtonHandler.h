#pragma once

#include <ArduinoJson.h>

#ifdef ENABLE_BUTTON_HANDLER

// Full implementation of ButtonHandler
class ButtonHandler {
public:
    static void init();
    static void loop();
    static void runButton(int id);

private:
    static void handleSingleClick();
    static void handleDoubleClick();
    static void handleLongPress();
    static void handleDuringLongPress();
    static void handleLongPressStop();
    static unsigned long longPressStartTime; // Track the start time of the long press
    static bool rebootTriggered; // Flag to prevent multiple reboots
    static int lastCountdownValue; // Track the last displayed countdown value
    static void registerCommands();
    static void executeButtonAction(const JsonObject& button);
};

#else

// No-op implementation of ButtonHandler
class ButtonHandler {
public:
    static void init() {} // No-op
    static void loop() {} // No-op
    static void runButton(int id) {} // No-op
};

#endif // ENABLE_BUTTON_HANDLER
