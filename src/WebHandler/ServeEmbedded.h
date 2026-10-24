#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeEmbedded {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void serveEmbeddedFile(AsyncWebServer &server, const char *path, const uint8_t *start, const uint8_t *end, const char *contentType);
    static void handleGetRequest(AsyncWebServer& server);
};

#endif // ENABLE_WEB_HANDLER