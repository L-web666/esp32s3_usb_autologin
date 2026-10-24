#ifdef ENABLE_DOWNLOAD_HANDLER

#include "DownloadHandler.h"
#include <LittleFS.h>
#include <HTTPClient.h>

void DownloadHandler::init()
{
    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }

    registerCommands();

    debugI("DownloadHandler initialized");
}

bool DownloadHandler::downloadFile(const String &url, const String &destinationPath)
{
    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        debugE("HTTP GET failed, error: %d", httpCode);
        http.end();
        return false;
    }

    File file = LittleFS.open(destinationPath, "w");
    if (!file)
    {
        debugE("Failed to open file for writing: %s", destinationPath.c_str());
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    if (contentLength <= 0)
    {
        debugE("Invalid content length");
        file.close();
        http.end();
        return false;
    }

    WiFiClient *stream = http.getStreamPtr();
    uint8_t buffer[128];
    int bytesRead = 0;
    while (http.connected() && (bytesRead < contentLength || contentLength < 0))
    {
        size_t available = stream->available();
        if (available)
        {
            size_t readBytes = stream->readBytes(buffer, std::min(available, sizeof(buffer)));
            file.write(buffer, readBytes);
            bytesRead += readBytes;
        }
        delay(10); // Avoid overwhelming the loop
    }

    file.close();
    http.end();

    if (bytesRead == contentLength || contentLength < 0)
    {
        return true;
    }
    else
    {
        debugE("Download incomplete. Expected: %d, Received: %d", contentLength, bytesRead);
        return false;
    }
}

String getFileNameFromUrl(const String &url)
{
    int lastSlashIndex = url.lastIndexOf('/');
    if (lastSlashIndex == -1 || lastSlashIndex == url.length() - 1)
    {
        debugE("Invalid URL format for extracting filename: %s", url.c_str());
        return "";
    }

    return "/" + url.substring(lastSlashIndex + 1);
}

void DownloadHandler::registerCommands()
{
    CommandHandler::registerCommand("download", [](const String &args)
                                    {
        if (args.isEmpty())
        {
            debugW("Usage: download <url> [destination]");
            return;
        }

        int spaceIndex = args.indexOf(' ');
        String url, destination;

        if (spaceIndex < 0)
        {
            // Only URL provided; derive destination from URL
            url = args;
            destination = getFileNameFromUrl(url);
            if (destination.isEmpty())
            {
                debugE("Failed to derive destination filename from URL");
                return;
            }
        }
        else
        {
            url = args.substring(0, spaceIndex);
            destination = args.substring(spaceIndex + 1);
            destination.trim(); // Trim whitespace from the destination string
        }

        if (url.isEmpty())
        {
            debugW("Invalid arguments. URL is empty.");
            debugI("Usage: download <url> [destination]");
            return;
        }

        debugI("Attempting to download from URL: %s to destination: %s", url.c_str(), destination.c_str());

        if (downloadFile(url, destination))
        {
            debugI("File downloaded successfully to: %s", destination.c_str());
        }
        else
        {
            debugE("Failed to download file from: %s", url.c_str());
        } },
        "Downloads a file from a URL and saves it to LittleFS. Usage: download <url> [destination]");

        CommandHandler::registerCommandAlias("wget", "download");
}

#endif // ENABLE_DOWNLOAD_HANDLER

// download https://raw.githubusercontent.com/mdelgert/ImprovWiFiWeb/refs/heads/main/scripts/hello1.scr /hello1.scr
// download https://raw.githubusercontent.com/mdelgert/ImprovWiFiWeb/refs/heads/main/scripts/hello1.scr /test1.scr
// download https://raw.githubusercontent.com/mdelgert/ImprovWiFiWeb/refs/heads/main/scripts/hello1.scr
// wget https://raw.githubusercontent.com/mdelgert/ImprovWiFiWeb/refs/heads/main/scripts/hello1.scr
// wget https://raw.githubusercontent.com/mdelgert/ImprovWiFiWeb/refs/heads/main/scripts/cron1.scr