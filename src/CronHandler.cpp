#ifdef ENABLE_CRON_HANDLER

#include "CronHandler.h"
#include "CommandHandler.h"
#include "NonBlockingTimer.h"

std::vector<CronHandler::CronJob> CronHandler::cronJobs;
static NonBlockingTimer cronTimer(1000);

void CronHandler::init()
{
    registerCommands();
    debugI("CronHandler initialized");
}

void CronHandler::loop()
{
    if(cronTimer.isReady())
    {
        executeJobs();
    }
}

void CronHandler::executeJobs()
{
    time_t now = time(nullptr);

    for (auto &job : cronJobs)
    {
        if (now >= job.nextExecution)
        {
            debugV("Executing cron job: %s -> %s", job.schedule.c_str(), job.command.c_str());
            CommandHandler::handleCommand(job.command.c_str());
            updateNextExecution(job);
        }
    }
}

void CronHandler::updateNextExecution(CronJob &job)
{
    time_t now = time(nullptr);
    job.nextExecution = cron_next(&job.expr, now);
    if (job.nextExecution == (time_t)-1)
    {
        debugE("Failed to calculate next execution time for schedule: %s", job.schedule.c_str());
    }
}

void CronHandler::listJobs()
{
    if (cronJobs.empty()) {
        debugI("No cron jobs registered.");
        return;
    }

    time_t now = time(nullptr);
    for (size_t i = 0; i < cronJobs.size(); ++i) {
        const auto &job = cronJobs[i];
        debugI("[%d] Schedule: %s, Command: %s, Next Execution: %s", 
               i, job.schedule.c_str(), job.command.c_str(), ctime(&job.nextExecution));
    }
}

void CronHandler::removeJob(int jobId)
{
    if (jobId < 0 || static_cast<size_t>(jobId) >= cronJobs.size()) {
        debugE("Invalid cron job ID: %d", jobId);
        return;
    }

    cronJobs.erase(cronJobs.begin() + jobId);
    debugI("Cron job [%d] removed successfully.", jobId);
}

void CronHandler::registerJob(const String &command)
{
    debugI("Full Command Received: %s", command.c_str());

    // Locate the first and second quotes for the schedule
    int firstQuoteStart = command.indexOf('\"');
    int firstQuoteEnd = command.indexOf('\"', firstQuoteStart + 1);
    debugI("First Quote Start: %d, First Quote End: %d", firstQuoteStart, firstQuoteEnd);

    if (firstQuoteStart == -1 || firstQuoteEnd == -1) {
        debugE("Invalid syntax. Schedule must be enclosed in double quotes.");
        return;
    }

    // Extract the schedule (first quoted string)
    String schedule = command.substring(firstQuoteStart + 1, firstQuoteEnd);
    schedule.trim();
    debugI("Parsed Schedule: %s", schedule.c_str());

    // Locate the second set of quotes for the command
    int secondQuoteStart = command.indexOf('\"', firstQuoteEnd + 1);
    int secondQuoteEnd = command.indexOf('\"', secondQuoteStart + 1);
    debugI("Second Quote Start: %d, Second Quote End: %d", secondQuoteStart, secondQuoteEnd);

    if (secondQuoteStart == -1 || secondQuoteEnd == -1) {
        debugE("Invalid syntax. Command must be enclosed in double quotes.");
        return;
    }

    // Extract the command (second quoted string)
    String cronCommand = command.substring(secondQuoteStart + 1, secondQuoteEnd);
    cronCommand.trim();
    debugI("Parsed Command: %s", cronCommand.c_str());

    // Validate that both schedule and command are non-empty
    if (schedule.isEmpty() || cronCommand.isEmpty()) {
        debugE("Invalid syntax. Both schedule and command must be provided.");
        return;
    }

    // Validate the cron schedule
    cron_expr expr;
    const char *error = nullptr;
    cron_parse_expr(schedule.c_str(), &expr, &error);
    if (error) {
        debugE("Invalid cron schedule: %s. Expected 5 or 6 fields (e.g., '* * * * * *').", error);
        return;
    }

    // Calculate the next execution time
    time_t now = time(nullptr);
    time_t nextExecution = cron_next(&expr, now);
    if (nextExecution == (time_t)-1) {
        debugE("Failed to calculate the next execution time for schedule: %s", schedule.c_str());
        return;
    }

    // Register the cron job
    CronJob job = {schedule.c_str(), cronCommand.c_str(), expr, nextExecution};
    cronJobs.push_back(job);

    debugI("Cron job registered: %s -> %s", schedule.c_str(), cronCommand.c_str());
}

void CronHandler::registerCommands()
{
    CommandHandler::registerCommand("crontab", [](const String &command)
                                    {
        debugI("Full Command Received: %s", command.c_str());

        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (cmd == "add") {
            registerJob(command);
        } else if (cmd == "list") {
            listJobs();
        } else if (cmd == "remove") {
            int jobId = args.toInt();
            removeJob(jobId);
        } else {
            debugW("Unknown crontab subcommand: %s", cmd.c_str());
        } }, "Handles led commands. Usage: led <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "Example: crontab add \"*/15 * * * * *\" \"tft print hello\"\n"
                                         "Note: Schedule and command must be enclosed in double quotes.\n"
                                         "Schedule format: \"<seconds> <minutes> <hours> <day of month> <month> <day of week> <year>\n"
                                         "list \n"
                                         "remove <jobId>\n"
        );
}

#endif // ENABLE_CRON_HANDLER

// Usage extended with year:
// crontab add "*/5 * * * * *" "tft print hello1"
// crontab add "*/15 * * * * *" "hid mouse 1000,1000"
// Usage extended without year:
// crontab add "1-15 * * * *" "tft print white"
// crontab add "1-15 * * * *" "led color white"
// crontab add "15-30 * * * *" "tft print green"
// crontab add "15-30 * * * *" "led color green"
// crontab add "30-45 * * * *" "tft print blue"
// crontab add "30-45 * * * *" "led color blue"
// crontab add "45-59 * * * *" "tft print purple"
// crontab add "45-59 * * * *" "led color purple"
// crontab add "* * * * *" "tft clock"

