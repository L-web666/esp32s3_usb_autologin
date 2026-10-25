#include "Globals.h"
#include "TemplateHandler.h"
#include "RemoteDebugHandler.h"
#include "GfxHandler.h"
#include "LedHandler.h"
#include "ButtonHandler.h"
#include "WebHandler.h"
#include "BluetoothHandler.h"
#include "MqttHandler.h"
#include "OTAHandler.h"
#include "TimeHandler.h"
#include "CommandHandler.h"
#include "DeviceHandler.h"
#include "ScriptHandler.h"
#include "CronHandler.h"
#include "SystemMonitor.h"
#include "DownloadHandler.h"
#include "AesHandler.h"
#include "CryptoHandler.h"
#include "JiggleHandler.h"
#include "ImprovWiFiHandler.h"
#include "LittleFsHandler.h"
#include "DuckyScriptHandler.h"
#include "AutoLoginHandler.h"
#include "SerialConfigHandler.h"

void setup()
{
  RemoteDebugHandler::init();
  LittleFsHandler::init();
  ScriptHandler::init();
  DuckyScriptHandler::init();
  ConfigManager::init();
  GfxHandler::init();
  ImprovWiFiHandler::init();
  WebHandler::init();
  LedHandler::init();
  ButtonHandler::init();
  OTAHandler::init();
  CommandHandler::init();
  DeviceHandler::init();

  // ---------------------------------------------------------------------------
  // 自动登录 / 串口通道尽量提前初始化
  // ---------------------------------------------------------------------------
  // 依赖关系：需要 ConfigManager::init()（读设置）和 DeviceHandler::init()
  // （USB.begin + keyboard.begin）已经跑完，所以只能放在这里。
  //
  // 但必须排在其余无关模块之前：Cron / SystemMonitor / Download / Aes /
  // Crypto / Jiggle / Bluetooth / Mqtt 等初始化完才轮到这里的话，
  // USB 枚举和"就绪检测"会被一起推迟，插上后要多等一两秒才打字。
  // 放到 DeviceHandler 之后立刻执行，能让 USB 一就绪就开始登录。
  AutoLoginHandler::init();
  SerialConfigHandler::init();

  CronHandler::init();
  SystemMonitor::init();
  DownloadHandler::init();
  AesHandler::init();
  CryptoHandler::init();
  TimeHandler::init(settings.device.timezone);
  JiggleHandler::init();
  BluetoothHandler::init();
  MqttHandler::init();

  CommandHandler::handleCommand(settings.device.bootCommand);
  
  //GfxHandler::printMessage(SOFTWARE_VERSION);
  //LedHandler::setDefaultBrightness(50);
  //LedHandler::setColorByName("purple");
  //LedHandler::setColorByName("green");
  //LedHandler::setColorByName("black"); //off
}

void loop()
{
  RemoteDebugHandler::loop();
  ScriptHandler::loop();
  GfxHandler::loop();
  ImprovWiFiHandler::loop();
  WebHandler::loop();
  ButtonHandler::loop();
  OTAHandler::loop();
  DeviceHandler::loop();
  CronHandler::loop();
  JiggleHandler::loop();
  BluetoothHandler::loop();
  MqttHandler::loop();
  AutoLoginHandler::loop();
  SerialConfigHandler::loop();
}