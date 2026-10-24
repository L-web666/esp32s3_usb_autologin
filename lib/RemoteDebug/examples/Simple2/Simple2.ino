#include <WiFi.h>
#include <RemoteDebug.h>

// WiFi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// RemoteDebug instance
RemoteDebug Debug;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(100);

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize RemoteDebug
  Debug.begin("ESP32_Debug"); // Set a friendly name for debugging
  Debug.setResetCmdEnabled(true); // Allow the reset command
  Debug.showTime(true);          // Show time in debug messages
  Debug.showProfiler(true);      // Enable profiler to measure execution time

  Debug.println("RemoteDebug initialized!");
}

void loop() {
  // Handle RemoteDebug
  Debug.handle();

  // Example debug messages
  static unsigned long lastMessage = 0;
  if (millis() - lastMessage > 5000) {
    lastMessage = millis();
    Debug.println("Sending a debug message...");
    Debug.printf("Uptime: %lu seconds\n", millis() / 1000);
  }
}
