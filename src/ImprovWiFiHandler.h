#pragma once

#ifdef ENABLE_IMPROV_WIFI_HANDLER

#include "ImprovWiFiLibrary.h"

class ImprovWiFiHandler
{
private:
    static ImprovWiFi improvSerial;
    static void onImprovWiFiErrorCb(ImprovTypes::Error err);
    static void onImprovWiFiConnectedCb(const char *ssid, const char *password);
    static bool connectWifi(const char *ssid, const char *password);
    static void scanAndSaveNetworks();

public:
    static void loop();
    static void init();
};

#else

class ImprovWiFiHandler {
public: // No-op implementation of ImprovWiFiHandler
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_IMPROV_WIFI_HANDLER
