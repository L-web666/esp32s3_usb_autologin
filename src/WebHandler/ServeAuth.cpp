#ifdef ENABLE_WEB_HANDLER

#include "ServeAuth.h"
#include "Globals.h"
#include "WebHandler.h"
#include <vector>
#include <LittleFS.h>

// Store active session tokens in memory
std::vector<String> activeSessions;

// Generate a secure random session token
String generateRandomToken()
{
    String token = "";
    for (int i = 0; i < 32; i++)
    { // 32-character token
        char c = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[random(62)];
        token += c;
    }
    return token;
}

String getContentType(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".svg")) return "image/svg+xml";
    return "text/plain";
}

void ServeAuth::registerEndpoints(AsyncWebServer &server)
{
    handleLoginRequest(server);
    handleLogoutRequest(server);
    handleSecureRequest(server);
}

void ServeAuth::handleLoginRequest(AsyncWebServer &server)
{
    server.on("/auth/login", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
        debugV("Received POST request on /auth/login");

        WebHandler::printRequestBody(request, data, len);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, data, len);

        if (error) {
            debugE("JSON deserialization failed: %s", error.c_str());
            request->send(400, "application/json", R"({"status": "error", "message": "Invalid JSON payload"})");
            return;
        }

        const char* username = doc["username"] | "";
        const char* password = doc["password"] | "";
        
        debugV("Attempting login: username=%s", username);
        
        //if (strcmp(username, "admin") == 0 && strcmp(password, "pass") == 0) {
        if (strcmp(username, settings.device.userName.c_str()) == 0 && strcmp(password, settings.device.userPassword.c_str()) == 0) {
            String sessionToken = generateRandomToken();
            activeSessions.push_back(sessionToken);  // Store valid session

            JsonDocument response;
            response["status"] = "success";
            response["message"] = "Login successful";
            response["data"]["token"] = sessionToken;

            String jsonResponse;
            serializeJson(response, jsonResponse);

            AsyncWebServerResponse *res = request->beginResponse(200, "application/json", jsonResponse);
            res->addHeader("Set-Cookie", "session=" + sessionToken + "; Path=/; HttpOnly;");
            //res->addHeader("Set-Cookie", "userName=" + settings.device.userName + "; Path=/; HttpOnly;");
            WebHandler::addCorsHeaders(res);
            request->send(res);

            debugV("Generated session token: %s", sessionToken.c_str());
        } else {
            request->send(401, "application/json", R"({"status": "error", "message": "Invalid credentials"})");
        } });
}

void ServeAuth::handleLogoutRequest(AsyncWebServer &server)
{
    server.on("/auth/logout", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (!request->hasHeader("Cookie")) {
            request->send(400, "application/json", R"({"status": "error", "message": "No session found"})");
            return;
        }

        AsyncWebHeader* cookieHeader = request->getHeader("Cookie");
        String cookie = cookieHeader->value();

        if (!cookie.startsWith("session=")) {
            request->send(400, "application/json", R"({"status": "error", "message": "Invalid session"})");
            return;
        }

        String sessionToken = cookie.substring(8);

        // Remove session from activeSessions
        activeSessions.erase(remove(activeSessions.begin(), activeSessions.end(), sessionToken), activeSessions.end());

        // Create response and set an expired cookie
        AsyncWebServerResponse *res = request->beginResponse(200, "application/json", R"({"status": "success", "message": "Logged out"})");
        res->addHeader("Set-Cookie", "session=; Path=/; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT;");
        WebHandler::addCorsHeaders(res);
        request->send(res);
    });
}

void ServeAuth::handleSecureRequest(AsyncWebServer &server)
{
    // Use a wildcard to catch all requests under /secure
    server.on("/secure/*", HTTP_ANY, [](AsyncWebServerRequest *request) {
        String url = request->url();

        debugI("Received request: %s", url.c_str());
        
        // Check for authentication
        if (!request->hasHeader("Cookie")) {
            debugI("No cookie header found, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }

        AsyncWebHeader* cookieHeader = request->getHeader("Cookie");
        String cookie = cookieHeader->value();

        if (!cookie.startsWith("session=")) {
            debugI("No session found in cookie, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }
        
        String sessionToken = cookie.substring(8);  // Extract session token

        // Check if sessionToken exists in activeSessions
        bool validSession = false;
        for (const String &token : activeSessions) {
            if (sessionToken == token) {
                validSession = true;
                break;
            }
        }

        if (!validSession) {
            debugI("Invalid session token, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }

        // Serve the requested file if it exists under /secure
        if (LittleFS.exists(url)) {
            debugI("Serving file: %s", url.c_str());
            request->send(LittleFS, url, getContentType(url));
        } else {
            request->send(404, "text/plain", "File Not Found");
        }
    });
}

#endif // ENABLE_WEB_HANDLER

/*
void ServeAuth::handleSecureRequest(AsyncWebServer &server)
{
    server.on("/secure/secure.html", HTTP_ANY, [](AsyncWebServerRequest *request) {
        String url = request->url();

        debugI("Received request: %s", url.c_str());
        
        // Check for authentication
        if (!request->hasHeader("Cookie")) {
            debugI("No cookie header found, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }

        AsyncWebHeader* cookieHeader = request->getHeader("Cookie");
        String cookie = cookieHeader->value();

        if (!cookie.startsWith("session=")) {
            debugI("No session found in cookie, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }

        String sessionToken = cookie.substring(8);  // Extract session token

        // Check if sessionToken exists in activeSessions
        bool validSession = false;
        for (const String &token : activeSessions) {
            if (sessionToken == token) {
                validSession = true;
                break;
            }
        }

        if (!validSession) {
            debugI("Invalid session token, redirecting to /login.html");
            request->redirect("/login.html");
            return;
        }

        // Serve file if user is authenticated
        if (LittleFS.exists(url)) {
            debugI("Serving file: %s", url.c_str());
            //request->send(LittleFS, url, getContentType(url));
            request->send(LittleFS, "/secure/secure.html", "text/html");
        } else {
            request->send(404, "text/plain", "File Not Found");
        }
    });
}

void ServeAuth::handleLogoutRequest(AsyncWebServer &server)
{
    server.on("/auth/logout", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (!request->hasHeader("Cookie")) {
            request->send(400, "application/json", R"({"status": "error", "message": "No session found"})");
            return;
        }

        AsyncWebHeader* cookieHeader = request->getHeader("Cookie");
        String cookie = cookieHeader->value();

        if (!cookie.startsWith("session=")) {
            request->send(400, "application/json", R"({"status": "error", "message": "Invalid session"})");
            return;
        }

        String sessionToken = cookie.substring(8);

        // Remove session from activeSessions
        activeSessions.erase(remove(activeSessions.begin(), activeSessions.end(), sessionToken), activeSessions.end());

        request->send(200, "application/json", R"({"status": "success", "message": "Logged out"})"); });
}
*/