#ifdef ENABLE_WEB_HANDLER

#include "WebHandler.h"
#include "Globals.h"
#include "ServeDevice.h"
#include "ServeSettings.h"
#include "ServeFiles.h"
#include "ServeCommand.h"
#include "ServeButtons.h"
#include "ServeAuth.h"
#include "ServeCategories.h"
//#include "ServeEmbedded.h"
#include <LittleFS.h>

//NonBlockingTimer WebHandler::myTimer(60000);
AsyncWebServer WebHandler::server(80);

void WebHandler::serveNotFound()
{
    server.onNotFound([](AsyncWebServerRequest *request){
        if (request->method() == HTTP_OPTIONS) { // Handle preflight CORS request
            AsyncWebServerResponse *response = request->beginResponse(204); // No Content
            WebHandler::addCorsHeaders(response); // Add generic CORS headers
            request->send(response);
            debugV("Handled CORS preflight request");
        } else { // Handle other unmatched routes
            //request->send(404, "text/plain", "Not found");
            request->redirect("/error.html");
            debugV("Route not found: %s", request->url().c_str());
        } });
}

void WebHandler::printRequestBody(AsyncWebServerRequest *request, uint8_t *data, size_t len)
{
    // Print as string (existing code)
    String requestBody = "";
    for (size_t i = 0; i < len; i++) {
        requestBody += (char)data[i];
    }
    
    debugV("Request Body (string): %s", requestBody.c_str());
}

bool WebHandler::isTokenValid(AsyncWebServerRequest *request)
{
    if (!settings.security.apiToken) return true;
    if (request->hasHeader("Authorization"))
    {
        AsyncWebHeader *header = request->getHeader("Authorization");
        if (String(header->value()) == String("Bearer ") + settings.security.apiKey)
        {
            return true;
        }
    }
    debugE("Invalid token");
    return false;
}

void WebHandler::addCorsHeaders(AsyncWebServerResponse *response)
{
    if (settings.features.cors) return;
    response->addHeader("Access-Control-Allow-Origin", "*"); // Allow any origin
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, PATCH, OPTIONS"); // Allow all standard HTTP methods
    response->addHeader("Access-Control-Allow-Headers", "*"); // Allow any headers the client may send
    response->addHeader("Access-Control-Allow-Credentials", "true"); // Allow credentials (optional, in case cookies or Authorization are needed)
    response->addHeader("Access-Control-Max-Age", "86400"); // Set max age for preflight requests (optional, but improves efficiency)
}

void WebHandler::sendErrorResponse(AsyncWebServerRequest *request, int statusCode, const char *message, bool checkToken)
{
    if (checkToken && !isTokenValid(request))
    {
        debugE("Token validation failed for error response");
        statusCode = 403; // Override status code for forbidden
        message = "Forbidden: Invalid token";
    }

    JsonDocument doc;
    doc["status"] = "error";
    doc["message"] = message;
    
    String jsonResponse;
    serializeJson(doc, jsonResponse);

    AsyncWebServerResponse *response = request->beginResponse(statusCode, "application/json", jsonResponse);
    addCorsHeaders(response);
    request->send(response);
}

void WebHandler::sendSuccessResponse(AsyncWebServerRequest *request, const char *message, JsonDocument *data, bool checkToken)
{
    if (checkToken && !isTokenValid(request))
    {
        debugE("Token validation failed for success response");
        sendErrorResponse(request, 403, "Forbidden: Invalid token", false);
        return;
    }

    JsonDocument doc;
    doc["status"] = "success";
    doc["message"] = message;

    if (data != nullptr)
    {
        doc["data"] = *data;
    }

    String jsonResponse;
    serializeJson(doc, jsonResponse);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", jsonResponse);
    addCorsHeaders(response);
    request->send(response);
}

void WebHandler::init()
{
    if (!settings.features.webHandler) return;

    ServeDevice::registerEndpoints(server);
    ServeSettings::registerEndpoints(server);
    ServeFiles::registerEndpoints(server);
    ServeCommand::registerEndpoints(server);
    ServeButtons::registerEndpoints(server);
    ServeCategories::registerEndpoints(server);
    ServeAuth::registerEndpoints(server);
    //ServeEmbedded::registerEndpoints(server);
    //server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
    //server.serveStatic("/", LittleFS, "/www").setDefaultFile("index.html");
    //server.serveStatic("/www", LittleFS, "/www").setDefaultFile("index.html");
    //server.serveStatic("/secure/secure.html", LittleFS, "/secure/secure.html").setAuthentication("admin", "pass");
    //server.serveStatic("/", LittleFS, "/www").setDefaultFile("index.html").setAuthentication("admin", "pass");
    server.serveStatic("/", LittleFS, "/www").setDefaultFile("index.html");
    serveNotFound();
    server.begin();
    debugI("WebHandler initialized");
}

void WebHandler::loop()
{
    // debugI("WebHandler loop");
}

#endif // ENABLE_WIFI_HANDLER