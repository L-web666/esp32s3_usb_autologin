# ESP32-S3 USB Auto-Login Keyboard

> 插上电脑就是一个 USB 键盘，自动输入 Windows 账号密码完成解锁登录。
> 通过板载 CH340 串口发一行命令就能改密码，**不需要重新编译或烧录**。

基于 [PassTxt](https://github.com/mdelgert/PassTxt) 二次开发，新增自动登录与串口配置功能。

[![PlatformIO](https://img.shields.io/badge/PlatformIO-espressif32%206.10.0-orange)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/arduino--esp32-2.0.17-blue)](https://github.com/espressif/arduino-esp32)
[![License](https://img.shields.io/badge/license-MIT-green)](LICENSE)

---

## 目录

- [功能特性](#功能特性)
- [工作原理](#工作原理)
- [硬件要求](#硬件要求)
- [快速开始](#快速开始)
- [串口命令（改密码不用烧录）](#串口命令改密码不用烧录)
- [配置文件](#配置文件)
- [凭据优先级](#凭据优先级)
- [时序调优](#时序调优)
- [构建](#构建)
- [烧录方法](#烧录方法)
- [实板验证结果](#实板验证结果)
- [踩坑记录](#踩坑记录)
- [安全说明](#安全说明)
- [已知限制](#已知限制)
- [与原版 PassTxt 的差异](#与原版-passtxt-的差异)
- [致谢](#致谢)

---

## 功能特性

| 功能 | 说明 |
|------|------|
| ⌨️ **USB 键盘** | 使用 ESP32-S3 原生 USB，免驱，即插即用 |
| 🔓 **自动登录** | 上电后自动叫醒屏幕 → 输入密码 → 回车 |
| 💾 **网页/编译都行** | 密码可放配置文件，也可用串口命令 |
| 🔧 **串口改密码** | 发 `setpwd 新密码` 即可，存 NVS，掉电不丢 |
| 🎬 **DuckyScript** | 保留原项目的脚本/宏功能 |
| 🧩 **可裁剪** | 各功能通过编译开关独立启用 |

**实测数据**（ESP32-S3 / 16MB Flash）：

```
RAM:   [=         ]  11.9% (used 38968 bytes from 327680 bytes)
Flash: [==        ]  23.7% (used 790845 bytes from 3342336 bytes)
```

---

## 工作原理

### 插入 → 登录的完整流程

```
上电
 └─> AL_WAIT_START     等 autoLoginDelay（默认 4000ms，留给系统装 HID 驱动）
      └─> AL_WAKE        按一次 NUMLOCK 叫醒屏幕、进入密码输入状态
           └─> AL_WAIT_SCREEN   等 2500ms 让登录界面绘制完成
                └─> AL_TYPE_USER   可选：输入用户名 + TAB
                     └─> AL_TYPE_PASS   逐字符输入密码
                          └─> AL_ENTER    按回车
                               └─> AL_DONE
```

每轮 `loop()` 只推进一步，**全程非阻塞**。这一点很重要：原项目的 `loop()` 里还跑着
网页服务器、MQTT、显示刷新等任务，如果用一个 `delay(5000)` 顶住，
那 5 秒内网页会卡死、MQTT 会掉线。

### 凭据来源

```
                    ┌──────────────────────────────────┐
                    │ 1. 串口 setpwd 存入 NVS 的密码    │ ← 最高优先级
                    └──────────────┬───────────────────┘
                                   ↓ 为空则
                    ┌──────────────────────────────────┐
                    │ 2. settings.loginPassword         │
                    └──────────────┬───────────────────┘
                                   ↓ 为空则
                    ┌──────────────────────────────────┐
                    │ 3. settings.userPassword（兜底）  │
                    └──────────────────────────────────┘
```

### 两个 Type-C 口的分工

这块板子（YD-ESP32-S3 / T-Dongle-S3 类）有两个 Type-C，**它们是完全独立的两条通道**：

| 接口 | 连接对象 | 固件运行时的表现 | 建议用途 |
|------|----------|------------------|----------|
| **原生 USB** | ESP32-S3 内部 USB 外设 | 枚举为 **HID 键盘**，串口消失 | 日常当键盘用 |
| **CH340** | UART0（GPIO43/44） | 始终是 **COM 串口**，不受固件影响 | 改密码、看日志 |

```
        ┌─────────────────────────────────────┐
        │            ESP32-S3                 │
        │                                     │
  原生USB├── USB 外设 ── HID 键盘 ──┐        │
        │                          │        │
  CH340 ├── UART0 (43/44) ─────────┼──────► 电脑
        │                          │        │
        └──────────────────────────┴────────┘
```

**这就是为什么配置通道放在 UART0 而不是 USB CDC**：
USB 口一旦被固件接管当键盘，就没有串口可用了；
而 CH340 走的 UART0 串口**任何时刻都在**。

---

## 硬件要求

- **芯片**：ESP32-S3（需带原生 USB 外设）
- **Flash**：8MB 或以上（分区表按 8MB 布局；实测 16MB 板子正常）
- **开发板**：带 CH340 + 两个 Type-C 的 ESP32-S3 板（如 YD-ESP32-S3、LILYGO T-Dongle-S3）
- **数据线**：支持数据传输的 USB 线（不能是纯充电线）

> 只有一个 Type-C 的板子也能用键盘功能，但**看不到日志、也没法用串口改密码**。

---

## 快速开始

### 1. 克隆

```bash
git clone <this-repo>
cd <this-repo>
```

### 2. 用 PlatformIO 构建并烧录

```bash
pio run                     # 编译
pio run -t upload           # 烧录固件（走 CH340 口最稳）
pio run -t uploadfs         # 烧录文件系统（含配置文件和网页资源）
```

### 3. 配置密码（二选一）

**方式 A — 串口命令（推荐，不用烧录）**

插上 CH340 那个 Type-C 口，用任意串口工具（波特率随意，CDC 不看波特率）连接，
发一行：

```
setpwd 你的Windows密码
```

**方式 B — 改配置文件**

编辑 `data/data/settings.json`：

```json
"device": {
    "autoLogin": true,
    "autoLoginDelay": 4000,
    "loginPassword": "你的Windows密码"
}
```

然后 `pio run -t uploadfs`。

### 4. 使用

把板子插到**原生 USB 口**（另一端插电脑），等待约 4 秒，密码会被自动输入。

---

## 串口命令（改密码不用烧录）

连接板子的 **CH340 串口**，发送文本命令（以 `\r\n` 结尾）：

| 命令 | 作用 |
|------|------|
| `setpwd <密码>` | 保存密码到 NVS，并**立即**用它登录一次 |
| `getpwd` | 查看是否已设置（只显示长度，**不回显明文**） |
| `clepwd` | 清除已保存的密码（别名 `cleapwd`） |
| `autologin` | 立即执行一次自动登录 |
| `status` | 显示串口状态、密码长度、autoLogin 配置、是否正在登录 |
| `help` | 列出所有命令（别名 `?`） |

**示例会话**：

```
> setpwd MyP@ssw0rd
OK 已保存密码（长度 10），并开始登录……

> getpwd
已设置密码，长度 10（出于安全不回显明文）

> status
串口: 未连接
已保存密码长度: 10
autoLogin(设置): 1
登录中: 否

> clepwd
OK 已清除保存的密码
```

启动时串口会打印横幅：

```
=== PassTxt AutoLogin (UART0 config channel) ===
NVS init: ESP_OK
Commands: setpwd / getpwd / clepwd / autologin / status / help
```

> **注意**：如果 CH340 和电脑都正常，`NVS init` 应为 `ESP_OK`。
> 如果显示其他错误，说明 NVS 分区异常，可整片擦除后重烧。

### PowerShell 一行改密码

```powershell
$p = New-Object System.IO.Ports.SerialPort COM8,115200
$p.Open(); $p.WriteLine("setpwd 你的密码"); Start-Sleep 1
$p.ReadExisting(); $p.Close()
```

> 把 `COM8` 换成你的 CH340 端口号（设备管理器 → 端口）。

---

## 配置文件

文件位置：`data/data/settings.json` → 板子上为 `/data/settings.json`

### 自动登录相关字段

| 字段 | 默认 | 说明 |
|------|------|------|
| `autoLogin` | `false` | **总开关**，必须为 `true` 才会自动输入 |
| `autoLoginDelay` | `5000` | 上电后等待多久才开始（毫秒）。系统慢就加大 |
| `loginUserName` | `""` | Windows 用户名，留空则不打用户名 |
| `loginPassword` | `""` | Windows 密码，留空则只按回车 |
| `loginSendUserName` | `false` | `true` 时先输入用户名 + TAB |
| `loginWakeScreen` | `true` | `true` 时先按 NumLock 叫醒屏幕 |
| `loginEnter` | `true` | 打完密码后是否按回车 |

### 完整示例

```json
{
  "device": {
    "name": "passtxt",
    "timezone": "Asia/Shanghai",
    "keyPressDelay": 20,
    "autoLogin": true,
    "autoLoginDelay": 4000,
    "loginUserName": "",
    "loginPassword": "",
    "loginSendUserName": false,
    "loginWakeScreen": true,
    "loginEnter": true
  }
}
```

> **配置读取的文件名**：程序优先读 `/data/settingsSecure.json`，
> 如果它不存在，会**自动回退**到 `/data/settings.json`。
>
> 这个回退机制很关键——原版固定读 `settingsSecure.json`，
> 而该文件被 `.gitignore` 排除、仓库里并不存在，
> 结果就是"配置完全不生效、自动登录永远不触发"。

---

## 凭据优先级

密码按以下顺序取值，**串口设置的密码优先级最高**：

1. **NVS 里由 `setpwd` 保存的密码** —— 最方便，改密码不用烧录
2. `settings.loginPassword` —— 配置文件里的专用字段
3. `settings.userPassword` —— 原项目的 web 后台密码，作为兜底

用户名优先级：`settings.loginUserName` → `settings.userName`

想恢复用配置文件里的密码，先 `clepwd` 清除 NVS 中的密码即可。

---

## 时序调优

参数位于 `src/AutoLoginHandler.cpp` 顶部：

```c
static const uint32_t AUTOLOGIN_BOOT_DELAY  = 5000;  // 上电后等待
static const uint32_t AUTOLOGIN_WAKE_DELAY  = 2500;  // 叫醒屏幕后等待
static const uint32_t AUTOLOGIN_TAB_DELAY   = 400;   // 按 TAB 后等待
static const uint32_t AUTOLOGIN_ENTER_DELAY = 300;   // 输入后到回车
```

按键间隔由 `DeviceHandler::keyPressDelay` 控制（默认 20ms，可用 `hid delay <ms>` 命令调整）。

### 症状 → 调法

| 现象 | 调整 |
|------|------|
| 丢了第一个字符 | `AUTOLOGIN_BOOT_DELAY` 加大到 8000 |
| 中间丢字符 / 顺序乱 | 加大 `keyPressDelay`（→50）或 `AUTOLOGIN_ENTER_DELAY` |
| 密码框里没光标、输入无效 | `loginWakeScreen` 保持 `true`，或把 `AUTOLOGIN_WAKE_DELAY` 加到 4000 |
| 打完密码没进去 | 确认 `loginEnter = true` |
| 多账户机器进错账户 | `loginSendUserName = true` 并填 `loginUserName` |

---

## 构建

### 环境

- [PlatformIO Core](https://platformio.org/install/cli) 6.x
- 平台：`espressif32 @ 6.10.0`
- 框架：Arduino（`framework-arduinoespressif32 @ 3.20017.0` / arduino-esp32 2.0.17）

### 编译开关

在 `platformio.ini` 中通过 `-D` 启用/禁用模块：

```ini
build_flags =
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1
    -D ENABLE_DEVICE_HANDLER         ; HID 键盘（必需）
    -D ENABLE_AUTOLOGIN_HANDLER      ; 自动登录
    -D ENABLE_SERIALCONFIG_HANDLER   ; 串口配置通道
    -D ENABLE_LITTLEFS_HANDLER
    -D ENABLE_DUCKYSCRIPT_HANDLER
    -D ENABLE_CRYPTO_HANDLER
    -D ENABLE_LED_HANDLER
    -D ENABLE_BUTTON_HANDLER
    -D ENABLE_GFX_HANDLER
    -D ENABLE_TIME_HANDLER
    -D ENABLE_JIGGLE_HANDLER
```

**默认关闭**（见[踩坑记录](#踩坑记录)）：

```ini
    ;-D ENABLE_IMPROV_WIFI_HANDLER
    ;-D ENABLE_WEB_HANDLER
    ;-D ENABLE_MQTT_HANDLER
    ;-D ENABLE_REMOTE_DEBUG_HANDLER
    ;-D ENABLE_BLUETOOTH_HANDLER
    ;-D ENABLE_OTA_HANDLER
```

### 分区表

```
Name       Type  SubType   Offset     Size
nvs        data  nvs       0x009000   0x005000
otadata    data  ota       0x00e000   0x002000
app0       app   ota_0     0x010000   0x330000
app1       app   ota_1     0x340000   0x330000
spiffs     data  spiffs    0x670000   0x180000
coredump   data  coredump  0x7f0000   0x010000
```

### 产物说明

| 文件 | 大小 | 用途 |
|------|------|------|
| `firmware.bin` | ~0.79 MB | 仅应用固件 |
| `littlefs.bin` | 1.5 MB | 配置文件系统 |
| `device.bin` | 8.0 MB | **整片烧录镜像**（bootloader + 分区表 + 固件 + 文件系统） |

整片烧录：

```bash
esptool.py --chip esp32s3 --port COM8 write_flash 0x0 device.bin
```

### 慢网络环境（中国大陆）

GitHub / PlatformIO registry 直连可能只有 ~14 KB/s，而工具链约 1 GB。
可改用乐鑫国内镜像（实测 ~30 MB/s）：

```
https://github.com/<path>  →  https://dl.espressif.cn/github_assets/<path>
```

然后通过 `platform_packages` 指向本地包：

```ini
platform_packages =
    toolchain-xtensa-esp32s3@file://C:/esp/dl/toolchain-xtensa-esp32s3-windows_x86-8.4.0+2021r2-patch5.tar.gz
    toolchain-riscv32-esp@file://C:/esp/dl/toolchain-riscv32-esp-windows_x86-8.4.0+2021r2-patch5.tar.gz
    platformio/framework-arduinoespressif32@file://C:/esp/dl/framework-arduinoespressif32-3.20017.0.tar.gz
```

---

## 烧录方法

有 4 种方式，按使用场景选择：

| 方法 | 需要安装 | 适用场景 |
|------|----------|----------|
| [1. PlatformIO](#1-platformio源码在手) | PlatformIO | 有源码、日常开发 |
| [2. esptool](#2-esptool命令行最灵活) | esptool | 最通用、可脚本化 |
| [3. 浏览器烧录](#3-浏览器烧录免安装) ⭐ | **无需安装** | 分发给别人、无开发环境 |
| [4. 浏览器在线版](#4-公共在线烧录页) | **无需安装** | 临时救急 |

### 1. PlatformIO（源码在手）

```bash
pio run -t upload              # 烧应用固件
pio run -t uploadfs            # 烧文件系统（配置 + 网页资源）
pio run -t upload --upload-port COM8   # 指定端口
```

PlatformIO 会自动处理偏移和 flash 参数，最省心。

### 2. esptool（命令行，最灵活）

**整片烧录**（推荐，一个文件搞定）：

```bash
esptool --chip esp32s3 --port COM8 write_flash 0x0 device.bin
```

**分文件烧录**（想保留已有文件系统时）：

```bash
esptool --chip esp32s3 --port COM8 write_flash \
  0x0      bootloader.bin \
  0x8000   partitions.bin \
  0x10000  firmware.bin \
  0x670000 littlefs.bin
```

> 偏移必须和[分区表](#分区表)一致，写错会导致无法启动。

### 3. 浏览器烧录（免安装）⭐

仓库自带一个网页烧录工具，别人**不需要装任何软件**就能刷固件。

**本地打开**（推荐，最稳）：

```bash
# 在该目录起一个本地 HTTP 服务
cd docs/webflasher
python -m http.server 8899

# 浏览器打开（必须是 Chrome / Edge / Firefox）
#   http://localhost:8899
```

> 浏览器要求 Web Serial 必须处于**安全上下文**，所以 `http://localhost` 可以，
> 直接双击 `index.html`（`file://` 协议）不行。

**部署到 GitHub Pages**（让别人也能在线刷）：

1. 仓库 → **Settings** → **Pages**
2. Source 选 **Deploy from a branch**
3. Branch 选 `main`，目录选 **`/docs`** → Save
4. 等待约 1 分钟，访问：

```
https://<你的用户名>.github.io/<仓库名>/webflasher/
```

**网页烧录用到的文件**（相对仓库根目录）：

```
docs/
└── webflasher/
    ├── index.html                  # 烧录页面（约 9 KB）
    ├── manifest.json               # 固件描述（约 0.3 KB）
    └── firmware/
        └── device.bin              # 完整镜像（7.94 MB / 8,323,072 字节）
```

| 文件 | 完整路径 | 说明 |
|------|----------|------|
| 烧录页面 | `docs/webflasher/index.html` | 页面 + 操作说明 + 故障排查 |
| 固件描述 | `docs/webflasher/manifest.json` | 芯片型号、版本、文件与偏移 |
| 固件本体 | `docs/webflasher/firmware/device.bin` | **单文件整片镜像，偏移 `0x0`** |

`manifest.json` 内容：

```json
{
  "name": "PassTxt AutoLogin Keyboard",
  "version": "1.0.10",
  "new_install_prompt_erase": true,
  "builds": [
    {
      "chipFamily": "ESP32-S3",
      "improv": false,
      "parts": [
        { "path": "firmware/device.bin", "offset": 0 }
      ]
    }
  ]
}
```

**关键点**：ESP Web Tools 不能像 esptool 那样在烧录时动态修改 flash 参数，
所以必须提供**已经合并好的单文件镜像**，并用 `offset: 0`。

`device.bin` 正是 `esptool merge_bin` 的产物（由 `tools/merge.py` 自动生成）：

```
0x000000  bootloader.bin
0x008000  partitions.bin
0x010000  firmware.bin
0x670000  littlefs.bin
```

**更新固件后**，需要把新的 `device.bin` 复制过去：

```bash
pio run                     # 重新构建
cp .pio/build/esp32-s3-devkitc-1/device.bin docs/webflasher/firmware/device.bin
```

> `device.bin` 有 7.9 MB，建议放在 **GitHub Releases** 而不是直接提交到仓库，
> 避免仓库体积膨胀。若放 Release，把 `manifest.json` 里的
> `path` 改成对应的释放地址即可（支持绝对 URL）。

### 4. 公共在线烧录页

如果连仓库都不想部署，可以用官方的 esptool-js 页面手动烧：

1. 打开 <https://espressif.github.io/esptool-js/>
2. 点 **Connect** 选择串口
3. 选择你的 `device.bin`
4. **Flash Address 填 `0x0`**
5. 点 **Program**

注意这种方式没有一键按钮，要手动选文件和填偏移，适合自己临时用。

---

## 实板验证结果

在 **YD-ESP32-S3（ESP32-S3 rev v0.2，16MB Flash）** 上完整验证：

| 项目 | 结果 |
|------|------|
| 编译 | ✅ `[SUCCESS]`，Flash 23.7%，RAM 11.9% |
| 烧录 | ✅ 8,323,072 字节，**Hash of data verified** |
| USB 枚举 | ✅ `VID_303A&PID_80A7` → HidUsb 键盘 |
| 自动输入 | ✅ 打开记事本，4 秒后自动打出配置的密码 + 回车 |
| 串口命令 | ✅ `setpwd` / `getpwd` / `status` / `autologin` 全部正常 |
| 串口改密码 | ✅ `setpwd test1234` → 立即登录打出 `test1234` |
| 稳定性 | ✅ 擦除 coredump 后运行，读回全 `0xFF`，无崩溃 |
| NVS | ✅ 启动打印 `NVS init: ESP_OK` |

---

## 踩坑记录

以下是开发过程中真实遇到并修复的问题，按"症状 → 根因 → 修复"记录，
或许能帮你省下几个小时。

### 1. 自动登录完全不触发：配置根本没被读取

**症状**：固件跑起来了、键盘也枚举了，但插上板子什么都不输入。

**根因**：`include/Globals.h` 里

```c
#define SETTINGS_FILE "/data/settingsSecure.json"
```

但仓库里只有 `settings.json`（`settingsSecure.json` 被 `.gitignore` 排除）。
文件不存在 → `ConfigManager::load()` 直接 `return` → 所有设置都是默认值
→ `autoLogin` 永远是 `false`。

**修复**：`ConfigManager::load()` / `save()` 增加回退。

```c
const char *path = SETTINGS_FILE;
if (!LittleFS.exists(path) && LittleFS.exists(SETTINGS_FILE_FALLBACK)) {
    path = SETTINGS_FILE_FALLBACK;
}
```

### 2. 固件疯狂重启：WiFi 在无网环境 abort

**症状**：整片烧录后键盘毫无反应，像死机。

**排查**：读取 coredump 分区（0x7f0000），发现不是全 `0xFF`，解析出：

```
abort() was called at PC 0x4206b9f4 on core 0     task: loopTask
0x4206b9f4 -> nvs::Page::getSeqNumber()          nvs_page.cpp:999
转储内可见: /networks.  "ChinaNet  sta.ssid
```

**根因**：WiFi 连接流程里 ASSERT 失败 → `abort()` → 重启循环 →
自动登录永远执行不到。

**修复**：关闭 `ENABLE_IMPROV_WIFI_HANDLER` / `ENABLE_WEB_HANDLER` /
`ENABLE_MQTT_HANDLER`。固件从 1.53 MB 降到 0.73 MB，崩溃消失。

> 如果你需要网页配置功能，可以重新打开，但要保证芯片能连上 WiFi。

### 3. NVS 从未初始化

**症状**：串口出现

```
[E][Preferences.cpp:50] begin(): nvs_open failed: NOT_FOUND
```

密码存不进去。

**根因**：整个工程**没有任何地方调用 `nvs_flash_init()`**，
而新烧固件的 NVS 分区是全 `0xFF`，`Preferences::begin()` 必然失败。

**修复**：在 `SerialConfigHandler::init()` 中初始化：

```c
esp_err_t err = nvs_flash_init();
if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    err = nvs_flash_init();
}
```

这很可能也是问题 2 的根源，因为 WiFi 同样依赖 NVS。

### 4. 想用 USB CDC 做配置通道，一直不成功

**症状**：无论怎么调，USB 上只枚举出键盘，CDC 串口始终不出现。

**根因**：`ARDUINO_USB_MODE=1` 时 `Serial` 是 `HWCDC`
（USB-SERIAL-JTAG 外设），和 TinyUSB 是两套独立硬件；
而要让 TinyUSB CDC 出现，接口必须在 `USB.begin()` **之前**注册：

```c
void DeviceHandler::init() {
    USB.begin();        // ❌ 原来先 begin
    keyboard.begin();   //    接口后注册 → 没进描述符
}
```

**更合理的修复**：既然这块板子有独立 CH340 的 UART0 口，
干脆把配置通道放到 UART0 上——它任何时候都在，且不影响原生口当键盘。

```c
Serial0.begin(115200);   // UART0 = CH340 口
```

### 5. Windows 命令行过长导致编译失败

**症状**：

```
xtensa-esp32s3-elf-g++: error: CreateProcess: No such file or directory
```

**根因**：路径太深时，`-I` 参数总和超过 Windows 命令行上限（8191 字符）。

**修复**：把项目放在**短路径**下，例如 `C:\esp\PassTxt`，
不要放在 `C:\Users\xxx\Desktop\...\a\b\c\` 这种深层目录。

### 6. `tools/merge.py` 报"系统找不到指定的文件"

**根因**：脚本里写死调用 `platformio` 命令，
但当 PlatformIO 通过 `python -m platformio` 运行时，PATH 里并没有该命令。

**修复**：

```python
subprocess.run([sys.executable, "-m", "platformio", "run", "--target", "buildfs"], ...)
```

### 7. 中文注释导致编译错乱（.ino 文件）

**症状**：`.ino` 里的 `#define` 莫名其妙失效，报一些不相干的错。

**根因**：Arduino 生成 `.ino.cpp` 时会重编码注释，
非 ASCII 的**行尾注释**偶尔会吞掉后面的换行符，
把下一行代码并进注释里。

**规则**：`.ino` 中所有含中文的注释都写在独立行；
`partitions.csv` 只用英文注释。

### 8. 分块下载器写出损坏文件

多线程写同一个文件用了 `r+b` + `seek`，块之间竞争导致
文件大小正确但内容是 `0x00`（`gzip: Not a gzipped file`）。

**修复**：每块写到独立文件，最后按顺序拼接。

---

## 安全说明

⚠️ **请务必阅读**

1. **密码以明文形式存储**
   - 存在 NVS（`setpwd`）或 `settings.json`（`loginPassword`）
   - 任何拿到这块板子的人都能读出密码

2. **建议措施**
   - 配合 **BitLocker** 等全盘加密使用
   - 不要把板子长期插在公共电脑上
   - 长期不用时执行 `clepwd` 清除密码
   - 不要把填好真实密码的 `settings.json` 提交到 Git

3. **自动输入的风险**
   当前实现是"直接输入"。如果电脑在**桌面状态**下插入板子，
   密码会被打进当前获得焦点的窗口（聊天框、编辑器、网页搜索框……）。

   规避方式：先按 `Win + L` 锁屏，再插板子。

4. **物理安全**
   USB HID 设备天然可以模拟任意按键。请勿在你不信任的电脑上使用，
   也请注意别人可以用同样的方式操作**你的**电脑。

---

## 已知限制

| 限制 | 说明 |
|------|------|
| 仅 ASCII 密码 | USB 键盘只能发送 ASCII 字符，含中文/emoji 的密码会被跳过（固件会在日志告警） |
| 无第二重验证 | 开机 PIN / 指纹 / 人脸仍需人工完成 |
| 无"已在桌面"检测 | 无法判断当前是否处于锁屏界面（USB HID 单向通信），见安全说明 |
| 单 USB 口板子 | 只能当键盘，无法使用串口改密码（除非另接 UART） |
| DuckyScript 阻塞 | `sendKeys()` 内部逐字符 `delay()`，长密码期间其他任务会短暂卡顿 |

---

## 与原版 PassTxt 的差异

### 新增文件

| 文件 | 说明 |
|------|------|
| `src/AutoLoginHandler.h` / `.cpp` | 自动登录状态机 |
| `src/SerialConfigHandler.h` / `.cpp` | UART0 串口命令通道 |

### 修改文件

| 文件 | 改动 |
|------|------|
| `src/ConfigManager.h` | `DeviceSettings` 新增 7 个自动登录字段 |
| `src/ConfigManager.cpp` | 读写新字段；配置文件名回退；自动登录默认值 |
| `src/Main.cpp` | 接入 `AutoLoginHandler` 与 `SerialConfigHandler` |
| `include/Globals.h` | 新增 `SETTINGS_FILE_FALLBACK` |
| `src/DeviceHandler.cpp` | 修正 USB 接口注册顺序 |
| `tools/merge.py` | 用 `sys.executable -m platformio` 调用 |
| `platformio.ini` | 新增两个 `ENABLE_*`，关闭 WiFi/Web/MQTT |
| `data/data/settings.json` | 新增自动登录配置项 |

### 未改动

原项目的 DuckyScript、宏按键、LED、加密、Jiggle 等功能**全部保留**，
代码路径未做修改。

---

## 致谢

- 原始项目：[PassTxt](https://github.com/mdelgert/PassTxt) by [@mdelgert](https://github.com/mdelgert)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32) — Espressif Systems
- [PlatformIO](https://platformio.org/) — 构建工具链

---

## License

本项目继承原项目的 [MIT License](LICENSE)。

---

## 免责声明

本项目仅用于**个人设备**的便利性与学习研究。

请勿用于：
- 未经授权访问他人计算机
- 绕过他人设备的安全防护
- 任何违反当地法律法规的用途

使用者需自行承担全部责任。
