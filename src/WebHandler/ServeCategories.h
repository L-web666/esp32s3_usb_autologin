#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeCategories
{
public:
    static void registerEndpoints(AsyncWebServer &server);

private:
    static void handleGetCategories(AsyncWebServerRequest *request);
    static void handlePostCategories(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
    static void handleDeleteCategory(AsyncWebServerRequest *request);
};

#endif // ENABLE_WEB_HANDLER
