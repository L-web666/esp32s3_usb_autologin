#ifdef ENABLE_WEB_HANDLER

#include <LittleFS.h>
#include <ArduinoJson.h>
#include "ServeSettings.h"
#include "Globals.h"
#include "WebHandler.h"

void ServeSettings::registerEndpoints(AsyncWebServer &server)
{
    server.on("/settings/get", HTTP_GET, handleGetSettings);
    server.on("/settings/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleSetSettings);
}

void ServeSettings::handleGetSettings(AsyncWebServerRequest *request)
{
    // Open the file for reading
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (!file || file.size() == 0) {
        WebHandler::sendErrorResponse(request, 400, "Failed to read settings.json or file is empty");
        return;
    }

    String json = file.readString(); // Read the file content into a string
    file.close(); // Close the file

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", json);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse incoming JSON
    JsonDocument newDoc;
    DeserializationError error = deserializeJson(newDoc, requestBody);
    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Read existing settings
    JsonDocument existingDoc;
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (file) {
        error = deserializeJson(existingDoc, file);
        file.close();
        if (error) {
            WebHandler::sendErrorResponse(request, 400, "Failed to parse existing settings");
            return;
        }
    } else {
        // If file doesn't exist, we'll start with an empty document
        debugV("No existing settings file found, creating new one");
    }

    // Merge new settings into existing settings
    // Update top-level objects (device, wifi, mqtt, security)
    for (JsonPair kv : newDoc.as<JsonObject>()) {
        const char* section = kv.key().c_str();
        JsonObject newSection = kv.value().as<JsonObject>();
        JsonObject existingSection = existingDoc[section] | JsonObject();

        // Update fields within each section
        for (JsonPair field : newSection) {
            existingSection[field.key()] = field.value();
        }
    }

    // Save merged settings
    file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(existingDoc, file);
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

/*
//handleGetSettingsArchieve(server);
//handleSetSettingsArchieve(server);

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);

    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Save it formatted (pretty)
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(doc, file); // ✅ Pretty format!
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

// This should be in a separate utility file for better organization repeating the same pattern in save buttons
static void mergeJson(JsonObject dst, JsonObjectConst src) {
    for (JsonPairConst kv : src) {
      const char* key = kv.key().c_str();
  
      if (kv.value().is<JsonObject>() && dst[key].is<JsonObject>()) {
        // Recursively merge sub-objects
        mergeJson(dst[key].as<JsonObject>(), kv.value().as<JsonObjectConst>());
      } else {
        // Overwrite primitive or add new field
        dst[key] = kv.value();
      }
    }
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start new
    }

    requestBody += String((char*)data).substring(0, len);

    if (index + len != total) return; // Wait until full body is received

    JsonDocument incomingDoc;
    DeserializationError err = deserializeJson(incomingDoc, requestBody);
    if (err) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format");
        return;
    }

    // Load existing settings if available
    JsonDocument existingDoc;
    File file = LittleFS.open(SETTINGS_FILE, "r");
    if (file) {
        deserializeJson(existingDoc, file);
        file.close();
    }

    // Merge incoming into existing
    //mergeJson(existingDoc, incomingDoc);
    mergeJson(existingDoc.as<JsonObject>(), incomingDoc.as<JsonObjectConst>());

    // Save back as pretty JSON
    file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 500, "Failed to open settings file for writing");
        return;
    }

    serializeJsonPretty(existingDoc, file);
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start fresh
    }
    requestBody += String((char*)data).substring(0, len);

    // Wait until the full body is received
    if (index + len != total) {
        return;
    }

    // Parse JSON
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, requestBody);

    if (error) {
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON format in request");
        return;
    }

    // Save it formatted (pretty)
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    serializeJsonPretty(doc, file); // ✅ Pretty format!
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

void ServeSettings::handleSetSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /settings/set");

    // Open the file for writing
    File file = LittleFS.open(SETTINGS_FILE, "w");
    if (!file) {
        WebHandler::sendErrorResponse(request, 400, "Failed to open settings.json for writing");
        return;
    }

    // Write the data to the file
    if (file.write(data, len) != len) {
        WebHandler::sendErrorResponse(request, 400, "Failed to write data to settings.json");
        return;
    }

    // Close the file
    file.close();

    WebHandler::sendSuccessResponse(request, "Settings updated successfully");
}

void ServeSettings::handleGetSettingsArchieve(AsyncWebServer &server)
{
    server.on("/settings/archieve/get", HTTP_GET, [](AsyncWebServerRequest *request){
        debugV("Received GET request on /settings/archieve/get");

        JsonDocument doc;

        // Device
        doc["device_name"]       = settings.device.name;
        doc["time_zone"]         = settings.device.timezone;
        doc["setup_mode"]        = settings.device.setupMode;
        doc["default_timeout"]   = settings.device.defaultTimeout;
        doc["boot_count"]        = settings.device.bootCount;
        doc["boot_time"]         = settings.device.bootTime;

        // WiFi
        doc["wifi_ssid"]         = settings.wifi.ssid;
        doc["wifi_password"]     = settings.wifi.password;
        doc["wifi_scan"]         = settings.wifi.scan;

        // MQTT
        doc["mqtt_enabled"]      = settings.mqtt.enabled;
        doc["mqtt_server"]       = settings.mqtt.server;
        doc["mqtt_port"]         = settings.mqtt.port;
        doc["mqtt_username"]     = settings.mqtt.username;
        doc["mqtt_password"]     = settings.mqtt.password;
        doc["mqtt_ssl"]          = settings.mqtt.ssl;
        doc["mqtt_topic_sub"]    = settings.mqtt.subTopic;
        doc["mqtt_topic_pub"]    = settings.mqtt.pubTopic;

        // Security
        doc["api_key"]           = settings.security.apiKey;
        doc["api_token"]         = settings.security.apiToken;
        doc["ota_password"]      = settings.security.otaPassword;

        // Features
        doc["cors"]              = settings.features.cors;
        doc["web_handler"]       = settings.features.webHandler;

        WebHandler::sendSuccessResponse(request, "Settings retrieved successfully", &doc);
    });
}

void ServeSettings::handleSetSettingsArchieve(AsyncWebServer &server)
{
    server.on("/settings/archieve/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
        [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
            debugV("Received POST request on /settings/archieve/set");

            WebHandler::printRequestBody(request, data, len);

            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);

            if (error) {
                debugE("JSON deserialization failed: %s", error.c_str());
                WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
                return;
            }

            // Device
            settings.device.name         = doc["device_name"] | settings.device.name;
            settings.device.timezone     = doc["time_zone"] | settings.device.timezone;
            settings.device.setupMode    = doc["setup_mode"] | settings.device.setupMode;
            settings.device.defaultTimeout = doc["default_timeout"] | settings.device.defaultTimeout;
            settings.device.bootCount    = doc["boot_count"] | settings.device.bootCount;
            settings.device.bootTime     = doc["boot_time"] | settings.device.bootTime;

            // WiFi
            settings.wifi.ssid           = doc["wifi_ssid"] | settings.wifi.ssid;
            settings.wifi.password       = doc["wifi_password"] | settings.wifi.password;
            settings.wifi.scan           = doc["wifi_scan"] | settings.wifi.scan;

            // MQTT
            settings.mqtt.enabled        = doc["mqtt_enabled"] | settings.mqtt.enabled;
            settings.mqtt.server         = doc["mqtt_server"] | settings.mqtt.server;
            settings.mqtt.port           = doc["mqtt_port"] | settings.mqtt.port;
            settings.mqtt.ssl            = doc["mqtt_ssl"] | settings.mqtt.ssl;
            settings.mqtt.username       = doc["mqtt_username"] | settings.mqtt.username;
            settings.mqtt.password       = doc["mqtt_password"] | settings.mqtt.password;
            settings.mqtt.subTopic       = doc["mqtt_topic_sub"] | settings.mqtt.subTopic;
            settings.mqtt.pubTopic       = doc["mqtt_topic_pub"] | settings.mqtt.pubTopic;

            // Security
            settings.security.apiKey     = doc["api_key"] | settings.security.apiKey;
            settings.security.apiToken   = doc["api_token"] | settings.security.apiToken;
            settings.security.otaPassword = doc["ota_password"] | settings.security.otaPassword;

            // Features
            settings.features.cors       = doc["cors"] | settings.features.cors;
            settings.features.webHandler = doc["web_handler"] | settings.features.webHandler;

            // Save updated settings
            ConfigManager::save();

            WebHandler::sendSuccessResponse(request, "Settings updated successfully");

            // Optional reboot if needed
            // if (delayTimer.isReady()) {
            //     ESP.restart();
            // }
        }
    );
}
*/

#endif // ENABLE_WEB_HANDLER
