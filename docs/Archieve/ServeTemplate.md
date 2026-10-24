Following this template pattern please provide backup and restore code.

#pragma once

#ifdef ENABLE_WEB_HANDLER

#include "Globals.h"
#include "WebHandler.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class ServeTemplate {
public:
    static void registerEndpoints(AsyncWebServer& server);

private:
    static void handleGetRequest(AsyncWebServer& server);
    static void handleSetRequest(AsyncWebServer& server);
};

#endif // ENABLE_WEB_HANDLER

#ifdef ENABLE_WEB_HANDLER

#include "ServeTemplate.h"

void ServeTemplate::registerEndpoints(AsyncWebServer &server)
{
    handleGetRequest(server);
    handleSetRequest(server);
}

void ServeTemplate::handleGetRequest(AsyncWebServer &server)
{
    server.on("/template/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        debugV("Received GET request on /template/get");

        // Create a JSON response
        JsonDocument data;
        data["data"] = "This is a JSON response";

        WebHandler::sendSuccessResponse(request, "GET response template", &data); });
}

void ServeTemplate::handleSetRequest(AsyncWebServer &server)
{
    server.on("/template/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
            debugV("Received POST request on /template/set");

            // Print the request body
            WebHandler::printRequestBody(request, data, len);

            // Parse JSON
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, data, len);

            if (error) {
                debugE("JSON deserialization failed: %s", error.c_str());
                WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
                return;
            }

            // Extract a field for debugging
            const char* exampleValue = doc["example"] | "undefined";
            debugV("Extracted JSON value: example=%s", exampleValue);

            // Respond with success
            WebHandler::sendSuccessResponse(request, "Data received successfully"); });
}

#endif // ENABLE_WEB_HANDLER