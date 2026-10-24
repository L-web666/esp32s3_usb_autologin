#pragma once

#ifdef ENABLE_CRON_HANDLER

#include "Globals.h"
#include "CronExpr.h"
#include <vector>
#include <functional>
#include <string>

class CronHandler {
private:
    struct CronJob {
        std::string schedule;
        std::string command;
        cron_expr expr;
        time_t nextExecution;
    };

    static std::vector<CronJob> cronJobs;
    static void updateNextExecution(CronJob &job);
    static void listJobs();
    static void removeJob(int jobId);
    static void registerJob(const String &command);
    static void registerCommands();

public:
    static void init();
    static void loop();
    static void executeJobs();
};

#else

class CronHandler {
public:
    static void init() {} // No-op
    static void loop() {} // No-op
    static void executeJobs() {} // No-op
};

#endif // ENABLE_CRON_HANDLER
