#pragma once

#ifdef ENABLE_WEB_HANDLER

#include <ESPAsyncWebServer.h>

class ServeDevice {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void handleDeviceInfo(AsyncWebServer& server);
    static void handleDeviceReboot(AsyncWebServer& server);
    static void handleDeviceWifiNetworks(AsyncWebServer& server);
    static void handleDeviceTimezones(AsyncWebServer& server);
    //static void handleDeviceBackup(AsyncWebServer& server);
    static void handleDeviceFormat(AsyncWebServer& server);
    static void handleDeviceOTA(AsyncWebServer &server);
};

#endif // ENABLE_WEB_HANDLER