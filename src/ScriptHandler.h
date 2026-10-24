#pragma once

#ifdef ENABLE_SCRIPT_HANDLER

#include "Globals.h"
#include <deque>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

class ScriptHandler
{
private:
    // RTOS Task Handle for script execution
    static TaskHandle_t scriptTaskHandle;

    // Task function for processing scripts
    static void scriptTask(void *pvParameters);

    // Handles the SCRIPT FILE subcommand to execute script files
    static void handleScriptFile(const String &args);

    // Registers the SCRIPT command and its subcommands
    static void registerCommands();

    // Handles special script commands like DELAY, DEFAULTDELAY, and REPEAT
    static bool handleSpecialScriptCommand(const String &line);

    // Executes a single command, applying delays if needed
    static void executeCommand(const String &command);

    // State variables for script execution
    static unsigned long defaultDelay;           // Default delay between commands
    static std::deque<String> repeatBuffer;      // Buffer to store lines for REPEAT
    static unsigned int repeatCount;             // Number of times to repeat buffered commands

public:
    // Initializes the ScriptHandler
    static void init();

    // Main loop function (currently unused)
    static void loop();
};

#else

class ScriptHandler {
public: // No-op implementation of ScriptHandler
    static void loop() {}
    static void init() {}
};

#endif // ENABLE_SCRIPT_HANDLER
