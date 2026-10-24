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
  CronHandler::init();
  SystemMonitor::init();
  DownloadHandler::init();
  AesHandler::init();
  CryptoHandler::init();
  TimeHandler::init(settings.device.timezone);
  JiggleHandler::init();
  BluetoothHandler::init();
  MqttHandler::init();

  // 串口配置通道：插上电脑会多一个 COM 口，发 setpwd <密码> 即可改密码，
  // 不用重新烧录固件。必须在 DeviceHandler::init()（USB.begin）之后。
  SerialConfigHandler::init();

  // 自动登录必须在 DeviceHandler::init() 之后：
  // 那时 USB.begin() 和 keyboard.begin() 才把键盘挂上去。
  AutoLoginHandler::init();

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