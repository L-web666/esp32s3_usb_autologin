Yes, you can restrict REST endpoints to only be accessible from the localhost when using the `ESPAsyncWebServer` library on your ESP32. Since the ESP32 doesn’t natively differentiate between localhost and external requests like a full-fledged server, you’ll need to inspect the incoming request’s `Host` header or the source IP address to enforce this restriction.

Here’s how you can do it:

### Approach 1: Check Client IP
On an ESP32, the client IP address of requests made from the device itself (localhost) is typically `127.0.0.1` or `::1` (for IPv6). You can use the `client()` method of the `AsyncWebServerRequest` object to check the source IP address.

```cpp
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Add a locked-down endpoint
  server.on("/restricted", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Check if the request is from localhost
    if (request->client()->remoteIP().toString() == "127.0.0.1") {
      request->send(200, "text/plain", "Hello, localhost!");
    } else {
      request->send(403, "text/plain", "Forbidden: Not allowed from this IP");
    }
  });

  // Start the server
  server.begin();
}

void loop() {
  // Nothing needed here for AsyncWebServer
}
```

### Approach 2: Use Custom Headers or Tokens
Another secure approach is to require a custom header or token in the request, which is only known to the device itself. This can act as a secondary layer of security.

```cpp
server.on("/secure", HTTP_GET, [](AsyncWebServerRequest *request) {
  if (request->hasHeader("X-Localhost-Token") && 
      request->header("X-Localhost-Token") == "YourSecureToken") {
    request->send(200, "text/plain", "Access granted");
  } else {
    request->send(403, "text/plain", "Forbidden: Invalid token");
  }
});
```

### Important Notes
1. **ESP32 Network Context**: In most use cases, the ESP32 handles requests over Wi-Fi. Therefore, the concept of "localhost" usually doesn’t apply the same way as on a computer. Instead, the ESP32 will treat itself as any other device on the network.
2. **Access Control via Firewall**: If your ESP32 is in a controlled environment, consider network-level restrictions (e.g., router firewall rules) to restrict access to your REST endpoints.
3. **Authentication**: For critical endpoints, always enforce proper authentication and use HTTPS (e.g., with SSL/TLS via ESPAsyncTCP and secure certificates).
4. **Localhost Specificity**: If your ESP32 device is self-issuing requests to itself, make sure it explicitly uses `127.0.0.1` in the client code.

With these techniques, you can effectively restrict access to specific endpoints based on your security requirements.