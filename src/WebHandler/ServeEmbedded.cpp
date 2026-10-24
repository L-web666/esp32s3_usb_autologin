#ifdef ENABLE_WEB_HANDLER

#include "ServeEmbedded.h"
#include "Globals.h"
#include "WebHandler.h"
#include "ServeEmbeddedFiles.h"
#include <ArduinoJson.h>

void ServeEmbedded::registerEndpoints(AsyncWebServer &server)
{
    handleGetRequest(server);
}

void ServeEmbedded::serveEmbeddedFile(AsyncWebServer &server, const char *path, const uint8_t *start, const uint8_t *end, const char *contentType)
{
    server.on(path, HTTP_GET, [path, start, end, contentType](AsyncWebServerRequest *request){
        size_t fileSize = end - start;
        AsyncWebServerResponse* response = request->beginResponse_P(200, contentType, start, fileSize);
        response->addHeader("Content-Encoding", "identity");
        WebHandler::addCorsHeaders(response);
        request->send(response);
        debugV("Served: %s", path); });
}

void ServeEmbedded::handleGetRequest(AsyncWebServer &server)
{
    /*
    serveEmbeddedFile(server, "/backup.html", backup_html_start, backup_html_end, "text/html");
    serveEmbeddedFile(server, "/buttoneditor.html", buttoneditor_html_start, buttoneditor_html_end, "text/html");
    serveEmbeddedFile(server, "/buttoneditor.css", buttoneditor_css_start, buttoneditor_css_end, "text/css");
    serveEmbeddedFile(server, "/buttoneditor.js", buttoneditor_js_start, buttoneditor_js_end, "application/javascript");
    serveEmbeddedFile(server, "/buttons.html", buttons_html_start, buttons_html_end, "text/html");
    serveEmbeddedFile(server, "/buttons.js", buttons_js_start, buttons_js_end, "application/javascript");
    serveEmbeddedFile(server, "/error.html", error_html_start, error_html_end, "text/html");
    serveEmbeddedFile(server, "/favicon.ico", favicon_ico_start, favicon_ico_end, "image/x-icon");
    serveEmbeddedFile(server, "/files.css", files_css_start, files_css_end, "text/css");
    serveEmbeddedFile(server, "/files.html", files_html_start, files_html_end, "text/html");
    serveEmbeddedFile(server, "/files.js", files_js_start, files_js_end, "application/javascript");
    serveEmbeddedFile(server, "/footer.html", footer_html_start, footer_html_end, "text/html");
    serveEmbeddedFile(server, "/global.js", global_js_start, global_js_end, "application/javascript");
    serveEmbeddedFile(server, "/", index_html_start, index_html_end, "text/html");
    serveEmbeddedFile(server, "/index.js", index_js_start, index_js_end, "application/javascript");
    serveEmbeddedFile(server, "/navbar.html", navbar_html_start, navbar_html_end, "text/html");
    serveEmbeddedFile(server, "/settings.html", settings_html_start, settings_html_end, "text/html");
    serveEmbeddedFile(server, "/settings.js", settings_js_start, settings_js_end, "application/javascript");
    serveEmbeddedFile(server, "/styles.css", styles_css_start, styles_css_end, "text/css");
    serveEmbeddedFile(server, "/terminal.css", terminal_css_start, terminal_css_end, "text/css");
    serveEmbeddedFile(server, "/terminal.html", terminal_html_start, terminal_html_end, "text/html");
    serveEmbeddedFile(server, "/terminal.js", terminal_js_start, terminal_js_end, "application/javascript");
    serveEmbeddedFile(server, "/timezones.json", timezones_json_start, timezones_json_end, "application/json");
    */
         
    debugI("ServeEmbedded handleGetRequest registered");
}

#endif // ENABLE_WEB_HANDLER