#ifdef ENABLE_WEB_HANDLER

#include "ServeCategories.h"
#include "Globals.h"
#include "WebHandler.h"
#include <ArduinoJson.h>
#include <LittleFS.h>

void ServeCategories::registerEndpoints(AsyncWebServer &server)
{
    server.on("/categories", HTTP_GET, handleGetCategories);
    server.on("/categories", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handlePostCategories);
    server.on("/categories", HTTP_DELETE, handleDeleteCategory);
}

void ServeCategories::handleGetCategories(AsyncWebServerRequest *request)
{
    // Open the file for reading
    File file = LittleFS.open(CATEGORIES_FILE, "r");
    if (!file || file.size() == 0) {
        WebHandler::sendErrorResponse(request, 400, "Failed to read categories.json or file is empty");
        return;
    }

    String json = file.readString(); // Read the file content into a string
    file.close(); // Close the file

    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", json);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeCategories::handlePostCategories(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    debugV("Received POST request on /categories");

    // Accumulate incoming data
    static String requestBody;
    if (index == 0) {
        requestBody = ""; // Start a new request body
    }
    requestBody += String((char *)data).substring(0, len); // Append the current chunk

    if (index + len != total) {
        return; // Wait until all chunks are received
    }

    debugV("Complete request body received: %s", requestBody.c_str());

    // Parse the incoming JSON payload
    JsonDocument incomingDoc;
    DeserializationError error = deserializeJson(incomingDoc, requestBody);

    if (error) {
        debugE("JSON deserialization failed: %s", error.c_str());
        WebHandler::sendErrorResponse(request, 400, "Invalid JSON payload");
        return;
    }

    // Read the existing JSON file
    File file = LittleFS.open(CATEGORIES_FILE, "r");
    JsonDocument existingDoc;
    
    if (file) {
        deserializeJson(existingDoc, file); // Deserialize existing data
        file.close();
    } else {
        debugW("No existing file found; initializing new document");
        existingDoc["categories"] = JsonArray(); // Initialize an empty "categories" array
    }

    JsonArray existingCategories = existingDoc["categories"];
    JsonArray incomingCategories = incomingDoc["categories"];

    // Update existing categories or add new ones
    for (JsonObject newCategory : incomingCategories) {
        bool found = false;

        for (JsonObject existingCategory : existingCategories) {
            if (existingCategory["id"] == newCategory["id"]) {
                // Update only the provided fields for the matching category
                for (JsonPair kv : newCategory) {
                    existingCategory[kv.key()] = kv.value();
                }
                found = true;
                break;
            }
        }

        /*
        if (!found) {
            // Generate a new ID
            int maxId = 0;
            for (JsonObject existingCategory : existingCategories) {
                int existingId = existingCategory["id"] | 0;
                if (existingId > maxId) {
                    maxId = existingId;
                }
            }
        
            int newId = maxId + 1;
        
            // Assign a new ID if it's missing or 0
            JsonObject categoryToAdd = newCategory;
            if (!categoryToAdd.containsKey("id") || categoryToAdd["id"] == 0) {
                categoryToAdd["id"] = newId;
            }
        
            existingCategories.add(categoryToAdd);
        }
        */

        if (!found) {
            // Generate a new ID
            int maxId = 0;
            for (JsonObject existingCategory : existingCategories) {
                int existingId = existingCategory["id"] | 0;
                if (existingId > maxId) {
                    maxId = existingId;
                }
            }
            
            int newId = maxId + 1;
            
            // Assign a new ID if it's missing or 0
            JsonObject categoryToAdd = newCategory;
            if (categoryToAdd["id"].isNull() || categoryToAdd["id"] == 0) {
                categoryToAdd["id"] = newId;
            }
            
            existingCategories.add(categoryToAdd);
        }

    }

    // Write the updated JSON back to the file
    file = LittleFS.open(CATEGORIES_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", CATEGORIES_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write categories.json");
        return;
    }

    //serializeJson(existingDoc, file); // Save updated JSON
    serializeJsonPretty(existingDoc, file); // ✅ Nicely formatted JSON

    file.close();

    //WebHandler::sendSuccessResponse(request, "Categories updated successfully");
    String responseBody;
    
    //serializeJson(existingDoc, responseBody);
    serializeJsonPretty(existingDoc, responseBody);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", responseBody);
    WebHandler::addCorsHeaders(response);
    request->send(response);
}

void ServeCategories::handleDeleteCategory(AsyncWebServerRequest *request)
{
    // Check if the "id" parameter is provided
    if (!request->hasParam("id")) {
        WebHandler::sendErrorResponse(request, 400, "Missing category ID");
        return;
    }

    String categoryId = request->getParam("id")->value(); // Use String instead of int
    debugV("Attempting to delete category with ID: %s", categoryId.c_str());

    // Open the JSON file for reading
    File file = LittleFS.open(CATEGORIES_FILE, "r");
    if (!file) {
        debugE("Failed to open file for reading: %s", CATEGORIES_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to read categories.json");
        return;
    }

    // Parse the JSON content
    JsonDocument doc; // Adjust size as needed
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        debugE("Failed to parse JSON file: %s", error.c_str());
        WebHandler::sendErrorResponse(request, 500, "Invalid JSON format categories.json");
        return;
    }

    JsonArray categories = doc["categories"];
    bool categoryFound = false;

    // Iterate over the categories array and remove the one with the matching ID
    for (size_t i = 0; i < categories.size(); i++) {
        if (String(categories[i]["id"].as<String>()) == categoryId) { // Compare as strings
            categories.remove(i);
            categoryFound = true;
            break;
        }
    }

    if (!categoryFound) {
        debugW("Category with ID %s not found", categoryId.c_str());
        WebHandler::sendErrorResponse(request, 404, "Category ID not found");
        return;
    }

    // Write the updated JSON back to the file
    file = LittleFS.open(CATEGORIES_FILE, "w");
    if (!file) {
        debugE("Failed to open file for writing: %s", CATEGORIES_FILE);
        WebHandler::sendErrorResponse(request, 500, "Failed to write categories.json");
        return;
    }

    //serializeJson(doc, file);
    serializeJsonPretty(doc, file);

    file.close();

    debugV("Category with ID %s deleted successfully", categoryId.c_str());
    WebHandler::sendSuccessResponse(request, "Category deleted successfully");
}

#endif // ENABLE_WEB_HANDLER