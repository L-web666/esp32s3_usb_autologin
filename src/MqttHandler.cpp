#ifdef ENABLE_MQTT_HANDLER

#include "MqttHandler.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LittleFS.h>

// Constants
static constexpr uint32_t DEFAULT_TIMEOUT_MS = 500;  // Initial reconnect delay
static constexpr uint32_t MAX_BACKOFF_MS = 900000;    // Max delay (15 minutes)
static constexpr uint8_t BACKOFF_FACTOR = 5;         // Exponential multiplier

// Static variables
static WiFiClient wifiClient;
static WiFiClientSecure wifiClientSecure;
static PubSubClient mqttClient(wifiClient);          // Default to non-secure client
static NonBlockingTimer mqttReconnectTimer(DEFAULT_TIMEOUT_MS);
static uint32_t currentBackoffMs = DEFAULT_TIMEOUT_MS;  // Tracks current delay

void MqttHandler::init() {
  if (!settings.mqtt.enabled) {
    return;
  }

  registerCommands();
  connectToMqtt();
}

void MqttHandler::loop() {
  if (!settings.mqtt.enabled) {
    return;
  }

  if (!mqttClient.connected()) {
    if (mqttReconnectTimer.isReady()) {
      debugW("MQTT: Disconnected, attempting to reconnect with backoff=%d ms", currentBackoffMs);
      settings.mqtt.isConnected = false;
      connectToMqtt();
    }
  } else {
    mqttClient.loop();  // Process incoming MQTT messages
    currentBackoffMs = DEFAULT_TIMEOUT_MS;  // Reset backoff on successful connection
  }
}

void MqttHandler::publish(const char* topic, const char* message) {
  if (!topic || !message) {
    debugE("MQTT: Invalid topic or message pointer");
    return;
  }

  if (!mqttClient.connected()) {
    debugW("MQTT: Disconnected, attempting reconnect before publish...");
    connectToMqtt();
  }

  debugI("MQTT: Publishing on [%s]: %s", topic, message);
  if (!mqttClient.publish(topic, message)) {
    debugE("MQTT: Publish failed");
  }
}

bool MqttHandler::loadCertificate(String& certContent) {
  if (!LittleFS.begin()) {
    debugE("MQTT: Failed to initialize LittleFS");
    return false;
  }

  File certFile = LittleFS.open(EMQX_CERT_FILE, "r");
  if (!certFile) {
    debugE("MQTT: Failed to open certificate file: %s", EMQX_CERT_FILE);
    return false;
  }

  certContent = certFile.readString();
  certFile.close();

  debugV("MQTT: Loaded certificate (%d bytes): %s", certContent.length(), certContent.c_str());
  return true;
}

void MqttHandler::connectToMqtt() {
  // Configure client based on SSL setting
  if (settings.mqtt.ssl) {
    debugI("MQTT: Configuring secure connection");
    String certContent;
    if (!loadCertificate(certContent)) {
      return;
    }
    wifiClientSecure.setCACert(certContent.c_str());
    mqttClient.setClient(wifiClientSecure);
  } else {
    debugI("MQTT: Using non-secure connection");
    mqttClient.setClient(wifiClient);
  }

  // Set server and callback
  mqttClient.setServer(settings.mqtt.server.c_str(), settings.mqtt.port);
  mqttClient.setCallback(handleMqttCallback);

  debugI("MQTT: Connecting to %s:%d", settings.mqtt.server.c_str(), settings.mqtt.port);

  // Attempt connection with or without credentials
  bool success = false;
  if (!settings.mqtt.username.isEmpty() || !settings.mqtt.password.isEmpty()) {
    debugI("MQTT: Connecting with credentials: [%s]", settings.mqtt.username.c_str());
    success = mqttClient.connect(settings.device.name.c_str(), 
                                settings.mqtt.username.c_str(), 
                                settings.mqtt.password.c_str());
  } else {
    debugI("MQTT: Connecting without credentials");
    success = mqttClient.connect(settings.device.name.c_str());
  }

  if (success) {
    debugI("MQTT: Connected as [%s]", settings.device.name.c_str());
    mqttClient.subscribe(settings.mqtt.subTopic.c_str());
    debugI("MQTT: Subscribed to [%s]", settings.mqtt.subTopic.c_str());

    String helloMessage = "Device: " + settings.device.name + " connected.";
    publish(settings.mqtt.pubTopic.c_str(), helloMessage.c_str());
    settings.mqtt.isConnected = true;

    // Reset backoff and timer on success
    currentBackoffMs = DEFAULT_TIMEOUT_MS;
    mqttReconnectTimer.setInterval(currentBackoffMs);
  } else {
    debugE("MQTT: Connection failed, state=%d", mqttClient.state());
    // Increase backoff exponentially, cap at MAX_BACKOFF_MS
    currentBackoffMs = min(currentBackoffMs * BACKOFF_FACTOR, MAX_BACKOFF_MS);
    mqttReconnectTimer.setInterval(currentBackoffMs);
  }
}

void MqttHandler::handleMqttCallback(char* topic, uint8_t* payload, uint32_t length) {
  if (!topic || !payload) {
    debugE("MQTT: Invalid callback parameters");
    return;
  }

  String message;
  message.reserve(length);  // Pre-allocate for efficiency
  for (uint32_t i = 0; i < length; i++) {
    message += static_cast<char>(payload[i]);
  }

  debugI("MQTT: Received on [%s]: %s", topic, message.c_str());

  if (String(topic) == settings.mqtt.subTopic) {
    debugD("MQTT: Echoing message to publish topic");
    publish(settings.mqtt.pubTopic.c_str(), message.c_str());
    CommandHandler::handleCommand(message);
  }
}

void MqttHandler::registerCommands() {
  CommandHandler::registerCommand(
      "mqtt", 
      [](const String& command) {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "msg")) {
          MqttHandler::publish(settings.mqtt.pubTopic.c_str(), args.c_str());
        } else if (CommandHandler::equalsIgnoreCase(cmd, "topic")) {
          int32_t delimiterPos = args.indexOf(' ');
          if (delimiterPos == -1) {
            debugW("MQTT: Invalid topic command format");
            return;
          }
          String topic = args.substring(0, delimiterPos);
          String message = args.substring(delimiterPos + 1);
          MqttHandler::publish(topic.c_str(), message.c_str());
        } else {
          debugW("MQTT: Unknown subcommand: %s", cmd.c_str());
        }
      }, 
      "Handles MQTT commands.\n"
      "Usage: mqtt <subcommand> [args]\n"
      "  msg <message> - Publish to default topic\n"
      "  topic <topic> <message> - Publish to specified topic");
}

#endif  // ENABLE_MQTT_HANDLER