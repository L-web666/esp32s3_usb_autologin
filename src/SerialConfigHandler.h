#pragma once

#ifdef ENABLE_SERIALCONFIG_HANDLER

#include <Arduino.h>

// SerialConfigHandler
// =============================================================================
// 提供一个**不需要重新烧录固件**就能改密码的串口命令通道。
//
// 为什么要它：
//   工程里配置存在 LittleFS 的 settings.json 里，而那个文件是编译进
//   文件系统镜像的 —— 改一个字段就得重建 + 重烧整个 littlefs，
//   非常不方便。这里额外开一个 USB CDC 串口，插上电脑后直接用任意
//   串口工具发一行命令就能改密码，改完存到 NVS，掉电不丢。
//
// 与键盘共存：
//   固件用的是 ARDUINO_USB_MODE=1（USB-SERIAL-JTAG 做 Serial），
//   而 TinyUSB 的 CDC 是另一套独立外设，两者可以和 HID 键盘同时存在。
//   所以插上后会同时看到：键盘 + 一个 COM 口。
//
// 命令（波特率随意，CDC 不看波特率）：
//   setpwd <你的Windows密码>     保存密码并立即用它登录一次
//   getpwd                       显示当前是否已设置密码（只显示长度，不回显明文）
//   clepwd / cleapwd             清除已保存的密码
//   autologin                    立即执行一次自动登录
//   status                       查看当前状态
//   help                         帮助
//
// 存储位置：NVS 命名空间 "autologin"，键 "pwd"。
// =============================================================================
class SerialConfigHandler
{
public:
    static void init();
    static void loop();

    // 读取串口配置里保存的密码；没有则返回空字符串
    static String getSavedPassword();

    // 是否有可用的串口通道（电脑已打开串口）
    static bool connected();
};

#else

class SerialConfigHandler
{
public:
    static void init() {}
    static void loop() {}
    static String getSavedPassword() { return String(); }
    static bool connected() { return false; }
};

#endif // ENABLE_SERIALCONFIG_HANDLER
