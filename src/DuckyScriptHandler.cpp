#ifdef ENABLE_DUCKYSCRIPT_HANDLER
#include "DuckyScriptHandler.h"
#include "DeviceHandler.h"
#include "LittleFS.h"
#include "KeyMappings.h"
#include <vector>

void DuckyScriptHandler::init() {
    registerCommands();
    debugI("DuckyScriptHandler initialized");
}

void DuckyScriptHandler::executeScript(const String &filePath) {
    debugI("Executing DuckyScript from: %s", filePath.c_str());
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file: %s", filePath.c_str());
        return;
    }

    String lastLine;
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line.isEmpty() || line.startsWith("//") || line.startsWith("REM")) {
            debugI("Skipping: %s", line.isEmpty() ? "empty" : line.c_str());
            continue;
        }
        debugI("Line: %s", line.c_str());
        processLine(line);
        lastLine = line;
    }
    file.close();
    debugI("DuckyScript execution completed");
}

void DuckyScriptHandler::processLine(const String &line) {
    static int defaultDelay = 0;
    static String lastLine;

    std::vector<String> tokens;
    int start = 0, end = 0;
    while ((end = line.indexOf(' ', start)) != -1) {
        String token = line.substring(start, end);
        token.trim();
        if (!token.isEmpty()) tokens.push_back(token);
        start = end + 1;
    }
    if (start < line.length()) {
        String token = line.substring(start);
        token.trim();
        if (!token.isEmpty()) tokens.push_back(token);
    }

    if (tokens.empty()) return;

    String command = tokens[0];
    String args = tokens.size() > 1 ? line.substring(line.indexOf(' ') + 1) : "";

    if (CommandHandler::equalsIgnoreCase(command, "DELAY")) {
        int delayValue = args.toInt();
        if (delayValue > 0) {
            debugI("DELAY %d ms", delayValue);
            delay(delayValue);
        } else {
            debugE("Invalid DELAY: %s", args.c_str());
        }
    }
    else if (CommandHandler::equalsIgnoreCase(command, "DEFAULT_DELAY")) {
        defaultDelay = args.toInt();
        if (defaultDelay > 0) debugI("DEFAULT_DELAY %d ms", defaultDelay);
        else debugE("Invalid DEFAULT_DELAY: %s", args.c_str());
    }
    else if (CommandHandler::equalsIgnoreCase(command, "STRING")) {
        if (!args.isEmpty()) {
            debugI("STRING: %s", args.c_str());
            DeviceHandler::sendKeys(args);
        } else {
            debugE("STRING missing args");
        }
    }
    else if (CommandHandler::equalsIgnoreCase(command, "STRINGLN")) {
        if (!args.isEmpty()) {
            debugI("STRINGLN: %s", args.c_str());
            DeviceHandler::sendKeys(args);
            DeviceHandler::processKey("ENTER", true);
            delay(10);
            DeviceHandler::processKey("ENTER", false);
        } else {
            debugE("STRINGLN missing args");
        }
    }
    else if (CommandHandler::equalsIgnoreCase(command, "REPEAT")) {
        int count = args.toInt();
        if (count > 0 && !lastLine.isEmpty()) {
            debugI("REPEAT %d times", count);
            for (int i = 0; i < count; i++) {
                processLine(lastLine);
            }
        } else {
            debugE("Invalid REPEAT or no previous line");
        }
    }
    else if (CommandHandler::equalsIgnoreCase(command, "COMMAND")) {
        if (!args.isEmpty()) {
            debugI("COMMAND: %s", args.c_str());
            CommandHandler::handleCommand(args);
        } else {
            debugE("COMMAND missing args");
        }
    }
    else {
        // Handle modifiers or single keys
        debugI("Key combo or single key: %s", line.c_str());
        pressKeyCombo(line);
    }

    if (defaultDelay > 0) delay(defaultDelay);

    lastLine = line;
}

void DuckyScriptHandler::pressKeyCombo(const String &line) {
    std::vector<String> tokens;
    int start = 0, end = 0;
    while ((end = line.indexOf(' ', start)) != -1) {
        String token = line.substring(start, end);
        token.trim();
        if (!token.isEmpty()) tokens.push_back(token);
        start = end + 1;
    }
    if (start < line.length()) {
        String token = line.substring(start);
        token.trim();
        if (!token.isEmpty()) tokens.push_back(token);
    }

    std::vector<String> pressedKeys;
    for (const String &token : tokens) {
        std::string key = token.c_str();
        uint8_t keyCode = KeyMappings::getKeyCode(key);
        if (keyCode != 0) {
            debugI("Pressing: %s", key.c_str());
            DeviceHandler::processKey(token, true);
            pressedKeys.push_back(token);
        } else {
            debugW("Unknown key: %s", key.c_str());
        }
    }

    delay(10); // Debounce

    for (const String &key : pressedKeys) {
        debugI("Releasing: %s", key.c_str());
        DeviceHandler::processKey(key, false);
    }
}

void DuckyScriptHandler::registerCommands() {
    CommandHandler::registerCommand("DUCKY", [](const String &command) {
        debugI("DUCKY command: %s", command.c_str());
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);
        if (CommandHandler::equalsIgnoreCase(cmd, "FILE")) {
            if (args.isEmpty()) {
                debugE("Missing file path for DUCKY FILE");
                return;
            }
            executeScript(args);
        } else if (CommandHandler::equalsIgnoreCase(cmd, "LINE")) {
            processLine(args);
        } else {
            debugW("Unknown DUCKY subcommand: %s", cmd.c_str());
        }
    }, "Usage: DUCKY FILE <file_path>\n"
       "  FILE <file_path> - Executes DuckyScript from file\n"
       "  LINE <line> - Processes a single DuckyScript line");
}

#endif