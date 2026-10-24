#ifdef ENABLE_SCRIPT_HANDLER

#include "ScriptHandler.h"
#include <LittleFS.h>

// Static member definitions
unsigned long ScriptHandler::defaultDelay = 0;           // Default delay
std::deque<String> ScriptHandler::repeatBuffer = {};     // Command buffer
unsigned int ScriptHandler::repeatCount = 0;             // Repeat count
TaskHandle_t ScriptHandler::scriptTaskHandle = nullptr;  // RTOS Task Handle

void ScriptHandler::loop() {}

void ScriptHandler::init()
{
    if (!LittleFS.begin()) {
        debugE("* LittleFS initialization failed!");
        return;
    }

    registerCommands();

    // Create the RTOS task for script processing
    xTaskCreatePinnedToCore(
        scriptTask,         // Task function
        "ScriptTask",       // Task name
        8192,               // Stack size
        nullptr,            // Parameters (none in this case)
        1,                  // Priority (1 = low)
        &scriptTaskHandle,  // Task handle
        tskNO_AFFINITY      // Run on any core
    );

    debugI("ScriptHandler initialized and RTOS task created.");
}

void ScriptHandler::scriptTask(void *pvParameters)
{
    // Infinite loop to handle scripts
    while (true) {
        // If there are commands in the repeat buffer, execute them
        if (!repeatBuffer.empty() && repeatCount > 0) {
            debugI("Executing repeated commands %u times", repeatCount);

            for (unsigned int i = 0; i < repeatCount; i++) {
                for (const String &repeatLine : repeatBuffer) {
                    executeCommand(repeatLine);
                }
                vTaskDelay(1); // Yield after each repetition cycle
            }

            repeatBuffer.clear();  // Clear buffer after execution
            repeatCount = 0;       // Reset repeat count
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // Prevent busy looping (adjust as needed)
    }
}

void ScriptHandler::handleScriptFile(const String &args)
{
    if (args.isEmpty()) {
        debugW("SCRIPT FILE requires a file path. Usage: SCRIPT FILE <path>");
        return;
    }

    if (!LittleFS.exists(args)) {
        debugW("Script file not found: %s", args.c_str());
        return;
    }

    File scriptFile = LittleFS.open(args, "r");
    if (!scriptFile) {
        debugE("Failed to open script file: %s", args.c_str());
        return;
    }

    debugI("Executing script file: %s", args.c_str());

    while (scriptFile.available()) {
        String line = scriptFile.readStringUntil('\n');
        line.trim(); // Remove any leading/trailing whitespace or newlines

        if (line.isEmpty() || line.startsWith("REM")) { // Ignore empty lines and comments
            continue;
        }

        // Handle special script commands
        if (!handleSpecialScriptCommand(line)) {
            debugD("Buffering and executing line: %s", line.c_str());
            repeatBuffer.push_back(line); // Add the command to the buffer
            executeCommand(line);
        }

        vTaskDelay(1); // Yield after each line to prevent WDT resets
    }

    scriptFile.close();
    debugI("Finished executing script file: %s", args.c_str());
}

bool ScriptHandler::handleSpecialScriptCommand(const String &line)
{
    String command, args;
    CommandHandler::parseCommand(line, command, args);

    if (CommandHandler::equalsIgnoreCase(command, "DEFAULTDELAY")) {
        defaultDelay = args.toInt();
        debugI("Set default delay to %lu ms", defaultDelay);
        return true; // Handled
    }

    if (CommandHandler::equalsIgnoreCase(command, "DELAY")) {
        unsigned long delayTime = args.toInt();
        debugI("Applying delay of %lu ms", delayTime);
        vTaskDelay(pdMS_TO_TICKS(delayTime));
        return true; // Handled
    }

    if (CommandHandler::equalsIgnoreCase(command, "REPEAT")) {
        repeatCount = args.toInt();
        debugI("Buffering repeated commands %u times", repeatCount);
        return true; // Handled
    }

    return false; // Not a special command
}

void ScriptHandler::executeCommand(const String &command)
{
    CommandHandler::handleCommand(command);

    // Apply the default delay after executing the command
    if (defaultDelay > 0) {
        vTaskDelay(pdMS_TO_TICKS(defaultDelay));
    }
}

void ScriptHandler::registerCommands()
{
    CommandHandler::registerCommand("SCRIPT", [](const String &command) {
        String subCommand, args;
        CommandHandler::parseCommand(command, subCommand, args);

        if (CommandHandler::equalsIgnoreCase(subCommand, "FILE")) {
            handleScriptFile(args);
        } else {
            debugW("Unknown SCRIPT subcommand: %s", subCommand.c_str());
        }
    },
    "Handles SCRIPT commands. Usage: SCRIPT <subcommand> [args]\n"
    "  Subcommands:\n"
    "  file <path> - Executes commands from the specified script file.");
}

#endif // ENABLE_SCRIPT_HANDLER
