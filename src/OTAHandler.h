#pragma once

#ifdef ENABLE_OTA_HANDLER

class OTAHandler
{
private:
    static void setupOTA(); // Private function for setting up OTA

public:
    static void init();
    static void loop();
    static void triggerUpdate();
};

#else

class OTAHandler {
public: // No-op implementation of OTAHandler
    static void init() {} // No-op
    static void loop() {} // No-op
    static void triggerUpdate() {} // No-op
};

#endif // ENABLE_OTA_HANDLER
