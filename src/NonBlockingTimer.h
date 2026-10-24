#pragma once

#include <Arduino.h>

class NonBlockingTimer
{
private:
    unsigned long previousMillis;    // Stores the last time the timer was checked
    unsigned long interval;          // Interval for the timer (in milliseconds)
    unsigned long pausedAt = 0;      // Tracks the time when the timer was paused
    unsigned long lastStatusUpdate;  // Tracks the last time status updates occurred
    unsigned long statusInterval;    // Interval for periodic status updates

public:
    /**
     * Constructor
     * @param interval The timer interval in milliseconds.
     * @param statusInterval Optional interval for status updates in milliseconds.
     */
    NonBlockingTimer(unsigned long interval, unsigned long statusInterval = 0)
        : previousMillis(0), interval(interval), lastStatusUpdate(0), statusInterval(statusInterval) {}

    /**
     * Sets a new interval for the timer.
     * @param newInterval The new interval in milliseconds.
     */
    void setInterval(unsigned long newInterval)
    {
        interval = newInterval;
    }

    /**
     * Sets a new interval for periodic status updates.
     * @param newStatusInterval The new status update interval in milliseconds.
     */
    void setStatusInterval(unsigned long newStatusInterval)
    {
        statusInterval = newStatusInterval;
    }

    /**
     * Checks if the timer has elapsed.
     * Resets the timer if elapsed.
     * @return True if the timer has elapsed, otherwise false.
     */
    bool isReady()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            previousMillis = currentMillis;
            return true;
        }
        return false;
    }

    /**
     * Checks if the status update interval has elapsed.
     * Resets the status update timer if elapsed.
     * @return True if the status update interval has elapsed, otherwise false.
     */
    bool isStatusReady()
    {
        if (statusInterval == 0)
        {
            return false; // Status checks disabled if interval is zero
        }

        unsigned long currentMillis = millis();
        if (currentMillis - lastStatusUpdate >= statusInterval)
        {
            lastStatusUpdate = currentMillis;
            return true;
        }
        return false;
    }

    /**
     * Resets the timer and the status update timer.
     */
    void reset()
    {
        previousMillis = millis();
        lastStatusUpdate = millis(); // Reset status updates as well
    }

    /**
     * Gets the remaining time until the timer elapses.
     * @return Remaining time in milliseconds. Returns 0 if the timer has already elapsed.
     */
    unsigned long remaining()
    {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval)
        {
            return 0; // Timer has expired
        }
        return interval - (currentMillis - previousMillis);
    }

    /**
     * Stops the timer by setting its state to far in the future.
     */
    void stop()
    {
        previousMillis = millis() + interval; // Effectively disables the timer
    }

    /**
     * Checks if the timer is still running.
     * @return True if the timer is running, otherwise false.
     */
    bool isRunning()
    {
        return remaining() > 0;
    }

    /**
     * Pauses the timer without resetting it.
     */
    void pause()
    {
        pausedAt = millis() - previousMillis;
        previousMillis = 0; // Mark as paused
    }

    /**
     * Resumes the timer after it was paused.
     */
    void resume()
    {
        if (pausedAt > 0)
        {
            previousMillis = millis() - pausedAt;
            pausedAt = 0;
        }
    }

    /**
     * Prints the current state of the timer for debugging purposes.
     */
    void debugInfo()
    {
        Serial.print("Timer Info: Interval=");
        Serial.print(interval);
        Serial.print(" ms, Remaining=");
        Serial.print(remaining());
        Serial.print(" ms, PreviousMillis=");
        Serial.println(previousMillis);
    }
};

/**
 * Example Usage:
 * 
 * NonBlockingTimer myTimer(5000, 1000); // 5-second timer with 1-second status updates
 * 
 * void setup()
 * {
 *     Serial.begin(115200);
 *     myTimer.reset();
 * }
 * 
 * void loop()
 * {
 *     if (myTimer.isStatusReady())
 *     {
 *         Serial.println("Status Update: Timer is running.");
 *     }
 * 
 *     if (myTimer.isReady())
 *     {
 *         Serial.println("Timer expired!");
 *         myTimer.reset();
 *     }
 * }
 */
