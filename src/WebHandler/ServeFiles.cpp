#ifdef ENABLE_WEB_HANDLER

#include "ServeFiles.h"
#include "Globals.h"
#include "WebHandler.h"
#include <LittleFS.h>

// Register endpoints for file management
void ServeFiles::registerEndpoints(AsyncWebServer &server)
{
    server.on("/files", HTTP_GET, handleListFiles);                                              // List files
    server.on("/file", HTTP_GET, handleReadFile);                                                // Read a file
    server.on("/file", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleWriteFile); // Write a file
    server.on("/file", HTTP_DELETE, handleDeleteFile);                                           // Delete a file
    server.on("/folder", HTTP_POST, handleCreateFolder);                                         // Create folder
    server.on("/folder", HTTP_DELETE, handleDeleteFolder);                                       // Delete folder
    server.on("/folders", HTTP_GET, handleListFolders);                                          // List folders
    server.on("/rename", HTTP_POST, handleRename);                                               // Rename file or folder
    server.on("/filemanager", HTTP_GET, handleFileManager);                                      // List files and folders together
    server.on("/folder/files", HTTP_GET, handleListFilesInFolder);                               // New endpoint: List files in a folder
}

// Handler for `/filemanager`
void ServeFiles::handleFileManager(AsyncWebServerRequest *request)
{
    String path = "/";
    if (request->hasParam("path"))
    {
        path = request->getParam("path")->value();
    }
    if (!path.endsWith("/"))
    {
        path += "/";
    }

    debugV("Received request for file manager with path: %s", path.c_str());

    File dir = LittleFS.open(path);
    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", path.c_str());
        WebHandler::sendErrorResponse(request, 404, "Invalid directory path");
        return;
    }

    JsonDocument doc;
    JsonArray files = doc["files"].to<JsonArray>();
    JsonArray folders = doc["folders"].to<JsonArray>();

    // List files and folders
    listFilesAndFolders(files, folders, path);

    doc["path"] = path;
    WebHandler::sendSuccessResponse(request, "Directory contents fetched successfully", &doc);
}

// Helper to list files and folders
void ServeFiles::listFilesAndFolders(JsonArray &files, JsonArray &folders, const String &path)
{
    File dir = LittleFS.open(path);
    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", path.c_str());
        return;
    }

    File file = dir.openNextFile();
    while (file)
    {
        String name = file.name();
        if (name.startsWith(path))
        {
            name = name.substring(path.length());
        }
        if (file.isDirectory())
        {
            debugV("Found folder: %s", name.c_str());
            folders.add(name);
        }
        else
        {
            debugV("Found file: %s", name.c_str());
            files.add(name);
        }
        file = dir.openNextFile();
    }
}

void ServeFiles::handleSearch(AsyncWebServerRequest *request)
{
    if (!request->hasParam("query"))
    {
        WebHandler::sendErrorResponse(request, 400, "Search query is required");
        return;
    }

    String query = request->getParam("query")->value();

    JsonDocument doc;
    JsonArray results = doc["results"].to<JsonArray>();

    // Search files and folders
    searchRecursive(results, "/", query);

    WebHandler::sendSuccessResponse(request, "Search completed", &doc);
}

void ServeFiles::searchRecursive(JsonArray &results, const String &path, const String &query)
{
    File dir = LittleFS.open(path);
    if (!dir || !dir.isDirectory())
        return;

    File file = dir.openNextFile();
    while (file)
    {
        String name = file.name();
        if (name.indexOf(query) >= 0)
        {
            results.add(path + name);
        }
        if (file.isDirectory())
        {
            searchRecursive(results, path + name + "/", query);
        }
        file = dir.openNextFile();
    }
}

void ServeFiles::handleRename(AsyncWebServerRequest *request)
{
    if (!request->hasParam("oldname") || !request->hasParam("newname"))
    {
        WebHandler::sendErrorResponse(request, 400, "Both oldname and newname are required");
        return;
    }

    String oldName = request->getParam("oldname")->value();
    String newName = request->getParam("newname")->value();

    if (!LittleFS.exists(oldName))
    {
        WebHandler::sendErrorResponse(request, 404, "File or folder not found");
        return;
    }

    if (LittleFS.rename(oldName, newName))
    {
        WebHandler::sendSuccessResponse(request, "Renamed successfully");
    }
    else
    {
        WebHandler::sendErrorResponse(request, 500, "Failed to rename");
    }
}

void ServeFiles::listFoldersRecursive(JsonArray &folders, const String &path)
{
    String dirPath = path.isEmpty() ? "/" : (path.endsWith("/") ? path : path + "/");

    debugV("Opening directory: %s", dirPath.c_str());
    File dir = LittleFS.open(dirPath);

    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", dirPath.c_str());
        return;
    }

    File file = dir.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            String fullPath = dirPath + file.name();
            debugV("Found folder: %s", fullPath.c_str());
            folders.add(fullPath);
            // Recursively list subdirectories
            listFoldersRecursive(folders, fullPath);
        }
        file = dir.openNextFile();
    }
}

void ServeFiles::handleListFolders(AsyncWebServerRequest *request)
{
    debugV("Received GET request on /folders");

    JsonDocument doc;
    JsonArray folders = doc["folders"].to<JsonArray>();

    // Start recursive folder listing from the root directory
    listFoldersRecursive(folders, "/");

    WebHandler::sendSuccessResponse(request, "Folders listed successfully", &doc);
}

void ServeFiles::handleCreateFolder(AsyncWebServerRequest *request)
{
    if (!request->hasParam("foldername"))
    {
        WebHandler::sendErrorResponse(request, 400, "Folder name is required");
        return;
    }
    String folderName = request->getParam("foldername")->value();
    if (LittleFS.mkdir(folderName))
    {
        WebHandler::sendSuccessResponse(request, "Folder created successfully");
    }
    else
    {
        WebHandler::sendErrorResponse(request, 500, "Failed to create folder");
    }
}

void ServeFiles::handleDeleteFolder(AsyncWebServerRequest *request)
{
    if (!request->hasParam("foldername"))
    {
        WebHandler::sendErrorResponse(request, 400, "Folder name is required");
        return;
    }
    String folderName = request->getParam("foldername")->value();
    if (deleteFolderRecursive(folderName))
    {
        WebHandler::sendSuccessResponse(request, "Folder deleted successfully");
    }
    else
    {
        WebHandler::sendErrorResponse(request, 500, "Failed to delete folder");
    }
}

/*
bool ServeFiles::deleteFolderRecursive(const String &folderPath)
{
    File dir = LittleFS.open(folderPath);
    if (!dir || !dir.isDirectory())
        return false;

    File file = dir.openNextFile();
    while (file)
    {
        String filePath = String(folderPath) + "/" + file.name();
        if (file.isDirectory())
        {
            deleteFolderRecursive(filePath);
        }
        else
        {
            LittleFS.remove(filePath);
        }
        file = dir.openNextFile();
    }
    return LittleFS.rmdir(folderPath);
}
*/

//Does not appear to work with nested files skipping the recursive delete
bool ServeFiles::deleteFolderRecursive(const String &folderPath)
{
    debugI("Attempting to delete folder: %s", folderPath.c_str());

    // Open the directory
    File dir = LittleFS.open(folderPath);
    if (!dir || !dir.isDirectory())
    {
        debugI("Error: Unable to open folder or not a directory: %s", folderPath.c_str());
        return false;
    }

    // Iterate through all files and subdirectories
    while (true)
    {
        File file = dir.openNextFile();
        if (!file)
        {
            // No more files
            break;
        }

        String filePath = folderPath + "/" + file.name(); // Construct the full path
        if (file.isDirectory())
        {
            // Recursively delete subdirectories
            debugI("Entering directory: %s", filePath.c_str());
            if (!deleteFolderRecursive(filePath))
            {
                debugI("Error: Failed to delete directory: %s", filePath.c_str());
                file.close();
                dir.close();
                return false;
            }
        }
        else
        {
            // Delete the file
            debugI("Deleting file: %s", filePath.c_str());
            if (!LittleFS.remove(filePath))
            {
                debugI("Error: Failed to delete file: %s", filePath.c_str());
                file.close();
                dir.close();
                return false;
            }
            debugI("Successfully deleted file: %s", filePath.c_str());
        }
        file.close(); // Explicitly close the file
    }

    dir.close(); // Close the directory handle

    // Attempt to remove the directory itself
    debugI("Deleting folder: %s", folderPath.c_str());
    if (!LittleFS.rmdir(folderPath))
    {
        debugI("Error: Failed to delete folder: %s", folderPath.c_str());
        return false;
    }

    debugI("Successfully deleted folder: %s", folderPath.c_str());
    return true;
}

// Helper to ensure parent directories exist
bool ServeFiles::ensureParentDirsExist(const String &filePath)
{
    int lastSlashIndex = filePath.lastIndexOf('/');
    if (lastSlashIndex <= 0)
        return true; // No parent directories

    String parentPath = filePath.substring(0, lastSlashIndex);
    debugV("Ensuring directory exists: %s", parentPath.c_str());

    if (LittleFS.exists(parentPath))
        return true;                   // Directory already exists
    return LittleFS.mkdir(parentPath); // Create the directory
}

// Recursive helper for listing files
void ServeFiles::listFilesRecursive(JsonArray &files, const String &path)
{
    // Ensure the directory path starts with "/" and does not end with "/"
    String dirPath = path.isEmpty() ? "/" : (path.endsWith("/") ? path : path + "/");

    debugV("Opening directory: %s", dirPath.c_str());
    File dir = LittleFS.open(dirPath);

    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", dirPath.c_str());
        return;
    }

    File file = dir.openNextFile();
    while (file)
    {
        String fullPath = dirPath + file.name();

        if (file.isDirectory())
        {
            debugV("Entering directory: %s", fullPath.c_str());
            // Recursively list files in subdirectory
            listFilesRecursive(files, fullPath);
        }
        else
        {
            debugV("Found file: %s", fullPath.c_str());
            files.add(fullPath);
        }

        file = dir.openNextFile();
    }
}

// Handle listing files
void ServeFiles::handleListFiles(AsyncWebServerRequest *request)
{
    debugV("Received GET request on /files");

    JsonDocument doc;
    JsonArray files = doc["files"].to<JsonArray>();

    // Start recursive listing from the root directory
    listFilesRecursive(files, "/");

    WebHandler::sendSuccessResponse(request, "Files listed successfully", &doc);
}

// Handle reading a file
void ServeFiles::handleReadFile(AsyncWebServerRequest *request)
{
    debugV("Received GET request on /file");

    if (!request->hasParam("filename"))
    {
        debugE("Missing filename parameter");
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();
    debugV("Requested file: %s", filename.c_str());

    if (!LittleFS.exists(filename))
    {
        debugE("File not found: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 404, "File not found");
        return;
    }

    File file = LittleFS.open(filename, "r");
    if (!file)
    {
        debugE("Failed to open file: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to open file");
        return;
    }

    String content = file.readString();
    file.close();
    debugV("File content: %s", content.c_str());

    // request->send(200, "text/plain", content);

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", content);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

// Handle writing to a file
void ServeFiles::handleWriteFile(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /file");

    if (!request->hasParam("filename"))
    {
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();
    debugV("Filename: %s", filename.c_str());

    // Ensure parent directories exist
    if (!ensureParentDirsExist(filename))
    {
        debugE("Failed to create parent directories for: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to create directories");
        return;
    }

    debugV("Data length: %d, Index: %d, Total: %d", len, index, total);

    File file = LittleFS.open(filename, index == 0 ? "w" : "a");
    if (!file)
    {
        debugE("Failed to open file for writing: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to open file for writing");
        return;
    }

    file.write(data, len);
    file.close();
    debugV("Data written to file: %s", filename.c_str());

    if (index + len == total)
    {
        WebHandler::sendSuccessResponse(request, "File saved successfully");
    }
}

/*
static String fileBuffer;
void ServeFiles::handleWriteFile(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (!request->hasParam("filename")) {
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }

    String filename = request->getParam("filename")->value();

    // Add the chunk to the buffer
    fileBuffer.concat((char *)data, len);

    // Write to file when all chunks are received
    if (index + len == total) {
        File file = LittleFS.open(filename, "w");
        if (!file) {
            WebHandler::sendErrorResponse(request, 500, "Failed to open file for writing");
            return;
        }
        file.print(fileBuffer); // Write the entire buffer
        file.close();
        fileBuffer = ""; // Clear the buffer
        WebHandler::sendSuccessResponse(request, "File saved successfully");
    }
}
*/

// Check if a file is protected
bool ServeFiles::isProtectedFile(const String &filename)
{
    static const char *protectedFiles[] = {
        //"/settings.json",
        //"/wifi_networks.json",
        //"/test.txt"
    };

    for (const char *protectedFile : protectedFiles)
    {
        if (filename == protectedFile)
            return true;
    }
    return false;
}

// Handle deleting a file
void ServeFiles::handleDeleteFile(AsyncWebServerRequest *request)
{
    debugV("Received DELETE request on /file");

    if (!request->hasParam("filename"))
    {
        debugE("Missing filename parameter");
        WebHandler::sendErrorResponse(request, 400, "Filename is required");
        return;
    }
    String filename = request->getParam("filename")->value();
    debugV("Filename to delete: %s", filename.c_str());

    if (isProtectedFile(filename))
    {
        debugE("Cannot delete protected file: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 403, "Cannot delete protected file: %s", filename.c_str());
        // WebHandler::sendErrorResponse(request, 403, "Cannot modify protected file");
        return;
    }

    if (LittleFS.remove(filename))
    {
        debugV("File deleted: %s", filename.c_str());
        WebHandler::sendSuccessResponse(request, "File deleted successfully");
    }
    else
    {
        debugE("Failed to delete file: %s", filename.c_str());
        WebHandler::sendErrorResponse(request, 500, "Failed to delete file");
    }
}

// This may be repeated code TODO: Refactor and cleanup later

// Recursive helper to list files in a folder and its subfolders
void ServeFiles::listFilesRecursiveInFolder(JsonArray &files, const String &basePath, const String &currentPath)
{
    String dirPath = currentPath.isEmpty() ? basePath : currentPath;
    if (!dirPath.endsWith("/"))
    {
        dirPath += "/";
    }

    debugV("Opening directory: %s", dirPath.c_str());
    File dir = LittleFS.open(dirPath);

    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", dirPath.c_str());
        return;
    }

    File file = dir.openNextFile();
    while (file)
    {
        String fullPath = dirPath + file.name();
        String relativePath = fullPath.substring(basePath.length()); // Relative to the base path

        if (file.isDirectory())
        {
            debugV("Entering subdirectory: %s", fullPath.c_str());
            listFilesRecursiveInFolder(files, basePath, fullPath); // Recurse into subfolder
        }
        else
        {
            debugV("Found file: %s", fullPath.c_str());
            files.add(relativePath); // Add file with path relative to the base folder
        }
        file = dir.openNextFile();
    }
}

// Handler for `/folder/files` with subfolder support
void ServeFiles::handleListFilesInFolder(AsyncWebServerRequest *request)
{
    String path = "/";
    if (request->hasParam("path"))
    {
        path = request->getParam("path")->value();
    }
    if (!path.endsWith("/"))
    {
        path += "/";
    }

    debugV("Received request to list files in folder (with subfolders): %s", path.c_str());

    File dir = LittleFS.open(path);
    if (!dir || !dir.isDirectory())
    {
        debugE("Failed to open directory: %s", path.c_str());
        WebHandler::sendErrorResponse(request, 404, "Invalid directory path");
        return;
    }

    JsonDocument doc;
    JsonArray files = doc["files"].to<JsonArray>();

    // List files recursively starting from the specified path
    listFilesRecursiveInFolder(files, path, path);

    doc["path"] = path;
    WebHandler::sendSuccessResponse(request, "Files in folder and subfolders listed successfully", &doc);
}

#endif // ENABLE_WEB_HANDLER