#ifdef ENABLE_AUTOLOGIN_HANDLER

#include "AutoLoginHandler.h"
#include "Globals.h"
#include "DeviceHandler.h"
#include "SerialConfigHandler.h"

// =============================================================================
// 时序参数（毫秒）
// =============================================================================
// 这些值是"保守值"，宁可慢一点也不要丢字符。
// 如果发现密码输错/丢字符，把下方常量调大即可。

// 上电后等多久才开始：给 Windows 装 HID 驱动 + 用户看清锁定界面
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

    bootDelayMs_ = settings.device.autoLoginDelay > 0
                       ? (uint32_t)settings.device.autoLoginDelay
                       : AUTOLOGIN_BOOT_DELAY;

    debugI("AutoLogin: enabled, will type after %u ms", (unsigned)bootDelayMs_);
    startStateMachine();
}

void AutoLoginHandler::trigger()
{
    debugI("AutoLogin: manual trigger");
    attemptedThisBoot_ = false;
    bootDelayMs_ = 1500; // 手动触发时不用等那么久
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
        if (elapsed >= bootDelayMs_)
        {
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
