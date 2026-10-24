#pragma once

#ifdef ENABLE_REMOTE_DEBUG_HANDLER

#include <RemoteDebug.h>

// Declare the RemoteDebug instance
extern RemoteDebug Debug;

class RemoteDebugHandler {
public:
    static void loop();
    static void startNetwork();
    static void init();

private:
    static void handleCustomCommands();
};

#else

// No-op implementation of RemoteDebugHandler
#define debugA(...)
#define debugP(...)
#define debugV(...)
#define debugD(...)
#define debugI(...)
#define debugW(...)
#define debugE(...)
class RemoteDebugHandler {
public:
    static void loop() {}           // No-op
    static void startNetwork() {}   // No-op
    static void init() {}           // No-op
};

#endif // ENABLE_REMOTE_DEBUG_HANDLER