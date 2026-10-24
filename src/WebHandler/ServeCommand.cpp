#ifdef ENABLE_WEB_HANDLER

#include "ServeCommand.h"
#include "Globals.h"
#include "WebHandler.h"

void ServeCommand::registerEndpoints(AsyncWebServer &server)
{
    handleCommandRequest(server);
}

void ServeCommand::handleCommandRequest(AsyncWebServer &server)
{
    server.on("/command/set", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
              {
        debugV("Received POST request on /command/set");

        // Convert the raw data to a string
        String command = String((const char*)data).substring(0, len);
        command.trim(); // Remove any leading/trailing whitespace or newlines

        if (command.isEmpty()) {
            debugW("Received an empty command");
            WebHandler::sendErrorResponse(request, 400, "Empty command received");
            return;
        }

        debugV("Executing command: %s", command.c_str());

        // Send the command to the CommandHandler
        CommandHandler::handleCommand(command);

        // Respond with success
        WebHandler::sendSuccessResponse(request, "Command executed successfully"); });
}

#endif // ENABLE_WEB_HANDLER
