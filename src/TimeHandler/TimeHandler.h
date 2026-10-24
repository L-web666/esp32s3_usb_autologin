#pragma once

#ifdef ENABLE_TIME_HANDLER

#include "Globals.h"
#include "Timezones.h"
#include <time.h>

/**
 * TimeHandler is responsible for setting up NTP time synchronization
 * and providing access to the current local time.
 */
class TimeHandler
{
private:
    static bool parsePosixOffsets(const char* posixStr, long* gmtOffset, int* daylightOffset);
    
public:
    static bool getTimeSyncStatus();
    static const char* getCurrentTimezone();
    static void init(String timezone);
    static void loop();
    static void syncTime();
    static long getLinuxTime();
    static String formatDateTime(const char *format, time_t timestamp = 0);
    static void logAllDateTimeFormats();
};

#else

/**
 * No-op implementation if ENABLE_TIME_HANDLER is not defined.
 * This allows other code to compile without changing references.
 */
class TimeHandler
{ 
public:
    static bool getTimeSyncStatus() { return false; }
    static const char* getCurrentTimezone() { return ""; }
    static void init(String timezone) {}
    static void loop() {}
    static void syncTime() {}
    static long getLinuxTime() { return 0; }
    static String formatDateTime(const char* format, time_t timestamp = 0) { return ""; }
    static void logAllDateTimeFormats() {}
};

#endif // ENABLE_TIME_HANDLER
