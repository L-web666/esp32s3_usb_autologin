#ifdef ENABLE_BUTTON_HANDLER

#include "ButtonHandler.h"
#include "Globals.h"
#include "ConfigManager.h"
#include "CommandHandler.h"
#include "GfxHandler.h"
#include "LedHandler.h"
#include "CryptoHandler.h"
#include "DeviceHandler.h"
#include "DuckyScriptHandler.h"
#include <OneButton.h>
#include <LittleFS.h>

// Configurable durations (in milliseconds)
const unsigned long REBOOT_HOLD_DURATION_MS = 5000; // 5 seconds total hold time to reboot
const unsigned long COUNTDOWN_START_MS = 2000; // Start countdown after 2 seconds (so countdown lasts 3 seconds)

// Ensure the countdown duration is valid
static_assert(REBOOT_HOLD_DURATION_MS > COUNTDOWN_START_MS, "REBOOT_HOLD_DURATION_MS must be greater than COUNTDOWN_START_MS");

// Static member initialization
unsigned long ButtonHandler::longPressStartTime = 0;
bool ButtonHandler::rebootTriggered = false;
int ButtonHandler::lastCountdownValue = -1;
static OneButton button(BUTTON_PIN, true);

// Initialize the button
void ButtonHandler::init()
{
    debugI("ButtonHandler initialized on pin: %d", BUTTON_PIN);
    button.attachClick(handleSingleClick);
    button.attachDoubleClick(handleDoubleClick);
    button.attachLongPressStart(handleLongPress);
    button.attachDuringLongPress(handleDuringLongPress);
    button.attachLongPressStop(handleLongPressStop);
    registerCommands();
}

// Update the button state (call in loop)
void ButtonHandler::loop()
{
    button.tick();
}

// Button action handlers
void ButtonHandler::handleSingleClick()
{
    debugI("Single click.");
    CommandHandler::handleCommand(settings.device.singlePress);
}

void ButtonHandler::handleDoubleClick()
{
    debugI("Double click.");
    CommandHandler::handleCommand(settings.device.doublePress);
}

void ButtonHandler::handleLongPress()
{
    debugI("Long press started.");
    CommandHandler::handleCommand(settings.device.longPress);
    longPressStartTime = millis(); // Record the start time of the long press
    rebootTriggered = false; // Reset the reboot flag
    lastCountdownValue = -1; // Reset the countdown value
}

void ButtonHandler::handleDuringLongPress()
{
    // Calculate how long the button has been held
    unsigned long currentTime = millis();
    unsigned long holdDuration = currentTime - longPressStartTime;

    debugI("During long press. Hold duration: %lu ms", holdDuration);

    // Check if we should start the countdown
    if (holdDuration >= COUNTDOWN_START_MS && !rebootTriggered) {
        // Calculate remaining time until reboot (in seconds)
        unsigned long remainingTimeMs = REBOOT_HOLD_DURATION_MS - holdDuration;
        int remainingSeconds = (remainingTimeMs + 999) / 1000; // Round up to the nearest second

        // Only update the display if the countdown value has changed
        if (remainingSeconds != lastCountdownValue && remainingSeconds >= 0) {
            lastCountdownValue = remainingSeconds;
            String message = "Rebooting in " + String(remainingSeconds) + " seconds...";
            GfxHandler::printMessage(message);
            LedHandler::setColorByName("orange");
            debugI("%s", message.c_str());
        }
    }

    // Check if the hold duration exceeds the reboot threshold and reboot hasn't been triggered yet
    if (holdDuration >= REBOOT_HOLD_DURATION_MS && !rebootTriggered) {
        debugI("Hold duration exceeded %lu ms. Rebooting device...", REBOOT_HOLD_DURATION_MS);
        rebootTriggered = true; // Set the flag to prevent multiple reboots
        LedHandler::setColorByName("purple");
        GfxHandler::printMessage("Clearing preferences...");
        ConfigManager::clearPreferences(); // Clear all preferences
        delay(1000); // Brief delay to ensure the message is displayed
        GfxHandler::printMessage("Rebooting now...");
        delay(1000); // Brief delay to ensure the message is displayed
        ESP.restart(); // Reboot the device (ESP32-specific)
    }
}

void ButtonHandler::handleLongPressStop()
{
    debugI("Long press stopped.");
    longPressStartTime = 0; // Reset the start time
    rebootTriggered = false; // Reset the reboot flag
    lastCountdownValue = -1; // Reset the countdown value
    GfxHandler::printMessage(""); // Clear the display message
}

void ButtonHandler::runButton(int id) {
    debugI("Running button with ID: %d", id);

    File file = LittleFS.open(BUTTONS_FILE, "r");
    if (!file) {
        debugE("Failed to open buttons file");
        return;
    }

    static JsonDocument buttonDoc;
    DeserializationError error = deserializeJson(buttonDoc, file);
    file.close();

    if (error) {
        debugE("Failed to deserialize JSON: %s", error.c_str());
        return;
    }

    const JsonArray buttons = buttonDoc["buttons"].as<JsonArray>();
    for (const JsonObject& button : buttons) {
        if (button["id"].as<int>() == id) {
            debugI("Found button with ID: %d", id);
            executeButtonAction(button);
            return;
        }
    }

    debugW("Button ID not found: %d", id);
}

void ButtonHandler::executeButtonAction(const JsonObject& button) {
    const String deviceAction = button["deviceAction"].as<String>();
    const String passwordAction = button["passwordAction"].as<String>();
    const String usernameAction = button["usernameAction"].as<String>();
    
    if (deviceAction == "1") {  // Login credentials
        
        DeviceHandler::sendKeys(button["userName"].as<String>());
        
        // If usernameAction is 1 (TAB) or 2 (ENTER)
        if (usernameAction == "1") {
            DeviceHandler::tapKey("TAB");
        } else if (usernameAction == "2") {
            DeviceHandler::tapKey("ENTER");
        }

        String decryptedPassword = CryptoHandler::decryptAES(
            button["userPassword"].as<String>(), 
            settings.device.userPassword
        );
        
        DeviceHandler::sendKeys(decryptedPassword);

        // If passwordAction is 1 (TAB) or 2 (ENTER)
        if (passwordAction == "1") {
            DeviceHandler::tapKey("TAB");
        } else if (passwordAction == "2") {
            DeviceHandler::tapKey("ENTER");
        }
    }
    else if (deviceAction == "2") {  // Command
        CommandHandler::handleCommand(button["command"].as<String>());
    }
    else if (deviceAction == "3") {  // DuckyScript
        String filePath = button["script"].as<String>();
        DuckyScriptHandler::executeScript(filePath);
        //CommandHandler::handleCommand("script file " + filePath);
    }
}

void ButtonHandler::registerCommands()
{
    CommandHandler::registerCommand("BUTTON", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "RUN")) {
            runButton(args.toInt());
        } else {
            debugW("Unknown BUTTON subcommand: %s", cmd.c_str());
        } }, "Handles BUTTON commands. Usage: BUTTON <subcommand> <args>\n"
                                         "  Subcommands:\n"
                                         "  run <id> - Finds and runs button by id");
        
        CommandHandler::registerCommandAlias("BTN", "BUTTON");
}

#endif // ENABLE_BUTTON_HANDLER