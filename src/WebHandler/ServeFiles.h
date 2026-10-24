#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class ServeFiles {
public:
    static void registerEndpoints(AsyncWebServer &server);

private:
    static void handleFileManager(AsyncWebServerRequest *request);
    static void listFilesAndFolders(JsonArray &files, JsonArray &folders, const String &path);
    static void handleSearch(AsyncWebServerRequest *request);
    static void searchRecursive(JsonArray &results, const String &path, const String &query);
    static void handleRename(AsyncWebServerRequest *request);
    static void listFoldersRecursive(JsonArray &folders, const String &path);
    static void handleListFolders(AsyncWebServerRequest *request);
    static void handleCreateFolder(AsyncWebServerRequest *request);
    static void handleDeleteFolder(AsyncWebServerRequest *request);
    static bool deleteFolderRecursive(const String &folderPath);
    static bool ensureParentDirsExist(const String &filePath);
    static void listFilesRecursive(JsonArray &files, const String &path);
    static void handleListFiles(AsyncWebServerRequest *request);
    static void handleReadFile(AsyncWebServerRequest *request);
    static void handleWriteFile(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static bool isProtectedFile(const String &filename);
    static void handleDeleteFile(AsyncWebServerRequest *request);
    //TODO cleanup
    static void listFilesRecursiveInFolder(JsonArray &files, const String &basePath, const String &currentPath);
    static void handleListFilesInFolder(AsyncWebServerRequest *request);
};

#endif // ENABLE_WEB_HANDLER
