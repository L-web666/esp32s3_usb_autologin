#pragma once

#ifdef ENABLE_LITTLEFS_HANDLER

#include <Arduino.h>

class LittleFsHandler
{
private:
    static void registerCommands();
    static bool deleteRecursive(const String &path);
    static void listFiles();

public:
    static void init();
    static void loop();

    // Helper methods for file operations
    static bool writeFile(const String &path, const String &content, bool append = false);
    static String readFile(const String &path);
    static bool deleteFile(const String &path);
    static bool deleteAllFiles();
    static bool createFolder(const String &path);
};

#else

class LittleFsHandler
{
public: // No-op implementation of LittleFsHandler
    static void init() {}
    static void loop() {}
};

#endif // ENABLE_LITTLEFS_HANDLER
