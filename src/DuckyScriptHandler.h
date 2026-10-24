#pragma once

#ifdef ENABLE_DUCKYSCRIPT_HANDLER

#include "Globals.h"

class DuckyScriptHandler
{
private:
    static void processLine(const String &line);
    static void registerCommands();
    static void pressKeyCombo(const String &line);

public:
    static void init();
    static void executeScript(const String &filePath);
};

#else

class DuckyScriptHandler
{
public:
    static void init() {} // No-op
    static void executeScript(const String &filePath) {} // No-op
};

#endif // ENABLE_DUCKYSCRIPT_HANDLER
