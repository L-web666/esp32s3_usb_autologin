I would like to implement DuckyScript my esp32-s3 microcontroller project.

I have multiple handlers in my code that already handle keys and mouse. 

I just need this code to read a script file path using LittleFS and follow standard DuckyScript.

void DeviceHandler::processKey(const String &keyName, bool press) // Convert Arduino String to std::string for map lookup
void DeviceHandler::sendKeys1(const String &text) // Send a string
void DeviceHandler::sendMouseMovement(int x, int y)

All of my code uses the following pattern. Can you provide example following same pattern as below?

//TemplateHandler.h

#pragma once

#ifdef ENABLE_TEMPLATE_HANDLER

class TemplateHandler
{
private:
    static void debugLevels();
    static void registerCommands();

public:
    static void init();
    static void loop();
};

#else

class TemplateHandler { 
public: // No-op implementation of TemplateHandler
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_TEMPLATE_HANDLER

//TemplateHandler.cpp

#ifdef ENABLE_TEMPLATE_HANDLER

#include "TemplateHandler.h"
#include "Globals.h"

static NonBlockingTimer myTimer(60000);

void TemplateHandler::init()
{
    registerCommands();
    debugI("TemplateHandler initialized");
}

void TemplateHandler::loop()
{
    if (myTimer.isReady())
    {
        debugI("TemplateHandler loop timer.");
    }
}

void TemplateHandler::registerCommands()
{
    CommandHandler::registerCommand("TEMPLATE", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "DEBUG")) {
            debugLevels();
        } else if (CommandHandler::equalsIgnoreCase(cmd, "HELLO")) {
            debugI("Hello World!");
        } else {
            debugW("Unknown TEMPLATE subcommand: %s", cmd.c_str());
        } }, "Handles TEMPLATE commands. Usage: TEMPLATE <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "  debug - Prints debug levels\n"
                                         "  hello - Prints 'Hello World!'");
}

#endif // ENABLE_TEMPLATE_HANDLER

// CommandHandler.cpp

#include "CommandHandler.h"

std::map<String, std::function<void(const String &)>> CommandHandler::commandRegistry;
std::map<String, String> CommandHandler::commandDescriptions;
std::function<void(const String &)> CommandHandler::defaultHandler = nullptr;

void CommandHandler::parseCommand(const String &input, String &cmd, String &args)
{
    int spaceIndex = input.indexOf(' ');
    if (spaceIndex > 0)
    {
        cmd = input.substring(0, spaceIndex);
        args = input.substring(spaceIndex + 1);
    }
    else
    {
        cmd = input;
        args = "";
    }

    cmd.toLowerCase(); // Normalize command case
}

void CommandHandler::handleCommand(const String &command)
{
    String cmd, args;
    parseCommand(command, cmd, args);

    auto it = commandRegistry.find(cmd);
    if (it != commandRegistry.end())
    {
        debugV("* Executing command: %s with args: %s", cmd.c_str(), args.c_str());
        it->second(args); // Call the registered handler
    }
    else if (defaultHandler)
    {
        debugV("* Calling default handler for command: %s", command.c_str());
        defaultHandler(command);
    }
    else
    {
        debugE("* Unknown command received: %s", cmd.c_str());
    }
}

void CommandHandler::registerCommand(const String &name, std::function<void(const String &)> handler, const String &description)
{
    String lowerName = name;
    lowerName.toLowerCase();
    if (commandRegistry.find(lowerName) != commandRegistry.end())
    {
        debugW("* Warning: Command '%s' is being overwritten.", lowerName.c_str());
    }
    commandRegistry[lowerName] = handler;
    commandDescriptions[lowerName] = description;
    debugD("* Command '%s' registered with description: %s", lowerName.c_str(), description.c_str());
}

void CommandHandler::registerCommandAlias(const String &alias, const String &existingCommand)
{
    String lowerAlias = alias;
    String lowerCommand = existingCommand;
    lowerAlias.toLowerCase();
    lowerCommand.toLowerCase();

    if (commandRegistry.find(lowerCommand) != commandRegistry.end())
    {
        commandRegistry[lowerAlias] = commandRegistry[lowerCommand];
        debugV("* Alias '%s' registered for command '%s'.", lowerAlias.c_str(), lowerCommand.c_str());
    }
    else
    {
        debugE("* Error: Command '%s' not found for alias registration.", existingCommand.c_str());
    }
}

void CommandHandler::listCommands()
{
    debugI("* Listing all available commands:");
    for (const auto &pair : commandDescriptions)
    {
        debugI("* Command: %s - %s", pair.first.c_str(), pair.second.c_str());
    }
}

void CommandHandler::setDefaultHandler(std::function<void(const String &)> handler)
{
    defaultHandler = handler;
    debugD("* Default handler set.");
}

bool CommandHandler::equalsIgnoreCase(const String &a, const String &b)
{
    return a.equalsIgnoreCase(b);
}

void CommandHandler::init()
{
    CommandHandler::setDefaultHandler([](const String &command) { // Set the default handler to call the 'help' command
        debugI("Unknown command: %s. Showing help menu.", command.c_str());
        CommandHandler::listCommands(); // Invoke the help command
    });

    CommandHandler::registerCommand("help", [](const String &) { // Register a "help" command
        CommandHandler::listCommands();
    },"Lists all available commands.");

    CommandHandler::registerCommand("debug", [](const String &command) { // Register a "debug" print command
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);
        debugI("Command:%s", command.c_str());
        debugI("CMD:%s", cmd.c_str());
        debugI("ARGS:%s", args.c_str());
    },"Prints a debug message. Usage: debug <message>");

    debugD("* CommandHandler initialized.");
}