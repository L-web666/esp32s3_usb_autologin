#ifdef ENABLE_SYSTEM_MONITOR

#include "SystemMonitor.h"
#include "Globals.h"
#include "TimeHandler.h"
#include "GfxHandler.h"
#include "CronHandler.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Timing intervals (in milliseconds)
constexpr unsigned long SECOND_CHECK_INTERVAL = 1000;
constexpr unsigned long MINUTE_CHECK_INTERVAL = 60000;
constexpr unsigned long FIVE_MINUTE_CHECK_INTERVAL = 300000;
constexpr unsigned long TEN_MINUTE_CHECK_INTERVAL = 600000;

// Task handle for the SystemMonitor task
TaskHandle_t systemMonitorTaskHandle = nullptr;

// Function prototypes
void SystemMonitor::init()
{
  // Create the SystemMonitor task
  xTaskCreatePinnedToCore(
      systemMonitorTask,        // Task function
      "SystemMonitor",          // Name of the task
      4096,                     // Stack size
      nullptr,                  // Parameter
      1,                        // Priority
      &systemMonitorTaskHandle, // Task handle
      1                         // Core
  );
}

// SystemMonitor task
void SystemMonitor::systemMonitorTask(void *parameter)
{
  unsigned long lastCheckTimes[] = {0, 0, 0, 0}; // Index: 0-Second, 1-Minute, 2-FiveMinute, 3-TenMinute
  constexpr unsigned long intervals[] = {
      SECOND_CHECK_INTERVAL,
      MINUTE_CHECK_INTERVAL,
      FIVE_MINUTE_CHECK_INTERVAL,
      TEN_MINUTE_CHECK_INTERVAL};

  while (true)
  {
    unsigned long currentMillis = millis();

    for (int i = 0; i < 4; i++)
    {
      if (currentMillis - lastCheckTimes[i] >= intervals[i])
      {
        lastCheckTimes[i] = currentMillis;

        // Call the specific handler for each interval
        switch (i)
        {
        case 0:
          handleSecondCheck();
          break;
        case 1:
          handleMinuteCheck();
          break;
        case 2:
          handleFiveMinuteCheck();
          break;
        case 3:
          handleTenMinuteCheck();
          break;
        }
      }
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // Sleep for 100 ms
  }
}

// Define individual handlers for each interval
void SystemMonitor::handleSecondCheck()
{
  //debugV("SystemMonitor: Running second-specific task.");
  settings.device.upTime++;
  CronHandler::executeJobs();
  TimeHandler::loop();

  // Increment boot count and update boot time only once after time is synced
  static bool bootUpdated = false; // Tracks if the update has been performed
  if (!bootUpdated && TimeHandler::getTimeSyncStatus())
  {
    settings.device.bootCount++;
    settings.device.bootTime = time(nullptr);
    ConfigManager::save();
    bootUpdated = true; // Mark as updated
    //debugV("Boot count and boot time updated.");
  }
}

void SystemMonitor::handleMinuteCheck()
{
  //debugV("SystemMonitor: Running minute-specific task.");
}

void SystemMonitor::handleFiveMinuteCheck()
{
  //debugV("SystemMonitor: Running five-minute-specific task.");
}

void SystemMonitor::handleTenMinuteCheck()
{
  //debugV("SystemMonitor: Running ten-minute-specific task.");
  TimeHandler::syncTime();
}

#endif // ENABLE_SYSTEM_MONITOR
