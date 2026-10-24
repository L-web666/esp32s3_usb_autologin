#ifdef ENABLE_IMPROV_WIFI_HANDLER

#include "ImprovWiFiHandler.h"
#include "Globals.h"
#include "GfxHandler.h"
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

static IPAddress apIP(192, 168, 4, 1);
static IPAddress netMsk(255, 255, 255, 0);

ImprovWiFi ImprovWiFiHandler::improvSerial(&Serial);

void ImprovWiFiHandler::loop()
{
    improvSerial.handleSerial();
}

void ImprovWiFiHandler::init()
{
    scanAndSaveNetworks();
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    improvSerial.setDeviceInfo(ImprovTypes::
                                   ChipFamily::CF_ESP32_S3,
                               "ImprovWiFi", SOFTWARE_VERSION,
                               "Demo", "http://{LOCAL_IPV4}");

    improvSerial.onImprovError(onImprovWiFiErrorCb);
    improvSerial.onImprovConnected(onImprovWiFiConnectedCb);
        
    if (improvSerial.tryConnectToWifi(settings.wifi.ssid.c_str(), settings.wifi.password.c_str()))
    {
        debugI("Connected to WiFi using saved credentials");
        String ipAddress = WiFi.localIP().toString();
        debugI("IP: %s", ipAddress.c_str());
        GfxHandler::printMessage("IP: " + ipAddress);
        RemoteDebugHandler::startNetwork();
        MDNS.begin(settings.device.name);
        MDNS.addService("_http", "_tcp", 80);
        debugI("Wifi connected MDNS responder started!");
    }
    else
    {
        WiFi.mode(WIFI_AP);
        if (WiFi.softAP(settings.device.name))
        {
            WiFi.softAPConfig(apIP, apIP, netMsk);
            debugI("Access Point started. SSID: %s", settings.device.name.c_str());
            GfxHandler::printMessage("Access Point started. SSID: " + settings.device.name);
            debugI("AP IP: %s", apIP.toString().c_str());
            GfxHandler::printMessage("AP IP: " + apIP.toString());
        }
        else
        {
            debugE("Failed to start Access Point!");
            GfxHandler::printMessage("Failed to start AP!");
        }
    }

    debugI("ImprovWiFiHandler initialized");
}

void ImprovWiFiHandler::onImprovWiFiErrorCb(ImprovTypes::Error err)
{
    debugE("ImprovWiFiHandler error occurred: %d", err);
}

void ImprovWiFiHandler::onImprovWiFiConnectedCb(const char *ssid, const char *password)
{
    settings.wifi.ssid = String(ssid);
    settings.wifi.password = String(password);
    ConfigManager::save();
    debugD("WiFi credentials saved to preferences. SSID: %s", ssid);
    GfxHandler::printMessage("Rebooting...");
    debugI("Rebooting...");
    delay(1000);
    ESP.restart();
}

void ImprovWiFiHandler::scanAndSaveNetworks()
{
    if (settings.wifi.scan == false)
    {
        debugI("Wi-Fi scan disabled");
        return;
    }

    if (!LittleFS.begin(true))
    {
        debugE("Failed to mount LittleFS");
        return;
    }
    debugI("LittleFS mounted successfully");

    int networkCount = WiFi.scanNetworks(false); // Blocking call
    debugI("Scanning for Wi-Fi networks...");

    if (networkCount < 0)
    {
        debugE("Wi-Fi scan failed with code %d", networkCount);
        return;
    }

    // Create a JSON document to store the network results
    JsonDocument doc; // Use JsonDocument for dynamic sizing
    JsonArray networks = doc["networks"].to<JsonArray>();

    for (int i = 0; i < networkCount; i++)
    {
        JsonObject network = networks.add<JsonObject>();
        network["ssid"] = WiFi.SSID(i);
        network["rssi"] = WiFi.RSSI(i);
        network["encryptionType"] = WiFi.encryptionType(i);
        network["isHidden"] = WiFi.SSID(i).isEmpty(); // Example logic for hidden networks
    }

    // Save the JSON document to LittleFS
    File file = LittleFS.open(WIFI_NETWORKS_FILE, "w");
    if (!file)
    {
        debugE("Failed to open file %s for writing", WIFI_NETWORKS_FILE);
        return;
    }

    if (serializeJson(doc, file) == 0)
    {
        debugE("Failed to write JSON to file %s", WIFI_NETWORKS_FILE);
    }
    else
    {
        debugI("Network scan results saved to %s", WIFI_NETWORKS_FILE);
    }

    file.close();

    // Free memory used by the scan
    WiFi.scanDelete();
}

#endif // ENABLE_IMPROV_WIFI_HANDLER
