//TemplateHandler.h

#pragma once

#ifdef ENABLE_TEMPLATE_HANDLER

class TemplateHandler
{
private:
    static void debugLevels();
    static void registerCommands();

public:
    static void init();
    static void loop();
};

#else

class TemplateHandler { 
public: // No-op implementation of TemplateHandler
    static void init() {} // No-op
    static void loop() {} // No-op
};

#endif // ENABLE_TEMPLATE_HANDLER