#pragma once

#define WIFI_NETWORKS_FILE "/data/networks.json"

//#define CATEGORIES_FILE "/data/categories.json"
#define CATEGORIES_FILE "/data/categoriesSecure.json"

//#define BUTTONS_FILE "/data/buttons.json"
#define BUTTONS_FILE "/data/buttonsSecure.json"

// 注意：仓库里只有 settings.json（settingsSecure.json 被 .gitignore 排除）。
// 原来固定读 settingsSecure.json，导致文件不存在时 ConfigManager::load()
// 直接返回、所有设置都是默认值（autoLogin 默认 false，自动登录完全不生效）。
// 这里改成优先读 Secure 文件、不存在时回退到普通文件，见 ConfigManager.cpp。
//#define SETTINGS_FILE "/data/settings.json"
#define SETTINGS_FILE "/data/settingsSecure.json"
#define SETTINGS_FILE_FALLBACK "/data/settings.json"

#define EMQX_CERT_FILE "/data/mqtt1.crt"
#define TIMEZONES_FILE "/data/timezones.json"

// Custom USB descriptors
#define CUSTOM_PRODUCT_NAME "Passtxt USB KeyboardMouse"
#define CUSTOM_MANUFACTURER "Passtxt"
#define CUSTOM_SERIAL_NUMBER "123456789"

//#include "Secure.h"
#include "SecureExample.h"
#include "ConfigManager.h"
#include "NonBlockingTimer.h"
#include "RemoteDebugHandler.h"
#include "BoardPins.h"
#include "CommandHandler.h"