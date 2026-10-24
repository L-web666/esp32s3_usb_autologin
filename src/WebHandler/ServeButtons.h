#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeButtons
{
public:
    static void registerEndpoints(AsyncWebServer &server);

private:
    static void handleGetButtons(AsyncWebServerRequest *request);
    static void handleDeleteButton(AsyncWebServerRequest *request);
    static void handlePostButtons(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleRunButton(AsyncWebServerRequest *request);
};

#endif // ENABLE_WEB_HANDLER
