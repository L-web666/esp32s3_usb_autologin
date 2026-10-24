//CommandHandler.h

#pragma once

#include <Arduino.h>
#include <map>
#include <functional>

class CommandHandler {
public:
    static void handleCommand(const String& command);
    static void registerCommand(const String& name, std::function<void(const String&)> handler, const String& description = "");
    static void registerCommandAlias(const String& alias, const String& existingCommand);
    static void listCommands();
    static void setDefaultHandler(std::function<void(const String&)> handler);
    static void parseCommand(const String& input, String& cmd, String& args);
    static bool equalsIgnoreCase(const String &a, const String &b);
    static void init();

private:
    static std::map<String, std::function<void(const String&)>> commandRegistry;
    static std::map<String, String> commandDescriptions;
    static std::function<void(const String&)> defaultHandler;
};
