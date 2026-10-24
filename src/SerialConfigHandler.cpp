#ifdef ENABLE_SERIALCONFIG_HANDLER

#include "SerialConfigHandler.h"
#include "Globals.h"
#include "AutoLoginHandler.h"

#include <Preferences.h>
#include <nvs_flash.h>
#include <stdarg.h>

// -----------------------------------------------------------------------------
// 用哪个串口
// -----------------------------------------------------------------------------
// platformio.ini 里是：
//     -DARDUINO_USB_MODE=0        -> TinyUSB（可与 HID 组成复合设备）
//     -DARDUINO_USB_CDC_ON_BOOT=1 -> Serial 就是 USBCDC
// 所以这里直接用全局 Serial：它和 DeviceHandler 里的 HID 键盘共用同一个
// TinyUSB 设备，枚举成一个设备的「键盘 + 串口」两个接口。
//
// 注意：不要自己再 `USBCDC x(0)` —— 会和 Serial 抢同一个接口号，
// 导致 CDC 接口不被注册（实测只剩键盘、没有串口）。


// NVS 命名空间与键
static const char *NVS_NS = "autologin";
static const char *NVS_KEY_PWD = "pwd";

// 命令行缓冲
static const size_t CMDLINE_MAX = 160;
static char lineBuf[CMDLINE_MAX];
static size_t lineLen = 0;

static Preferences prefs;

// -----------------------------------------------------------------------------
// 存储
// -----------------------------------------------------------------------------
String SerialConfigHandler::getSavedPassword()
{
    // NVS 没初始化成功时 begin() 会返回 false，此时返回空密码，
    // 让上层回退到 settings 里的密码，避免整个登录流程被打断。
    if (!prefs.begin(NVS_NS, true))
    {
        return String();
    }
    String pwd = prefs.getString(NVS_KEY_PWD, "");
    prefs.end();
    return pwd;
}

static bool savePassword(const String &pwd)
{
    if (!prefs.begin(NVS_NS, false))
    {
        return false;
    }
    bool ok = true;
    if (pwd.length() == 0)
    {
        prefs.remove(NVS_KEY_PWD);
    }
    else
    {
        ok = (prefs.putString(NVS_KEY_PWD, pwd) == pwd.length());
    }
    prefs.end();
    return ok;
}

bool SerialConfigHandler::connected()
{
    // 不依赖串口对象的 bool 转换（Stream/HardwareSerial 没有）。
    // 只要有字节可读、或最近收到过命令，就认为通道可用。
    return lineLen > 0 || Serial0.available() > 0;
}

// -----------------------------------------------------------------------------
// 输出
// -----------------------------------------------------------------------------
static void reply(const char *fmt, ...)
{
    char buf[200];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n < 0)
    {
        return;
    }
    Serial0.print(buf);
    Serial0.print("\r\n");
}

// -----------------------------------------------------------------------------
// 命令
// -----------------------------------------------------------------------------
static void printHelp()
{
    reply("可用命令：");
    reply("  setpwd <密码>   保存密码并立即登录一次");
    reply("  getpwd          查看是否已设置（只显示长度）");
    reply("  clepwd          清除已保存的密码");
    reply("  autologin       立即执行一次自动登录");
    reply("  status          当前状态");
    reply("  help            本帮助");
}

static void handleLine(char *line)
{
    // 去掉首尾空白
    while (*line == ' ' || *line == '\t')
    {
        ++line;
    }
    size_t n = strlen(line);
    while (n > 0 && (line[n - 1] == ' ' || line[n - 1] == '\t'))
    {
        line[--n] = 0;
    }
    if (n == 0)
    {
        return;
    }

    if (!strncasecmp(line, "setpwd", 6) && (line[6] == ' ' || line[6] == 0))
    {
        const char *pwd = line + 6;
        while (*pwd == ' ' || *pwd == '\t')
        {
            ++pwd;
        }
        if (*pwd == 0)
        {
            reply("用法: setpwd <密码>   （密码不能为空）");
            return;
        }
        if (!savePassword(String(pwd)))
        {
            reply("保存失败：NVS 不可用");
            return;
        }
        reply("OK 已保存密码（长度 %u），并开始登录……", (unsigned)strlen(pwd));
        AutoLoginHandler::trigger();
        return;
    }

    if (!strncasecmp(line, "getpwd", 6))
    {
        String pwd = SerialConfigHandler::getSavedPassword();
        if (pwd.length() == 0)
        {
            reply("当前未设置密码（自动登录只会按回车）");
        }
        else
        {
            reply("已设置密码，长度 %u（出于安全不回显明文）", (unsigned)pwd.length());
        }
        return;
    }

    if (!strncasecmp(line, "clepwd", 6) || !strncasecmp(line, "cleapwd", 7))
    {
        savePassword(String());
        reply("OK 已清除保存的密码");
        return;
    }
    if (!strncasecmp(line, "autologin", 9))
    {
        reply("OK 立即执行自动登录");
        AutoLoginHandler::trigger();
        return;
    }

    if (!strncasecmp(line, "status", 6))
    {
        String pwd = SerialConfigHandler::getSavedPassword();
        reply("串口: %s", SerialConfigHandler::connected() ? "已连接" : "未连接");
        reply("已保存密码长度: %u", (unsigned)pwd.length());
        reply("autoLogin(设置): %d", (int)settings.device.autoLogin);
        reply("登录中: %s", AutoLoginHandler::busy() ? "是" : "否");
        return;
    }

    if (!strncasecmp(line, "help", 4) || line[0] == '?')
    {
        printHelp();
        return;
    }

    reply("未知命令: %s", line);
    reply("输入 help 查看可用命令");
}

// -----------------------------------------------------------------------------
// 初始化 / 轮询
// -----------------------------------------------------------------------------
void SerialConfigHandler::init()
{
    // ------------------------------------------------------------------
    // 初始化 NVS
    // ------------------------------------------------------------------
    // 整个工程原本没有任何地方调用 nvs_flash_init()，而刚烧完固件的
    // NVS 分区是全 0xFF。结果是 Preferences::begin() 直接失败
    // （串口里能看到 "nvs_open failed: NOT_FOUND"），密码根本存不进去；
    // 原项目里 WiFi 相关代码也依赖 NVS，很可能就是它 abort 崩溃的原因。
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // 分区满或版本不匹配：擦掉重来（此处只存密码，丢弃无妨）
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    // ------------------------------------------------------------------
    // 串口通道走 UART0（GPIO43/44），也就是板载 CH340 那个 Type-C 口
    // ------------------------------------------------------------------
    // 为什么不用 USB CDC：这块板子有两个 Type-C，
    //   - 原生口：固件一接管就没有串口了，改密码/看日志都没法用；
    //   - CH340 口：接的是 UART0，任何时候都在，且不影响原生口当键盘。
    // 所以把配置通道放在 UART0 上最实用。
    Serial0.begin(115200);
    delay(50);

    // 直接用 Serial0 打印，不走 debugI —— 本项目默认关闭了 RemoteDebug，
    // debugI 是空宏，什么都看不到。
    Serial0.print("\r\n=== PassTxt AutoLogin (UART0 config channel) ===\r\n");
    Serial0.printf("NVS init: %s\r\n", esp_err_to_name(err));
    Serial0.print("Commands: setpwd / getpwd / clepwd / autologin / status / help\r\n");
}

void SerialConfigHandler::loop()
{
    while (Serial0.available())
    {
        int c = Serial0.read();
        if (c < 0)
        {
            break;
        }
        if (c == '\r' || c == '\n')
        {
            if (lineLen > 0)
            {
                lineBuf[lineLen] = 0;
                handleLine(lineBuf);
                lineLen = 0;
            }
        }
        else if (c == 8 || c == 127)
        {
            if (lineLen > 0)
            {
                --lineLen;
            }
        }
        else if (lineLen < CMDLINE_MAX - 1)
        {
            lineBuf[lineLen++] = (char)c;
        }
    }
}

#endif // ENABLE_SERIALCONFIG_HANDLER
