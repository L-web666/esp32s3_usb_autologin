#ifdef ENABLE_LITTLEFS_HANDLER

#include "LittleFsHandler.h"
#include "Globals.h"
#include "CommandHandler.h"
#include <LittleFS.h>

void LittleFsHandler::init()
{
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }

    registerCommands();
    debugI("LittleFsHandler initialized");
}

void LittleFsHandler::loop()
{
    // No recurring logic required here for now
}

bool LittleFsHandler::writeFile(const String &path, const String &content, bool append)
{
    File file = LittleFS.open(path, append ? "a" : "w");
    if (!file)
    {
        debugE("Failed to open file for writing: %s", path.c_str());
        return false;
    }
    file.print(content);
    file.close();
    return true;
}

String LittleFsHandler::readFile(const String &path)
{
    File file = LittleFS.open(path, "r");
    if (!file)
    {
        debugE("Failed to open file for reading: %s", path.c_str());
        return "";
    }

    String content;
    while (file.available())
    {
        content += (char)file.read();
    }
    file.close();
    return content;
}

bool LittleFsHandler::deleteFile(const String &path)
{
    return LittleFS.remove(path);
}

bool LittleFsHandler::deleteAllFiles()
{
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory())
    {
        debugE("Failed to open root directory");
        return false;
    }

    File file = root.openNextFile();
    while (file)
    {
        LittleFS.remove(file.name());
        file = root.openNextFile();
    }

    return true;
}

bool LittleFsHandler::createFolder(const String &path)
{
    return LittleFS.mkdir(path);
}

bool LittleFsHandler::deleteRecursive(const String &path)
{
    File dir = LittleFS.open(path);
    if (!dir.isDirectory())
    {
        return LittleFS.remove(path); // Try deleting as a file
    }

    File file = dir.openNextFile();
    while (file)
    {
        String filePath = String(path) + "/" + file.name();
        if (!deleteRecursive(filePath))
        {
            return false;
        }
        file = dir.openNextFile();
    }
    return LittleFS.rmdir(path);
}

void LittleFsHandler::listFiles()
{
    debugI("Listing all files in LittleFS:");
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory())
    {
        debugE("Failed to open root directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        debugI("File: %s, Size: %d", file.name(), file.size());
        file = root.openNextFile();
    }
}

void LittleFsHandler::registerCommands()
{
    CommandHandler::registerCommand("LITTLEFS", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "LIST")) {
            listFiles();
        } else if (CommandHandler::equalsIgnoreCase(cmd, "FORMAT")) {
            if (LittleFS.format()) {
                debugI("LittleFS formatted successfully");
            } else {
                debugE("Failed to format LittleFS");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "WRITE")) {
            String path, content;
            int splitIndex = args.indexOf(' ');
            if (splitIndex > 0) {
                path = args.substring(0, splitIndex);
                content = args.substring(splitIndex + 1);
                if (writeFile(path, content)) {
                    debugI("File written successfully: %s", path.c_str());
                } else {
                    debugE("Failed to write file: %s", path.c_str());
                }
            } else {
                debugW("Usage: LITTLEFS WRITE <path> <content>");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "READ")) {
            if (!args.isEmpty()) {
                String content = readFile(args);
                if (!content.isEmpty()) {
                    debugI("Content of %s: %s", args.c_str(), content.c_str());
                } else {
                    debugE("Failed to read file or file is empty: %s", args.c_str());
                }
            } else {
                debugW("Usage: LITTLEFS READ <path>");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "DELETE")) {
            if (!args.isEmpty()) {
                if (deleteFile(args)) {
                    debugI("File deleted successfully: %s", args.c_str());
                } else {
                    debugE("Failed to delete file: %s", args.c_str());
                }
            } else {
                debugW("Usage: LITTLEFS DELETE <path>");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "DELETE_ALL")) {
            if (deleteAllFiles()) {
                debugI("All files deleted successfully");
            } else {
                debugE("Failed to delete all files");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "MKDIR")) {
            if (!args.isEmpty()) {
                if (createFolder(args)) {
                    debugI("Folder created successfully: %s", args.c_str());
                } else {
                    debugE("Failed to create folder: %s", args.c_str());
                }
            } else {
                debugW("Usage: LITTLEFS MKDIR <path>");
            }
        } else if (CommandHandler::equalsIgnoreCase(cmd, "RMDIR")) {
            if (!args.isEmpty()) {
                if (deleteRecursive(args)) {
                    debugI("Folder deleted recursively: %s", args.c_str());
                } else {
                    debugE("Failed to delete folder: %s", args.c_str());
                }
            } else {
                debugW("Usage: LITTLEFS RMDIR <path>");
            }
        } else {
            debugW("Unknown LITTLEFS subcommand: %s", cmd.c_str());
        } }, "Handles LittleFS commands. Usage: LITTLEFS <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "  LIST - Lists all files in LittleFS\n"
                                         "  FORMAT - Formats LittleFS\n"
                                         "  WRITE <path> <content> - Writes to a file\n"
                                         "  READ <path> - Reads a file\n"
                                         "  DELETE <path> - Deletes a file\n"
                                         "  DELETE_ALL - Deletes all files\n"
                                         "  MKDIR <path> - Creates a folder\n"
                                         "  RMDIR <path> - Recursively deletes a folder and its contents");

    CommandHandler::registerCommandAlias("lfs", "littlefs");
}

#endif // ENABLE_LITTLEFS_HANDLER
