
---

I am working on an ESP32 project using the Arduino framework and ArduinoJson library. I am trying to implement an `ActionHandler` class that processes a JSON string or `JsonDocument`. The JSON contains an `action` field (e.g., `led`, `reboot`) and an optional `parameters` object with key-value pairs.

The goal is to:
1. Parse the JSON.
2. Extract the `action` and log it using `RemoteDebug`.
3. Handle `parameters` if they exist, logging each key-value pair.
4. Use robust type-checking to avoid runtime errors and invalid type conversions.
5. I already have a debbufer implemented globally it can be called with debugE('message') debugI('message')
6. This json library does not require declaring size use this to declare JsonDocument doc;

Here’s an example JSON payload:
```json
{
  "action": "led",
  "parameters": {
    "color": "red",
    "brightness": 100
  }
}

{
  "action": "tft",
  "message": "Hello World"
}

{
  "action": "reboot"
}
```

The issue I am facing:
- I get the following error: **`invalid use of incomplete type 'InvalidConversion'`** when attempting to cast `doc["parameters"].as<JsonObject>()`.
- Using `DynamicJsonDocument` or `StaticJsonDocument` triggers warnings about deprecation.
- I need a working example that avoids these issues and adheres to ArduinoJson best practices.

Environment:
- Platform: ESP32-S3
- ArduinoJson: Latest version (v6 or v7).

