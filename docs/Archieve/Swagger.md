For your ESP32 project, since you're using `AsyncWebServer`, you can implement Swagger documentation manually by serving a JSON file that describes your API. Here's how you can do it:

---

### **1. Create the OpenAPI (Swagger) JSON File**
Create a file named `swagger.json` with your API definitions. Place it in the `LittleFS` file system.

Example `swagger.json`:
```json
{
  "openapi": "3.0.0",
  "info": {
    "title": "ESP32 API",
    "version": "1.0.0",
    "description": "API documentation for ESP32 endpoints"
  },
  "servers": [
    { "url": "http://192.168.1.100", "description": "Local ESP32 server" }
  ],
  "paths": {
    "/device/reboot": {
      "post": {
        "summary": "Reboots the ESP32 device",
        "responses": {
          "200": { "description": "Device rebooting..." }
        }
      }
    },
    "/settings/get": {
      "get": {
        "summary": "Retrieve device settings",
        "responses": {
          "200": {
            "description": "Returns the current settings",
            "content": {
              "application/json": {
                "schema": {
                  "type": "object",
                  "properties": {
                    "device": { "type": "object" },
                    "wifi": { "type": "object" }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
```

---

### **2. Serve `swagger.json` via `AsyncWebServer`**
Modify your ESP32 web server to serve the `swagger.json` file.

```cpp
#include "FS.h"
#include "LittleFS.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    
    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }

    // Serve the Swagger JSON
    server.on("/swagger.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/swagger.json", "application/json");
    });

    server.begin();
}

void loop() {
}
```

---

### **3. Use Swagger UI to View API Documentation**
1. Go to [Swagger Editor](https://editor.swagger.io/)
2. Click **File > Import URL**
3. Enter your ESP32's IP address with `/swagger.json`, e.g.:
   ```
   http://192.168.1.100/swagger.json
   ```
4. Now you can view and test your API endpoints.

---

### **4. Optional: Host Swagger UI Locally**
If you want to host the Swagger UI directly on the ESP32:
1. Download the [Swagger UI dist files](https://github.com/swagger-api/swagger-ui).
2. Upload them to `LittleFS`.
3. Modify `index.html` to point to your ESP32's `swagger.json`:
   ```html
   <script>
       window.onload = function() {
           const ui = SwaggerUIBundle({
               url: "/swagger.json",
               dom_id: "#swagger-ui"
           });
       };
   </script>
   ```
4. Serve `index.html` from `LittleFS`:
   ```cpp
   server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
   ```

Now, you can access `http://192.168.1.100/` to view the Swagger UI.

---

### **Final Notes**
- Make sure `swagger.json` is in `LittleFS` (`/data/swagger.json` when using PlatformIO).
- Your ESP32 must be accessible via Wi-Fi for Swagger UI to load it.
- You can modify `swagger.json` to add more endpoints as needed.

Would you like an automated script to generate the `swagger.json` based on your endpoints?

Yes, you can serve a Swagger UI page on your ESP32 while using externally hosted Swagger UI packages (CDN). This reduces storage requirements on your ESP32 by loading the Swagger UI dynamically from an external source.

---

### **Steps to Serve a Swagger UI Page with External CDN**
1. **Modify `index.html` to Use External Swagger UI**
2. **Serve the `index.html` from LittleFS**
3. **Serve `swagger.json` for API Documentation**

---

### **1. Create `index.html` with External Swagger UI**
Save the following as `/index.html` in your ESP32's LittleFS:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 API Documentation</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui.min.css">
</head>
<body>
    <div id="swagger-ui"></div>
    
    <script src="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui-bundle.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui-standalone-preset.min.js"></script>
    
    <script>
        window.onload = function() {
            const ui = SwaggerUIBundle({
                url: "/swagger.json",
                dom_id: "#swagger-ui",
                presets: [
                    SwaggerUIBundle.presets.apis,
                    SwaggerUIStandalonePreset
                ],
                layout: "StandaloneLayout"
            });
        };
    </script>
</body>
</html>
```

---

### **2. Serve `index.html` from ESP32**
Modify your `AsyncWebServer` setup to serve this page:

```cpp
#include "FS.h"
#include "LittleFS.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }

    // Serve the Swagger UI page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Serve the Swagger JSON file
    server.on("/swagger.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/swagger.json", "application/json");
    });

    server.begin();
}

void loop() {
}
```

---

### **3. Upload `swagger.json` to LittleFS**
Create `swagger.json` in `/data/swagger.json`:

```json
{
  "openapi": "3.0.0",
  "info": {
    "title": "ESP32 API",
    "version": "1.0.0",
    "description": "API documentation for ESP32 endpoints"
  },
  "servers": [
    { "url": "http://192.168.1.100", "description": "Local ESP32 server" }
  ],
  "paths": {
    "/device/reboot": {
      "post": {
        "summary": "Reboots the ESP32 device",
        "responses": {
          "200": { "description": "Device rebooting..." }
        }
      }
    },
    "/settings/get": {
      "get": {
        "summary": "Retrieve device settings",
        "responses": {
          "200": {
            "description": "Returns the current settings",
            "content": {
              "application/json": {
                "schema": {
                  "type": "object",
                  "properties": {
                    "device": { "type": "object" },
                    "wifi": { "type": "object" }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
```

---

### **4. Upload Files to ESP32**
If using PlatformIO:
1. Place `index.html` and `swagger.json` inside `data/`
2. Run:
   ```sh
   pio run --target uploadfs
   ```

---

### **5. Access Swagger UI**
- Go to: `http://<ESP32-IP>/`
- The UI will load, pulling the API definition from `swagger.json`.

---

### **Advantages of Using an External Swagger UI**
- ✅ **Saves storage** on ESP32 by not storing Swagger UI files locally.
- ✅ **Always updated** with latest Swagger UI versions from CDN.
- ✅ **Minimal setup**—only need `index.html` and `swagger.json`.

---

Would you like help dynamically generating `swagger.json` from your ESP32 endpoints?

Yes! You can host a **dark-themed Swagger UI** on your ESP32 while using externally hosted packages from a CDN. Swagger UI doesn't have a built-in dark mode, but you can apply a custom dark theme using CSS.

---

## **Steps to Host a Dark-Themed Swagger Page on ESP32**
1. **Use an externally hosted Swagger UI via CDN**
2. **Apply a dark theme using custom CSS**
3. **Serve the `index.html` and `swagger.json` from ESP32**
4. **Access your API documentation in dark mode**

---

## **1. Create a Dark-Themed `index.html`**
Save this file as `/index.html` in your ESP32's LittleFS:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 API Documentation</title>
    
    <!-- Swagger UI CDN -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui.min.css">
    
    <!-- Dark Theme Custom Styles -->
    <style>
        body {
            background-color: #121212;
            color: #e0e0e0;
        }
        .swagger-ui {
            background-color: #121212;
        }
        .topbar {
            background-color: #1e1e1e;
        }
        .swagger-ui .info {
            color: #e0e0e0;
        }
        .swagger-ui .opblock {
            background: #222;
        }
        .swagger-ui .opblock .opblock-summary-method {
            color: #fff;
        }
        .swagger-ui .opblock .opblock-summary-description {
            color: #ccc;
        }
        .swagger-ui .scheme-container {
            background: #1e1e1e;
        }
    </style>
</head>
<body>
    <div id="swagger-ui"></div>

    <!-- Swagger UI Scripts -->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui-bundle.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/swagger-ui/4.15.5/swagger-ui-standalone-preset.min.js"></script>

    <script>
        window.onload = function() {
            const ui = SwaggerUIBundle({
                url: "/swagger.json",
                dom_id: "#swagger-ui",
                presets: [
                    SwaggerUIBundle.presets.apis,
                    SwaggerUIStandalonePreset
                ],
                layout: "StandaloneLayout",
                deepLinking: true
            });
        };
    </script>
</body>
</html>
```

---

## **2. Serve `index.html` from ESP32**
Modify your ESP32 `AsyncWebServer` to serve the Swagger page and JSON definition:

```cpp
#include "FS.h"
#include "LittleFS.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("Failed to mount LittleFS");
        return;
    }

    // Serve the Swagger UI dark theme page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // Serve the Swagger JSON definition
    server.on("/swagger.json", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/swagger.json", "application/json");
    });

    server.begin();
}

void loop() {
}
```

---

## **3. Upload `swagger.json` to ESP32**
Save this as `/swagger.json` inside `data/`:

```json
{
  "openapi": "3.0.0",
  "info": {
    "title": "ESP32 API",
    "version": "1.0.0",
    "description": "Dark-themed API documentation for ESP32"
  },
  "servers": [
    { "url": "http://192.168.1.100", "description": "Local ESP32 server" }
  ],
  "paths": {
    "/device/reboot": {
      "post": {
        "summary": "Reboots the ESP32 device",
        "responses": {
          "200": { "description": "Device rebooting..." }
        }
      }
    },
    "/settings/get": {
      "get": {
        "summary": "Retrieve device settings",
        "responses": {
          "200": {
            "description": "Returns the current settings",
            "content": {
              "application/json": {
                "schema": {
                  "type": "object",
                  "properties": {
                    "device": { "type": "object" },
                    "wifi": { "type": "object" }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
```

---

## **4. Upload Files to ESP32**
If using **PlatformIO**, place `index.html` and `swagger.json` inside `data/` and upload using:

```sh
pio run --target uploadfs
```

---

## **5. Access the Dark-Mode Swagger Page**
1. Connect to your ESP32's IP (e.g., `http://192.168.1.100/`)
2. The Swagger UI will load in **dark mode**
3. You can now browse and test your API in a visually comfortable theme

---

### **Advantages of This Approach**
✅ **Uses External Swagger UI CDN** → Saves ESP32 flash memory  
✅ **Custom Dark Mode Styling** → Improves readability  
✅ **No Extra Dependencies** → Works with existing ESP32 Web Server  

Would you like help generating `swagger.json` dynamically based on registered endpoints?