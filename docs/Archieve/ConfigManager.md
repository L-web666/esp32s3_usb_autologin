
---

#### **ConfigManager.h**
```cpp
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <Preferences.h>
#include <Arduino.h>

// Struct for all settings
struct Settings {
    String wifiSSID = "defaultSSID";
    String wifiPassword = "defaultPassword";
    String deviceName = "ESP32_Device";
    bool setupMode = true;
    int timeout = 30;
    bool ledEnabled = false; // Example for another bool setting
};

// Global settings object
extern Settings settings;

// ConfigManager class declaration
class ConfigManager {
public:
    static void begin();  // Initialize preferences and load settings
    static void save();   // Save settings to preferences

private:
    static void load();   // Load settings from preferences
    static Preferences preferences;
};

#endif // CONFIGMANAGER_H
```

---

#### **ConfigManager.cpp**
```cpp
#include "ConfigManager.h"

// Global settings object
Settings settings;

// Preferences instance
Preferences ConfigManager::preferences;

// Initialize preferences and load settings
void ConfigManager::begin() {
    preferences.begin("config", false);  // Open preferences namespace
    load();  // Load settings from preferences
}

// Load settings with defaults
void ConfigManager::load() {
    settings.wifiSSID = preferences.getString("wifiSSID", settings.wifiSSID);
    settings.wifiPassword = preferences.getString("wifiPassword", settings.wifiPassword);
    settings.deviceName = preferences.getString("deviceName", settings.deviceName);
    settings.setupMode = preferences.getBool("setupMode", settings.setupMode);
    settings.timeout = preferences.getInt("timeout", settings.timeout);
    settings.ledEnabled = preferences.getBool("ledEnabled", settings.ledEnabled);
}

// Save settings to preferences
void ConfigManager::save() {
    preferences.putString("wifiSSID", settings.wifiSSID);
    preferences.putString("wifiPassword", settings.wifiPassword);
    preferences.putString("deviceName", settings.deviceName);
    preferences.putBool("setupMode", settings.setupMode);
    preferences.putInt("timeout", settings.timeout);
    preferences.putBool("ledEnabled", settings.ledEnabled);
}
```

---

### Usage Example

#### **main.cpp**
```cpp
#include <Arduino.h>
#include "ConfigManager.h"

void setup() {
    Serial.begin(115200);

    // Initialize configuration
    ConfigManager::begin();

    // Access settings directly without declaring them
    Serial.println("Initial Settings:");
    Serial.println("WiFi SSID: " + settings.wifiSSID);
    Serial.println("Device Name: " + settings.deviceName);
    Serial.println("Setup Mode: " + String(settings.setupMode));
    Serial.println("Timeout: " + String(settings.timeout));
    Serial.println("LED Enabled: " + String(settings.ledEnabled));

    // Modify settings directly
    settings.wifiSSID = "MyNewSSID";
    settings.wifiPassword = "NewPassword";
    settings.deviceName = "NewDevice";
    settings.setupMode = false;
    settings.timeout = 60;
    settings.ledEnabled = true;

    // Save updated settings
    ConfigManager::save();

    Serial.println("\nUpdated Settings Saved!");
}

void loop() {
    // Example: use settings
    if (settings.ledEnabled) {
        Serial.println("LED is enabled. Performing LED actions...");
        delay(1000);
    }
}
```

---

#### **WiFiHandler.cpp**
```cpp
#include "ConfigManager.h"
#include <WiFi.h>

void connectToWiFi() {
    // Directly access global settings
    Serial.println("Connecting to WiFi: " + settings.wifiSSID);
    WiFi.begin(settings.wifiSSID.c_str(), settings.wifiPassword.c_str());

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected!");
    Serial.println("IP Address: " + WiFi.localIP().toString());
}
```

---

#### **LEDHandler.cpp**
```cpp
#include "ConfigManager.h"

void toggleLED() {
    // Directly access global settings
    if (settings.ledEnabled) {
        Serial.println("Toggling LED ON...");
        // Example: digitalWrite(LED_PIN, HIGH);
    } else {
        Serial.println("LED is disabled.");
    }
}
```

---

### Key Features of This Approach
1. **Global Access**:
   - `settings` is a single global object declared in `ConfigManager.h` and can be used directly in any file without additional declarations.

2. **Encapsulation**:
   - The `ConfigManager` class encapsulates all logic for loading and saving settings.

3. **Consistency**:
   - Usage of `settings` is the same in all files—no need to include or declare anything extra.

4. **Minimal Code in Handlers**:
   - Handlers access the `settings` object directly, keeping them simple.

---

### **Why This is Closer to Your Debugger Example**
- Just like your debugger, you **only include `ConfigManager.h`**, and the `settings` object is globally accessible.
- There’s **no need to explicitly declare or retrieve `settings` in each handler**; it works out of the box.

This structure ensures simplicity while maintaining clean separation of configuration logic.