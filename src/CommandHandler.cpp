// CommandHandler.cpp

#include "CommandHandler.h"
#include "Globals.h"

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

    // CommandHandler::registerCommand("help", [](const String &) { // Register a "help" command
    //     CommandHandler::listCommands();
    // },"Lists all available commands.");

    CommandHandler::registerCommand("help", [](const String &args)
                                    {
    if (args.isEmpty())
    {
        // List all command names
        debugI("Available commands:");
        for (const auto &pair : commandDescriptions)
        {
            debugI("* %s", pair.first.c_str());
        }
        debugI("Type 'help <command>' for more details on a specific command.");
    }
    else
    {
        // Show details for a specific command
        String lowerArgs = args;
        lowerArgs.toLowerCase();
        auto it = commandDescriptions.find(lowerArgs);
        if (it != commandDescriptions.end())
        {
            debugI("Command: %s", it->first.c_str());
            debugI("Description: %s", it->second.c_str());
        }
        else
        {
            debugE("Unknown command: %s. Use 'help' to list all commands.", args.c_str());
        }
    } }, "Lists all available commands or shows details for a specific command. Usage: help [command]");

    CommandHandler::registerCommand("reboot", [](const String &) { // Register a "reboot" command
        ESP.restart();
    },
                                    "Reboot the device.");

    // CommandHandler::registerCommand("debug", [](const String &command) { // Register a "debug" print command
    //     String cmd, args;
    //     CommandHandler::parseCommand(command, cmd, args);
    //     debugI("Command:%s", command.c_str());
    //     debugI("CMD:%s", cmd.c_str());
    //     debugI("ARGS:%s", args.c_str());
    // },
    //                                 "Prints a debug message. Usage: debug <message>");

    debugD("* CommandHandler initialized.");
}