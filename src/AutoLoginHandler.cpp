#ifdef ENABLE_AUTOLOGIN_HANDLER

#include "AutoLoginHandler.h"
#include "Globals.h"
#include "DeviceHandler.h"
#include "SerialConfigHandler.h"

// 直接用 TinyUSB 的 HID 就绪查询：tud_hid_n_ready(0)
// （Arduino 的 USBHID::ready() 是 private hid 成员里的方法，外部拿不到）
#include "class/hid/hid_device.h"

// =============================================================================
// 时序参数（毫秒）
// =============================================================================
// 注意：起始等待不再是一个固定的大延迟。
// 状态机每轮轮询 TinyUSB 的 tud_hid_n_ready()，**一旦电脑把 HID 接口配置好就
// 立刻开始打字**，而不必死等 4~5 秒。
// 只有在 USB 一直不就绪（比如插在只供电的充电器上）时，
// 才会退回到 AUTOLOGIN_FALLBACK_DELAY 继续走流程。

// USB 就绪后，再等这么久让它稳定（Windows 挂上驱动后偶有极短的抖动）
static const uint32_t AUTOLOGIN_USB_SETTLE = 150;

// USB 一直没就绪时的兜底等待：到点仍然尝试打字，避免永远卡住
static const uint32_t AUTOLOGIN_FALLBACK_DELAY = 8000;

// 没有启用 USB 就绪检测时的默认等待（settings.autoLoginDelay <= 0 且强制模式）
static const uint32_t AUTOLOGIN_BOOT_DELAY = 5000;

// 按下 NumLock 叫醒屏幕后，等登录界面画出来
static const uint32_t AUTOLOGIN_WAKE_DELAY = 2500;

// 输入用户名后，等系统把光标移到密码框
static const uint32_t AUTOLOGIN_TAB_DELAY = 400;

// 打完密码后等系统处理，再按回车
static const uint32_t AUTOLOGIN_ENTER_DELAY = 300;

// 说明：按键之间的间隔由 DeviceHandler::keyPressDelay 控制（默认 20ms，
// 可通过 `hid delay <ms>` 命令调整），这里不重复定义。

// =============================================================================
// 状态机
// =============================================================================
enum AutoLoginState : uint8_t
{
    AL_IDLE = 0,       // 什么都不做
    AL_WAIT_START,     // 等待上电延时结束
    AL_WAKE,           // 发 NumLock 叫醒屏幕
    AL_WAIT_SCREEN,    // 等登录界面就绪
    AL_TYPE_USER,      // 输入用户名
    AL_TYPE_PASS,      // 输入密码
    AL_ENTER,          // 按回车
    AL_DONE            // 完成
};

static AutoLoginState state_ = AL_IDLE;
static uint32_t stateSince_ = 0;

// 本次登录要用的凭据（在 startStateMachine 时快照，避免中途被改）
static String loginUser_;
static String loginPass_;
static bool wantSendUser_ = false;
static bool wantWake_ = false;
static bool wantEnter_ = true;
static uint32_t bootDelayMs_ = AUTOLOGIN_BOOT_DELAY;

// 输入用户名/按 TAB 之后，等多久再去打密码
static uint32_t userTypedAt_ = 0;
static bool userTyped_ = false;

// USB 就绪检测
static bool usbReadySeen_ = false;    // 是否曾经看到过 USB 就绪
static uint32_t usbReadyAt_ = 0;      // 首次就绪的时间点
static bool waitUsbReady_ = true;     // 本次是否启用"等 USB 就绪"

// 防止重复触发（一次插拔只登录一次）
static bool attemptedThisBoot_ = false;

// -----------------------------------------------------------------------------
// 工具
// -----------------------------------------------------------------------------
static void alSetState(AutoLoginState s)
{
    state_ = s;
    stateSince_ = millis();
    debugI("AutoLogin: state -> %d", (int)s);
}

// USB HID 接口是否已经就绪（电脑完成枚举、驱动挂上）
static bool usbHidReady()
{
    return tud_hid_n_ready(0);
}

// -----------------------------------------------------------------------------
// 对外接口
// -----------------------------------------------------------------------------
void AutoLoginHandler::init()
{
    registerCommands();

    // 读取配置并决定是否自动开始
    if (!settings.device.autoLogin)
    {
        debugI("AutoLogin: disabled in settings (device.autoLogin = false)");
        return;
    }

    // autoLoginDelay 的新语义：
    //   > 0 : 仍按固定延迟等待（想手动加延时的人可以设它）
    //   = 0 : 立刻开始，只等 USB 就绪（推荐，插上即打）
    //   < 0 : 同 0，但强制等 USB 就绪、不启用兜底超时
    //
    // 默认配置是 0，所以插上电脑后一旦 HID 就绪就立刻输入，
    // 不再固定傻等 4 秒。
    const int cfgDelay = settings.device.autoLoginDelay;
    if (cfgDelay > 0)
    {
        // 用户显式要求固定延迟：不做 USB 轮询，纯计时
        waitUsbReady_ = false;
        bootDelayMs_ = (uint32_t)cfgDelay;
        debugI("AutoLogin: fixed delay mode, will type after %u ms", (unsigned)bootDelayMs_);
    }
    else
    {
        // 等 USB 就绪模式：一就绪 + 短暂稳定期就开始
        waitUsbReady_ = true;
        bootDelayMs_ = AUTOLOGIN_USB_SETTLE;
        debugI("AutoLogin: waiting for USB HID ready, then typing after %u ms",
               (unsigned)bootDelayMs_);
    }

    startStateMachine();
}

void AutoLoginHandler::trigger()
{
    debugI("AutoLogin: manual trigger");
    attemptedThisBoot_ = false;
    // 手动触发时 USB 一般已经就绪，短暂等待即可
    waitUsbReady_ = true;
    bootDelayMs_ = AUTOLOGIN_USB_SETTLE;
    startStateMachine();
}

void AutoLoginHandler::cancel()
{
    if (state_ != AL_IDLE && state_ != AL_DONE)
    {
        debugI("AutoLogin: cancelled by user");
    }
    alSetState(AL_IDLE);
    // 确保没有键卡在按下状态
    DeviceHandler::getKeyboard().releaseAll();
}

bool AutoLoginHandler::busy()
{
    return state_ != AL_IDLE && state_ != AL_DONE;
}

// -----------------------------------------------------------------------------
// 采集凭据
// -----------------------------------------------------------------------------
void AutoLoginHandler::startStateMachine()
{
    if (attemptedThisBoot_)
    {
        debugW("AutoLogin: already attempted this boot, ignoring");
        return;
    }
    attemptedThisBoot_ = true;

    // 凭据优先级（高 -> 低）：
    //   1) 串口命令 setpwd 存进 NVS 的密码（改密码不用重烧固件，最方便）
    //   2) settings 里的 loginPassword（专门为自动登录准备）
    //   3) settings 里原有的 userPassword（web 后台账号密码，兜底）
    String serialPwd = SerialConfigHandler::getSavedPassword();

    loginUser_ = settings.device.loginUserName.length() > 0
                     ? settings.device.loginUserName
                     : settings.device.userName;

    if (serialPwd.length() > 0)
    {
        loginPass_ = serialPwd;
        debugI("AutoLogin: using password from serial config (NVS)");
    }
    else if (settings.device.loginPassword.length() > 0)
    {
        loginPass_ = settings.device.loginPassword;
    }
    else
    {
        loginPass_ = settings.device.userPassword;
    }

    wantSendUser_ = settings.device.loginSendUserName;
    wantWake_ = settings.device.loginWakeScreen;
    wantEnter_ = settings.device.loginEnter;

    if (loginPass_.isEmpty())
    {
        debugW("AutoLogin: password is empty, will only press ENTER");
    }

    // 键盘只能发 ASCII 可打印字符。含中文等非 ASCII 字符的密码会被静默丢弃，
    // 导致输错密码却看不出原因，所以这里提前告警。
    int bad = 0;
    for (size_t i = 0; i < loginPass_.length(); i++)
    {
        uint8_t c = (uint8_t)loginPass_[i];
        if (c < 0x20 || c > 0x7E)
        {
            bad++;
        }
    }
    if (bad > 0)
    {
        debugE("AutoLogin: password contains %d non-ASCII character(s); "
               "the USB keyboard can only type ASCII, those will be skipped",
               bad);
    }
    if (loginPass_.length() > 120)
    {
        debugW("AutoLogin: password is %u chars, typing it will take a while",
               (unsigned)loginPass_.length());
    }

    debugI("AutoLogin: user=%s  sendUser=%d  wake=%d  enter=%d  passLen=%u",
           loginUser_.c_str(), wantSendUser_, wantWake_, wantEnter_,
           (unsigned)loginPass_.length());

    userTyped_ = false;

    // 每次启动流程都重新做一次 USB 就绪检测
    usbReadySeen_ = false;
    usbReadyAt_ = 0;

    alSetState(AL_WAIT_START);
}

// -----------------------------------------------------------------------------
// 推进状态机（在 loop 里调用，绝不阻塞）
// -----------------------------------------------------------------------------
void AutoLoginHandler::stepStateMachine()
{
    const uint32_t elapsed = millis() - stateSince_;

    switch (state_)
    {
    case AL_WAIT_START:
    {
        // ---------------------------------------------------------------
        // 等 USB 就绪（而不是固定傻等）
        // ---------------------------------------------------------------
        // 电脑插入后要枚举设备、挂 HID 驱动，这段时间内发的按键会丢。
        // 与其固定等 4~5 秒，不如每轮问一下 TinyUSB：
        //   tud_hid_n_ready() == true  表示主机已经把配置描述符下发、
        //   接口处于"可以接收报告"的状态——这时打字才不会丢。
        if (waitUsbReady_ && !usbReadySeen_)
        {
            if (usbHidReady())
            {
                usbReadySeen_ = true;
                usbReadyAt_ = millis();
                debugI("AutoLogin: USB HID ready at %u ms", (unsigned)usbReadyAt_);
                // 同时打到 UART0：本项目默认关闭 RemoteDebug，debugI 是空宏，
                // 只有直接 write 才能在串口上看到实际时刻。
                Serial0.printf("[AutoLogin] USB HID ready at %u ms\r\n", (unsigned)usbReadyAt_);
            }
            else if (elapsed >= AUTOLOGIN_FALLBACK_DELAY)
            {
                // 一直不就绪（可能插在只供电的口上），照样往下走，
                // 免得永远卡在这里。
                debugW("AutoLogin: USB not ready after %u ms, continuing anyway",
                       (unsigned)AUTOLOGIN_FALLBACK_DELAY);
                usbReadySeen_ = true;
                usbReadyAt_ = millis();
            }
            break;
        }

        // 这里 elapsed 从「就绪时刻」算起，或固定延迟模式下从进状态算起
        const uint32_t waitBase = usbReadySeen_ ? (millis() - usbReadyAt_) : elapsed;
        if (waitBase >= bootDelayMs_)
        {
            Serial0.printf("[AutoLogin] start typing at %u ms (since boot)\r\n", (unsigned)millis());
            if (wantWake_)
            {
                alSetState(AL_WAKE);
            }
            else
            {
                alSetState(AL_TYPE_USER);
            }
        }
        break;
    }

    case AL_WAKE:
        // 一次 NumLock 轻按：点亮屏幕并进入密码输入状态。
        // 用 tapKey 走项目已有的按键映射，保证按下/松开成对出现。
        debugI("AutoLogin: wake screen (NUMLOCK)");
        DeviceHandler::tapKey("NUMLOCK");
        alSetState(AL_WAIT_SCREEN);
        break;

    case AL_WAIT_SCREEN:
        if (elapsed >= AUTOLOGIN_WAKE_DELAY)
        {
            alSetState(AL_TYPE_USER);
        }
        break;

    case AL_TYPE_USER:
        if (wantSendUser_ && loginUser_.length() > 0)
        {
            debugI("AutoLogin: typing user name");
            DeviceHandler::sendKeys(loginUser_);
            DeviceHandler::tapKey("TAB");
            // 记下时间戳：下一步要等光标跳到密码框，不能用状态切换时间
            // （因为同一次调用里会立刻切到 AL_TYPE_PASS，elapsed 恒为 0）
            userTypedAt_ = millis();
            userTyped_ = true;
        }
        alSetState(AL_TYPE_PASS);
        break;

    case AL_TYPE_PASS:
        // 刚按过 TAB 的话，等光标移动到密码框再继续
        if (userTyped_ && (millis() - userTypedAt_) < AUTOLOGIN_TAB_DELAY)
        {
            break;
        }
        if (loginPass_.length() > 0)
        {
            debugI("AutoLogin: typing password (%u chars)", (unsigned)loginPass_.length());
            DeviceHandler::sendKeys(loginPass_);
        }
        alSetState(AL_ENTER);
        break;

    case AL_ENTER:
        if (elapsed >= AUTOLOGIN_ENTER_DELAY)
        {
            if (wantEnter_)
            {
                debugI("AutoLogin: pressing ENTER");
                DeviceHandler::tapKey("ENTER");
            }
            DeviceHandler::getKeyboard().releaseAll();
            alSetState(AL_DONE);
            debugI("AutoLogin: finished");
        }
        break;

    case AL_DONE:
    case AL_IDLE:
    default:
        break;
    }
}

void AutoLoginHandler::loop()
{
    if (state_ == AL_IDLE || state_ == AL_DONE)
    {
        return;
    }
    stepStateMachine();
}

// -----------------------------------------------------------------------------
// 命令注册：串口 / Web 终端里可以手动重来一次
// -----------------------------------------------------------------------------
void AutoLoginHandler::registerCommands()
{
    CommandHandler::registerCommand("autologin", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (args.isEmpty() || CommandHandler::equalsIgnoreCase(args, "now"))
        {
            AutoLoginHandler::trigger();
        }
        else if (CommandHandler::equalsIgnoreCase(args, "cancel"))
        {
            AutoLoginHandler::cancel();
        }
        else if (CommandHandler::equalsIgnoreCase(args, "status"))
        {
            debugI("AutoLogin: attempted=%d busy=%d user=%s passLen=%u",
                   attemptedThisBoot_, AutoLoginHandler::busy(),
                   loginUser_.c_str(),
                   (unsigned)loginPass_.length());
        }
        else
        {
            debugW("Unknown autologin argument: %s (use: now | cancel | status)", args.c_str());
        } },
                                    "Auto-login to Windows on boot.\n"
                                    "  autologin now     - run the login sequence immediately\n"
                                    "  autologin cancel  - stop a pending login\n"
                                    "  autologin status  - show current settings");
}

#endif // ENABLE_AUTOLOGIN_HANDLER
