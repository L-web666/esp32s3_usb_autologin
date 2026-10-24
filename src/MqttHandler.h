#pragma once

#include "Globals.h"  // Assuming this is the correct header

#ifdef ENABLE_MQTT_HANDLER

class MqttHandler {
 public:
  static void init();
  static void loop();
  static void publish(const char* topic, const char* message);

 private:
  static void connectToMqtt();
  static void handleMqttCallback(char* topic, uint8_t* payload, uint32_t length);
  static void registerCommands();
  static bool loadCertificate(String& certContent);
};

#else

// No-op implementation when MQTT is disabled
class MqttHandler {
 public:
  static void init() {}
  static void loop() {}
  static void publish(const char* topic, const char* message) {}
};

#endif  // ENABLE_MQTT_HANDLER