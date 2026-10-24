#pragma once

#include <Arduino.h>

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define ENABLE_SERVE_ACTIONS

class WebHandler
{
public:
    static void printRequestBody(AsyncWebServerRequest* request, uint8_t* data, size_t len);
    static void sendErrorResponse(AsyncWebServerRequest* request, int statusCode, const char* message, bool checkToken = true);
    static void sendSuccessResponse(AsyncWebServerRequest* request, const char* message, JsonDocument* data = nullptr, bool checkToken = true);
    static bool isTokenValid(AsyncWebServerRequest* request);
    static void addCorsHeaders(AsyncWebServerResponse* response);
    static void init();
    static void loop();
    
private:
    //static NonBlockingTimer myTimer;
    static AsyncWebServer server;
    static void serveNotFound();
};

#else  // If ENABLE_WEB_HANDLER is NOT defined, use a no-op class

// Forward-declare only what's needed for no-op methods:
class AsyncWebServerRequest;
class AsyncWebServerResponse;

class WebHandler
{
public:
    static void printRequestBody(AsyncWebServerRequest*, uint8_t*, size_t) {}
    static void sendErrorResponse(AsyncWebServerRequest*, int, const char*, bool) {}

    // Use 'void*' or some other type that won't collide with ArduinoJson.
    // We do not need JsonDocument in the no-op scenario.
    static void sendSuccessResponse(AsyncWebServerRequest*, const char*, void*, bool) {}
    
    static bool isTokenValid(AsyncWebServerRequest*) { return false; }
    static void addCorsHeaders(AsyncWebServerResponse*) {}
    static void init() {}
    static void loop() {}
};

#endif // ENABLE_WEB_HANDLER