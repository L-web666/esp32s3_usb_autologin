#pragma once

#include "Globals.h"
#include <Preferences.h>
#include <Arduino.h>

// Sub-struct for device info
struct DeviceSettings {
    String name;
    bool setupMode;
    String timezone;
    int defaultTimeout;
    int keyPressDelay;
    uint64_t bootCount;
    uint64_t bootTime;
    uint64_t upTime;
    String userName;
    String userPassword;
    String singlePress;
    String doublePress;
    String longPress;
    String bootCommand;

    // ---- 自动登录（插上电脑自动输入密码解锁）----
    bool autoLogin;              // 是否启用
    int autoLoginDelay;          // 上电后等多久才开始（毫秒）
    String loginUserName;        // Windows 用户名（留空则只打密码）
    String loginPassword;        // Windows 密码（留空则只按回车）
    bool loginSendUserName;      // 是否先输入用户名 + TAB
    bool loginWakeScreen;        // 是否先按 NumLock 叫醒屏幕
    bool loginEnter;             // 打完密码是否按回车
};

// Sub-struct for Wi-Fi
struct WiFiSettings {
    String ssid;
    String password;
    bool scan;
};

// Sub-struct for MQTT
struct MqttSettings {
    bool enabled;
    String server;
    int port;
    bool ssl;
    String username; //todo fix name to userName for consistency
    String password; //todo fix name to userPassword for consistency
    String subTopic;
    String pubTopic;
    bool isConnected;
};

// Sub-struct for security/auth
struct SecuritySettings {
    String apiKey;
    bool apiToken;
    String otaPassword;
};

// Sub-struct for feature toggles
struct FeatureSettings {
    bool cors;
    bool webHandler;
};

// Main Settings container
struct Settings {
    DeviceSettings device;
    WiFiSettings wifi;
    MqttSettings mqtt;
    SecuritySettings security;
    FeatureSettings features;
};

// Global settings object
extern Settings settings;

// ConfigManager class declaration
class ConfigManager {
public:
    static void init();
    static void save();
    static void clear();
    static void savePreferences();
    static void clearPreferences();

private:
    static void load();
    static void loadPreferences();
    static Preferences preferences;
};