#ifdef ENABLE_DEVICE_HANDLER

#include "DeviceHandler.h"
#include "Globals.h"
#include "DeviceDescriptors.h"
#include "KeyMappings.h"
#include <USB.h>
#include <LittleFS.h>

int DeviceHandler::keyPressDelay = 50;
//int DeviceHandler::keyPressDelay = settings.device.keyPressDelay;
USBHIDMouse DeviceHandler::mouse;
USBHIDKeyboard DeviceHandler::keyboard;

USBHIDKeyboard &DeviceHandler::getKeyboard() {
    return keyboard;
}

void DeviceHandler::setKeyPressDelay(int delay) {
    keyPressDelay = delay > 0 ? delay : 20;
    //keyPressDelay = delay > 0 ? delay : settings.device.keyPressDelay; // Fallback to default from settings
    debugI("Key press delay set to %d ms", keyPressDelay);
}

void DeviceHandler::loop() {}

void DeviceHandler::init() {
    // 顺序很重要：各接口的 begin() 要在 USB.begin() 之前，
    // 这样它们的接口才会被写进 USB 配置描述符。
    // 原来先 USB.begin() 再 keyboard.begin()，导致只有键盘一个接口，
    // 后加的 CDC 串口（SerialConfigHandler）根本不会出现在设备上。
    Serial.begin(115200);
    mouse.begin();
    keyboard.begin();
    USB.begin();
    registerCommands();
    debugI("DeviceHandler initialized");
    // Note: Ensure LittleFS.begin() is called somewhere in the setup process if not already done
}

void DeviceHandler::sendMouseMovement(int x, int y) {
    mouse.move(x, y);
    debugI("Mouse moved: x=%d, y=%d", x, y);
}

void DeviceHandler::sendKeys(const String &text) {
    for (size_t i = 0; i < text.length(); i++) {
        keyboard.write(text[i]);
        delay(keyPressDelay); // Configurable delay between key presses
    }
    debugI("Keys sent: %s", text.c_str());
}

void DeviceHandler::processKey(const String &keyName, bool press) {
    std::string key = keyName.c_str();
    auto it = KeyMappings::keyMap.find(key);
    if (it != KeyMappings::keyMap.end()) {
        uint8_t keyCode = it->second;
        if (press) {
            keyboard.pressRaw(keyCode); // Press the raw HID key code
            debugI("Pressed key: %s (code: %d)", keyName.c_str(), keyCode);
        } else {
            keyboard.releaseAll(); // Release all keys
            debugI("Released all keys");
        }
    } else {
        debugW("Invalid key: %s", keyName.c_str());
    }
}

void DeviceHandler::tapKey(const String &keyName) {
    processKey(keyName, true);  // Press
    delay(50);                  // Hold duration
    //delay(settings.device.keyPressDelay); // Use the configured delay for consistency
    processKey(keyName, false); // Release
}

void DeviceHandler::printFile(const char *filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    // Reset file pointer to start
    file.seek(0);

    while (file.available()) {
        char c = file.read();
        if (c != '\r') {
            keyboard.print(c);   
        }
    }

    file.close();
    keyboard.println();
}

void DeviceHandler::registerCommands() {
    CommandHandler::registerCommand("hid", [](const String &command) {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (cmd == "mouse") {
            int x = 0, y = 0;
            int commaIndex = args.indexOf(',');
            if (commaIndex > 0) {
                x = args.substring(0, commaIndex).toInt();
                y = args.substring(commaIndex + 1).toInt();
                DeviceHandler::sendMouseMovement(x, y);
            } else {
                debugW("Invalid args for HID mouse. Expected: x,y");
            }
        }
        else if (cmd == "keys") {
            sendKeys(args);
        }
        else if (cmd == "winlock") {
            keyboard.pressRaw(HID_KEY_GUI_LEFT);
            keyboard.pressRaw(HID_KEY_L);
            delay(500);
            keyboard.releaseAll();
            debugI("Windows locked");
        }
        else if (cmd == "tapkey") {
            if (!args.isEmpty()) {
                DeviceHandler::tapKey(args);
            } else {
                debugW("No key provided for tapKey");
            }
        }
        else if (cmd == "processkey") {
            int spaceIndex = args.indexOf(' ');
            if (spaceIndex > 0) {
                String key = args.substring(0, spaceIndex);
                String action = args.substring(spaceIndex + 1);
                if (action.equalsIgnoreCase("press")) {
                    DeviceHandler::processKey(key, true);
                } else if (action.equalsIgnoreCase("release")) {
                    DeviceHandler::processKey(key, false);
                } else {
                    debugW("Invalid action for processKey. Use 'press' or 'release'");
                }
            } else {
                debugW("Invalid args for processKey. Expected: <key> <press/release>");
            }
        }
        else if (cmd == "delay") {
            int delay = args.toInt();
            if (delay > 0) {
                DeviceHandler::setKeyPressDelay(delay);
            } else {
                debugW("Invalid delay value. Expected positive integer");
            }
        }
        else if (cmd == "file") { // New subcommand to handle file typing
            if (args.isEmpty()) {
                debugW("No file path provided for HID file command");
                return;
            }
            
            printFile(args.c_str());
            debugI("File %s typed out successfully", args.c_str());
        }
        else {
            debugW("Unknown HID subcommand: %s", cmd.c_str());
        }
    },
    "Handles HID commands. Usage: hid <subcommand> [args]\n"
    "  Subcommands:\n"
    "  mouse x,y - Move mouse by x and y\n"
    "  keys <keys> - Send keys\n"
    "  winlock - Locks Windows\n"
    "  tapKey <key> - Tap a single key\n"
    "  processKey <key> <press/release> - Press or release a key\n"
    "  delay <ms> - Set key press delay (default 20 ms)\n"
    "  file <path> - Type out the contents of the file line by line"); // Updated help string
}

#endif // ENABLE_DEVICE_HANDLER

/*
void DeviceHandler::printFile1(const char *filePath) {

    // Note if windows notepad spell check is enabled, it will not work
    // Tested with notepad++ and it works
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    // Tested works without this day
    delay(1000);
    keyboard.begin();
    file.seek(0);

    // Buffer small chunks to avoid timing issues
    const size_t BUFFER_SIZE = 32; // Small to stay memory-safe
    char buffer[BUFFER_SIZE + 1];  // +1 for null terminator
    size_t bytesRead;

    while (file.available()) {
        bytesRead = file.readBytes(buffer, BUFFER_SIZE);
        buffer[bytesRead] = '\0'; // Null-terminate for safety

        for (size_t i = 0; i < bytesRead; i++) {
            if (buffer[i] != '\r') {
                //keyboard.print(buffer[i]);
                keyboard.write(buffer[i]);
                //yield(); // Works without this delay
                //delay(50); // Keep max delay for now
            }
        }
        //delay(100); // Extra delay between chunks
    }

    file.close();
    //keyboard.println();
    keyboard.write('\n');
}

void DeviceHandler::printFile2(const char *filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    // Reset file pointer to start
    file.seek(0);

    while (file.available()) {
        char c = file.read();
        if (c != '\r') { // Skip carriage returns, if any
            //keyboard.write(c);
            keyboard.print(c);
            //debugI(c);
            //delay(50); // It's working without this delay
            //yield(); // It's working without this delay     
        }
    }

    file.close();
    //keyboard.write('\n'); // Ensure final newline
    keyboard.println(); // Move to the next line after sending the file content
}

void DeviceHandler::printFile3(const char *filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    // Initial delay for USB HID stability
    //delay(500);
    keyboard.begin();
    file.seek(0);

    // Buffer setup
    const size_t BUFFER_SIZE = 32; // Matches USB HID buffer size nicely
    char buffer[BUFFER_SIZE + 1];  // +1 for null terminator
    size_t bytesRead;

    while (file.available()) {
        bytesRead = file.readBytes(buffer, BUFFER_SIZE);
        buffer[bytesRead] = '\0'; // Ensure null termination

        for (size_t i = 0; i < bytesRead; i++) {
            if (buffer[i] != '\r') { // Skip carriage returns
                keyboard.print(buffer[i]); // Reliable for ASCII
                //delay(20); // ~50 chars/sec, safe for Notepad spell check
            }
        }
        //delay(50); // Pause between chunks
    }

    file.close();
    keyboard.println(); // Consistent newline
}

void DeviceHandler::printFile4(const char *filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    // Reset file pointer to start
    file.seek(0);

    while (file.available()) {
        char c = file.read();
        if (c != '\r') {
            keyboard.print(c);   
        }
    }

    file.close();
    keyboard.println();
}

void DeviceHandler::printFile5(const char *filePath) {
    // First pass: Count total lines
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        debugE("Failed to open file %s for reading", filePath);
        return;
    }

    size_t totalLines = 0;
    while (file.available()) {
        file.readStringUntil('\n'); // Skip to next line
        totalLines++;
    }
    file.close();

    // Second pass: Print in batches of 10 lines
    const size_t BATCH_SIZE = 10;
    size_t linesPrinted = 0;

    while (linesPrinted < totalLines) {
        file = LittleFS.open(filePath, "r");
        if (!file) {
            debugE("Failed to reopen file %s", filePath);
            return;
        }

        // Skip to the current line offset
        for (size_t i = 0; i < linesPrinted && file.available(); i++) {
            file.readStringUntil('\n');
        }

        // Print up to BATCH_SIZE lines
        size_t linesInBatch = 0;
        while (file.available() && linesInBatch < BATCH_SIZE) {
            String line = file.readStringUntil('\n');
            line.trim(); // Remove \r or trailing spaces
            for (size_t i = 0; i < line.length(); i++) {
                keyboard.print(line[i]);
                delay(20); // Pace for Notepad spell check
            }
            keyboard.println(); // Newline after each line
            linesInBatch++;
            linesPrinted++;
        }

        file.close();
        delay(100); // Pause between batches
    }
}
*/