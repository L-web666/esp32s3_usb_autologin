#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class ServeAuth
{
public:
    static void registerEndpoints(AsyncWebServer &server);

private:
    static void handleLoginRequest(AsyncWebServer &server);
    static void handleLogoutRequest(AsyncWebServer &server);
    static void handleSecureRequest(AsyncWebServer &server);
};

#endif // ENABLE_WEB_HANDLER
