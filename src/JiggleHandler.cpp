// JiggleHandler.cpp

#ifdef ENABLE_JIGGLE_HANDLER

#include "JiggleHandler.h"
#include "DeviceHandler.h"
#include "GfxHandler.h"

// Define the default interval and amount in one place
constexpr int DEFAULT_JIGGLE_INTERVAL = 60000;
constexpr int DEFAULT_JIGGLE_AMOUNT = 5;

NonBlockingTimer JiggleHandler::jiggleTimer(DEFAULT_JIGGLE_INTERVAL, 1000); // 1-second status updates
int JiggleHandler::jiggleInterval = DEFAULT_JIGGLE_INTERVAL;                // Use the same default interval here
int JiggleHandler::jiggleAmount = DEFAULT_JIGGLE_AMOUNT;                    // Default jiggle amount
bool JiggleHandler::showCountdown = true;                                   // Show countdown by default
bool JiggleHandler::jiggleEnabled = false;                                   // Jiggle initially disabled

void JiggleHandler::init()
{
    registerCommands();
    debugI("JiggleHandler initialized");
}

void JiggleHandler::loop()
{
    if (jiggleEnabled)
    {
        if (showCountdown && jiggleTimer.isStatusReady()) // Status check every 1 second
        {
            unsigned long timeRemaining = jiggleTimer.remaining();
            unsigned long secondsRemaining = (timeRemaining + 999) / 1000; // Round up to the nearest second
            GfxHandler::printMessage(String("Jiggle: ") + secondsRemaining);
            debugV("Jiggle countdown: %lu ms remaining", timeRemaining);
        }

        if (jiggleTimer.isReady()) // Jiggle action when timer expires
        {
            performJiggle();
        }
    }
}

void JiggleHandler::performJiggle()
{
    // Move the mouse slightly in a square pattern based on jiggleAmount
    DeviceHandler::sendMouseMovement(jiggleAmount, -jiggleAmount);
    delay(50);
    DeviceHandler::sendMouseMovement(-jiggleAmount, jiggleAmount);
    delay(50);
    debugI("Mouse jiggled with amount: %d.", jiggleAmount);
}

void JiggleHandler::registerCommands()
{
    CommandHandler::registerCommand("jiggle", [](const String &command)
                                    {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (CommandHandler::equalsIgnoreCase(cmd, "true"))
        {
            jiggleEnabled = true;
            debugI("Mouse jiggle enabled.");
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "false"))
        {
            jiggleEnabled = false;
            debugI("Mouse jiggle disabled.");
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "time"))
        {
            int interval = args.toInt();
            if (interval > 0)
            {
                jiggleInterval = interval;
                jiggleTimer.setInterval(jiggleInterval);
                debugI("Mouse jiggle interval set to %d ms.", jiggleInterval);
            }
            else
            {
                debugW("Invalid interval: %s", args.c_str());
            }
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "amount"))
        {
            int amount = args.toInt();
            if (amount > 0)
            {
                jiggleAmount = amount;
                debugI("Mouse jiggle amount set to %d.", jiggleAmount);
            }
            else
            {
                debugW("Invalid amount: %s", args.c_str());
            }
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "countdown"))
        {
            if (CommandHandler::equalsIgnoreCase(args, "true"))
            {
                showCountdown = true;
                debugI("Countdown display enabled.");
            }
            else if (CommandHandler::equalsIgnoreCase(args, "false"))
            {
                showCountdown = false;
                debugI("Countdown display disabled.");
            }
            else
            {
                debugW("Invalid argument for countdown: %s", args.c_str());
            }
        }
        else if (CommandHandler::equalsIgnoreCase(cmd, "state"))
        {
            String stateMessage = String("Jiggle State:\n") +
                                  "  Enabled: " + (jiggleEnabled ? "true" : "false") + "\n" +
                                  "  Interval: " + jiggleInterval + " ms\n" +
                                  "  Amount: " + jiggleAmount + " pixels\n" +
                                  "  Countdown: " + (showCountdown ? "true" : "false");
            debugI("%s", stateMessage.c_str());
            //GfxHandler::printMessage(stateMessage);
        }
        else
        {
            debugW("Unknown jiggle subcommand: %s", cmd.c_str());
        } }, "Handles JIGGLE commands. Usage: JIGGLE <subcommand> [args]\n"
             "  Subcommands:\n"
             "  true - Enables mouse jiggle\n"
             "  false - Disables mouse jiggle\n"
             "  time <ms> - Sets the jiggle interval in milliseconds\n"
             "  amount <pixels> - Sets the jiggle amount in pixels\n"
             "  countdown <true|false> - Enables or disables countdown display\n"
             "  state - Prints the current jiggle state");
}

#endif // ENABLE_JIGGLE_HANDLER
