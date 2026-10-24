//SystemMonitor.h

#pragma once

#ifdef ENABLE_SYSTEM_MONITOR

class SystemMonitor {
private:
    static void systemMonitorTask(void *parameter);
    static void handleSecondCheck();
    static void handleMinuteCheck();
    static void handleFiveMinuteCheck();
    static void handleTenMinuteCheck();

public:
    static void init();
};

#else

class SystemMonitor { 
public:
    static void init() {} // No-op
};

#endif // ENABLE_SYSTEM_MONITOR