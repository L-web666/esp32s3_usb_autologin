#include "ConfigManager.h"
#include <LittleFS.h>
#include <ArduinoJson.h>

// Global settings object
Settings settings;

// 自动登录的默认值。
// 注意 settings 是全局对象（零初始化），bool 默认 false、int 默认 0，
// 所以这里显式给一份合理默认：
//   autoLogin 默认 false —— 没见过配置就不乱打字，避免误触
//   loginWakeScreen / loginEnter 默认 true —— 打开后即可正确工作
static void applyAutoLoginDefaults(DeviceSettings &d)
{
    d.autoLogin = false;
    d.autoLoginDelay = 5000;
    d.loginSendUserName = false;
    d.loginWakeScreen = true;
    d.loginEnter = true;
}

// Preferences instance
Preferences ConfigManager::preferences;

// Namespace for preferences
const char *ns = "config";

// Initialize preferences and load settings
void ConfigManager::init() {
    applyAutoLoginDefaults(settings.device);
    load();
}

// Load settings from LittleFS
void ConfigManager::load() {
    // 优先读 settingsSecure.json（放敏感配置）；
    // 不存在时回退到 settings.json —— 仓库里只提供后者，
    // 之前固定读 Secure 文件导致"设置完全不生效"（自动登录默认关闭）。
    const char *path = SETTINGS_FILE;
    if (!LittleFS.exists(path) && LittleFS.exists(SETTINGS_FILE_FALLBACK)) {
        path = SETTINGS_FILE_FALLBACK;
        debugI("%s not found, falling back to %s", SETTINGS_FILE, path);
    }

    File file = LittleFS.open(path, "r");
    if (!file || file.size() == 0) {
        debugE("Failed to open %s or file is empty", path);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        debugE("Failed to parse JSON: %s", error.c_str());
        return;
    }

    // Device
    if (doc["device"]["name"]) settings.device.name = doc["device"]["name"].as<String>();
    if (doc["device"]["setupMode"]) settings.device.setupMode = doc["device"]["setupMode"].as<bool>();
    if (doc["device"]["timezone"]) settings.device.timezone = doc["device"]["timezone"].as<String>();
    if (doc["device"]["bootCount"]) settings.device.bootCount = doc["device"]["bootCount"].as<uint64_t>();
    if (doc["device"]["bootTime"]) settings.device.bootTime = doc["device"]["bootTime"].as<uint64_t>();
    if (doc["device"]["defaultTimeout"]) settings.device.defaultTimeout = doc["device"]["defaultTimeout"].as<int>();
    if (doc["device"]["keyPressDelay"]) settings.device.keyPressDelay = doc["device"]["keyPressDelay"].as<int>();
    
    // Device security
    if (doc["device"]["userName"]) settings.device.userName = doc["device"]["userName"].as<String>();
    if (doc["device"]["userPassword"]) settings.device.userPassword = doc["device"]["userPassword"].as<String>();

    // Device button commands
    if (doc["device"]["singlePress"]) settings.device.singlePress = doc["device"]["singlePress"].as<String>();
    if (doc["device"]["doublePress"]) settings.device.doublePress = doc["device"]["doublePress"].as<String>();
    if (doc["device"]["longPress"]) settings.device.longPress = doc["device"]["longPress"].as<String>();
    
    // Device boot command
    if (doc["device"]["bootCommand"]) settings.device.bootCommand = doc["device"]["bootCommand"].as<String>();

    // Auto login to Windows on boot
    if (doc["device"]["autoLogin"]) settings.device.autoLogin = doc["device"]["autoLogin"].as<bool>();
    if (doc["device"]["autoLoginDelay"]) settings.device.autoLoginDelay = doc["device"]["autoLoginDelay"].as<int>();
    if (doc["device"]["loginUserName"]) settings.device.loginUserName = doc["device"]["loginUserName"].as<String>();
    if (doc["device"]["loginPassword"]) settings.device.loginPassword = doc["device"]["loginPassword"].as<String>();
    if (doc["device"]["loginSendUserName"]) settings.device.loginSendUserName = doc["device"]["loginSendUserName"].as<bool>();
    if (doc["device"]["loginWakeScreen"]) settings.device.loginWakeScreen = doc["device"]["loginWakeScreen"].as<bool>();
    if (doc["device"]["loginEnter"]) settings.device.loginEnter = doc["device"]["loginEnter"].as<bool>();
    
    // WiFi
    if (doc["wifi"]["ssid"]) settings.wifi.ssid = doc["wifi"]["ssid"].as<String>();
    if (doc["wifi"]["password"]) settings.wifi.password = doc["wifi"]["password"].as<String>();
    if (doc["wifi"]["scan"]) settings.wifi.scan = doc["wifi"]["scan"].as<bool>();

    // MQTT
    if (doc["mqtt"]["enabled"]) settings.mqtt.enabled = doc["mqtt"]["enabled"].as<bool>();
    if (doc["mqtt"]["server"]) settings.mqtt.server = doc["mqtt"]["server"].as<String>();
    if (doc["mqtt"]["port"]) settings.mqtt.port = doc["mqtt"]["port"].as<int>();
    if (doc["mqtt"]["ssl"]) settings.mqtt.ssl = doc["mqtt"]["ssl"].as<bool>();
    if (doc["mqtt"]["username"]) settings.mqtt.username = doc["mqtt"]["username"].as<String>();
    if (doc["mqtt"]["password"]) settings.mqtt.password = doc["mqtt"]["password"].as<String>();
    if (doc["mqtt"]["subTopic"]) settings.mqtt.subTopic = doc["mqtt"]["subTopic"].as<String>();
    if (doc["mqtt"]["pubTopic"]) settings.mqtt.pubTopic = doc["mqtt"]["pubTopic"].as<String>();

    // Security
    if (doc["security"]["apiKey"]) settings.security.apiKey = doc["security"]["apiKey"].as<String>();
    if (doc["security"]["apiToken"]) settings.security.apiToken = doc["security"]["apiToken"].as<bool>();
    if (doc["security"]["otaPassword"]) settings.security.otaPassword = doc["security"]["otaPassword"].as<String>();

    // Features
    if (doc["features"]["cors"]) settings.features.cors = doc["features"]["cors"].as<bool>();
    if (doc["features"]["webHandler"]) settings.features.webHandler = doc["features"]["webHandler"].as<bool>();

    debugI("Settings loaded from %s", SETTINGS_FILE);
}

// Save settings to LittleFS
void ConfigManager::save() {
    JsonDocument doc; // or StaticJsonDocument if you prefer

    // Device
    doc["device"]["name"] = settings.device.name;
    doc["device"]["setupMode"] = settings.device.setupMode;
    doc["device"]["timezone"] = settings.device.timezone;
    doc["device"]["bootCount"] = settings.device.bootCount;
    doc["device"]["bootTime"] = settings.device.bootTime;
    doc["device"]["defaultTimeout"] = settings.device.defaultTimeout;
    doc["device"]["keyPressDelay"] = settings.device.keyPressDelay; // Save the key press delay

    // Device security
    doc["device"]["userName"] = settings.device.userName;
    doc["device"]["userPassword"] = settings.device.userPassword;
    
    // Device button commands
    doc["device"]["singlePress"] = settings.device.singlePress;
    doc["device"]["doublePress"] = settings.device.doublePress;
    doc["device"]["longPress"] = settings.device.longPress;

    // Device boot command
    doc["device"]["bootCommand"] = settings.device.bootCommand;

    // Auto login to Windows on boot
    doc["device"]["autoLogin"] = settings.device.autoLogin;
    doc["device"]["autoLoginDelay"] = settings.device.autoLoginDelay;
    doc["device"]["loginUserName"] = settings.device.loginUserName;
    doc["device"]["loginPassword"] = settings.device.loginPassword;
    doc["device"]["loginSendUserName"] = settings.device.loginSendUserName;
    doc["device"]["loginWakeScreen"] = settings.device.loginWakeScreen;
    doc["device"]["loginEnter"] = settings.device.loginEnter;
    
    // WiFi
    doc["wifi"]["ssid"] = settings.wifi.ssid;
    doc["wifi"]["password"] = settings.wifi.password;
    doc["wifi"]["scan"] = settings.wifi.scan;

    // MQTT
    doc["mqtt"]["enabled"] = settings.mqtt.enabled;
    doc["mqtt"]["server"] = settings.mqtt.server;
    doc["mqtt"]["port"] = settings.mqtt.port;
    doc["mqtt"]["ssl"] = settings.mqtt.ssl;
    doc["mqtt"]["username"] = settings.mqtt.username;
    doc["mqtt"]["password"] = settings.mqtt.password;
    doc["mqtt"]["subTopic"] = settings.mqtt.subTopic;
    doc["mqtt"]["pubTopic"] = settings.mqtt.pubTopic;

    // Security
    doc["security"]["apiKey"] = settings.security.apiKey;
    doc["security"]["apiToken"] = settings.security.apiToken;
    doc["security"]["otaPassword"] = settings.security.otaPassword;
    
    // Features
    doc["features"]["cors"] = settings.features.cors;
    doc["features"]["webHandler"] = settings.features.webHandler;

    // 写回"当前实际在用的那个文件"：如果启动时回退到了 settings.json，
    // 保存也写 settings.json，避免读出和写回变成两个文件、配置自我分裂。
    const char *path = SETTINGS_FILE;
    if (!LittleFS.exists(path) && LittleFS.exists(SETTINGS_FILE_FALLBACK)) {
        path = SETTINGS_FILE_FALLBACK;
    }

    File file = LittleFS.open(path, "w");
    if (!file) {
        debugE("Failed to open %s for writing", path);
        return;
    }

    size_t written = serializeJsonPretty(doc, file);
    file.close();

    if (written == 0) {
        debugE("Failed to write JSON to %s", path);
    } else {
        debugI("Settings saved to %s (%u bytes)", path, (unsigned int)written);
    }
}

// Clear all LittleFS
void ConfigManager::clear() {
    if (LittleFS.exists(SETTINGS_FILE)) {
        LittleFS.remove(SETTINGS_FILE);
        debugI("Settings file %s removed", SETTINGS_FILE);
    } else {
        debugW("Settings file %s not found", SETTINGS_FILE);
    }

    settings = Settings(); // Reset to default-initialized struct
}

// Load settings from preferences
void ConfigManager::loadPreferences() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for loading.");
        return;
    }

    // Device
    settings.device.name         = preferences.getString("deviceName", settings.device.name);
    settings.device.timezone     = preferences.getString("timezone", settings.device.timezone);
    settings.device.setupMode    = preferences.getBool("setupMode", settings.device.setupMode);
    settings.device.defaultTimeout = preferences.getInt("defaultTimeout", settings.device.defaultTimeout);
    settings.device.bootCount    = preferences.getULong("bootCount", settings.device.bootCount);
    settings.device.bootTime     = preferences.getULong("bootTime", settings.device.bootTime);
    settings.device.keyPressDelay = preferences.getInt("keyPressDelay", settings.device.keyPressDelay); // Load the key press delay

    // Device security
    settings.device.userName     = preferences.getString("deviceUserName", settings.device.userName);
    settings.device.userPassword = preferences.getString("deviceUserPassword", settings.device.userPassword);

    // WiFi
    settings.wifi.ssid           = preferences.getString("wifiSSID", settings.wifi.ssid);
    settings.wifi.password       = preferences.getString("wifiPassword", settings.wifi.password);
    settings.wifi.scan           = preferences.getBool("wifiScan", settings.wifi.scan);

    // MQTT
    settings.mqtt.enabled        = preferences.getBool("mqttEnabled", settings.mqtt.enabled);
    settings.mqtt.server         = preferences.getString("mqttServer", settings.mqtt.server);
    settings.mqtt.port           = preferences.getInt("mqttPort", settings.mqtt.port);
    settings.mqtt.ssl            = preferences.getBool("mqttSsl", settings.mqtt.ssl);
    settings.mqtt.username       = preferences.getString("mqttUsername", settings.mqtt.username);
    settings.mqtt.password       = preferences.getString("mqttPassword", settings.mqtt.password);
    settings.mqtt.subTopic       = preferences.getString("mqttSubTopic", settings.mqtt.subTopic);
    settings.mqtt.pubTopic       = preferences.getString("mqttPubTopic", settings.mqtt.pubTopic);

    // Security
    settings.security.apiKey     = preferences.getString("apiKey", settings.security.apiKey);
    settings.security.apiToken   = preferences.getBool("apiToken", settings.security.apiToken);
    settings.security.otaPassword = preferences.getString("otaPassword", settings.security.otaPassword);

    // Features
    settings.features.cors       = preferences.getBool("cors", settings.features.cors);
    settings.features.webHandler = preferences.getBool("webHandler", settings.features.webHandler);

    preferences.end();
    debugI("ConfigManager loaded settings from namespace: %s", ns);
}

// Save settings to preferences
void ConfigManager::savePreferences() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for saving.");
        return;
    }

    // Device
    preferences.putString("deviceName", settings.device.name);
    preferences.putString("timezone", settings.device.timezone);
    preferences.putBool("setupMode", settings.device.setupMode);
    preferences.putInt("defaultTimeout", settings.device.defaultTimeout);
    preferences.putULong("bootCount", settings.device.bootCount);
    preferences.putULong("bootTime", settings.device.bootTime);
    preferences.putInt("keyPressDelay", settings.device.keyPressDelay); // Save the key press delay

    // Device security
    preferences.putString("deviceUserName", settings.device.userName);
    preferences.putString("deviceUserPassword", settings.device.userPassword);

    // WiFi
    preferences.putString("wifiSSID", settings.wifi.ssid);
    preferences.putString("wifiPassword", settings.wifi.password);
    preferences.putBool("wifiScan", settings.wifi.scan);

    // MQTT
    preferences.putBool("mqttEnabled", settings.mqtt.enabled);
    preferences.putString("mqttServer", settings.mqtt.server);
    preferences.putInt("mqttPort", settings.mqtt.port);
    preferences.putBool("mqttSsl", settings.mqtt.ssl);
    preferences.putString("mqttUsername", settings.mqtt.username);
    preferences.putString("mqttPassword", settings.mqtt.password);
    preferences.putString("mqttSubTopic", settings.mqtt.subTopic);
    preferences.putString("mqttPubTopic", settings.mqtt.pubTopic);

    // Security
    preferences.putString("apiKey", settings.security.apiKey);
    preferences.putBool("apiToken", settings.security.apiToken);
    preferences.putString("otaPassword", settings.security.otaPassword);

    // Features
    preferences.putBool("cors", settings.features.cors);
    preferences.putBool("webHandler", settings.features.webHandler);

    preferences.end();
    debugI("ConfigManager saved settings to namespace: %s", ns);
}

// Clear all preferences
void ConfigManager::clearPreferences() {
    if (!preferences.begin(ns, false)) {
        debugE("Failed to open preferences for clearing.");
        return;
    }

    preferences.clear();
    preferences.end();

    settings = Settings(); // reset to defaults
    debugI("ConfigManager cleared preferences in namespace: %s", ns);
}