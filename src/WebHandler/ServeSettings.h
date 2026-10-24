#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeSettings {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void handleGetSettings(AsyncWebServerRequest *request);
    static void handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
};

#endif // ENABLE_WEB_HANDLER