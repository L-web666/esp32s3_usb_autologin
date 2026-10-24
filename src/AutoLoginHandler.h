#pragma once

#ifdef ENABLE_AUTOLOGIN_HANDLER

#include <Arduino.h>

// AutoLoginHandler
// =============================================================================
// 在板子插入电脑（上电）后，自动把 Windows 账号密码打进去并回车，完成解锁登录。
//
// 为什么需要它：
//   电脑锁屏/显示登录界面时，插上板子后要么是锁屏界面、要么是桌面。
//   想在锁屏界面自动输入密码，必须在键盘可用之后、由板子自己发起一次
//   "叫醒屏幕 + 输入密码 + 回车"的动作。本类负责这个时序。
//
// 两种凭据来源（按顺序尝试，前者为空时用后者）：
//   1) settings.device.loginUserName / loginPassword
//      —— 专门为自动登录准备，明文存在 settingsSecure.json 里
//   2) settings.device.userName / userPassword
//      —— 项目原有的登录凭据（web 后台账号），复用它的密码
//
// 使用方法：
//   在 platformio.ini 里打开 -D ENABLE_AUTOLOGIN_HANDLER
//   然后在 settings.json 的 device 段落里加：
//       "autoLogin": true,
//       "autoLoginDelay": 5000,
//       "loginUserName": "你的Windows账号",
//       "loginPassword": "你的Windows密码",
//       "loginSendUserName": true,
//       "loginWakeScreen": true,
//       "loginEnter": true
//
// 手动触发：串口/命令台发送 `autologin` 会立刻重新执行一次登录。
// =============================================================================
class AutoLoginHandler
{
public:
    static void init();   // 注册命令
    static void loop();   // 非阻塞推进状态机

    // 立即（重置计时后）执行一次自动登录
    static void trigger();

    // 取消正在等待/进行的自动登录
    static void cancel();

    // 是否还在等待或执行中
    static bool busy();

private:
    static void registerCommands();
    static void startStateMachine();
    static void stepStateMachine();
};

#else // 未启用时的空实现，避免调用处到处写 #ifdef

class AutoLoginHandler
{
public:
    static void init() {}
    static void loop() {}
    static void trigger() {}
    static void cancel() {}
    static bool busy() { return false; }
};

#endif // ENABLE_AUTOLOGIN_HANDLER
