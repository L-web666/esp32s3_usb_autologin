// JiggleHandler.h

#pragma once

#ifdef ENABLE_JIGGLE_HANDLER

#include "Globals.h"

class JiggleHandler
{
private:
    static NonBlockingTimer jiggleTimer;
    static int jiggleInterval;
    static int jiggleAmount;
    static bool showCountdown;
    static bool jiggleEnabled;
    static void performJiggle();
    static void registerCommands();
    
public:
    static void init();
    static void loop();
};

#else

class JiggleHandler
{
public:
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_JIGGLE_HANDLER
