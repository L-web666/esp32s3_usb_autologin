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

void TemplateHandler::debugLevels()
{
    // Example of debug levels
    debugV("* This is a message of debug level VERBOSE");
    debugD("* This is a message of debug level DEBUG");
    debugI("* This is a message of debug level INFO");
    debugW("* This is a message of debug level WARNING");
    debugE("* This is a message of debug level ERROR");
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