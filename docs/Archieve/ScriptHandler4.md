Let’s clarify those notes and address your issue with the ESP32 rebooting after repeating a task multiple times.

---

### **Important Notes Explained**

#### **1. Task Priority**
In FreeRTOS, tasks are assigned priorities that determine their execution order. If the script handler runs at a low priority and there are higher-priority tasks in your system, the script handler might be starved of CPU time, causing delays or failures in execution.

- **Solution**:
  - Assign an appropriate priority to the task running the script handler.
  - Typically, tasks handling time-critical operations (e.g., networking, hardware communication) are assigned higher priorities, while non-critical tasks (e.g., logging or periodic background tasks) have lower priorities.

Example of task creation with priority:
```cpp
xTaskCreatePinnedToCore(
    scriptTask,         // Task function
    "ScriptTask",       // Task name
    4096,               // Stack size (bytes)
    NULL,               // Task parameters
    1,                  // Task priority (higher numbers = higher priority)
    NULL,               // Task handle
    tskNO_AFFINITY      // Core affinity
);
```

- Assign priorities carefully:
  - `0`: Idle task (very low priority).
  - `1-4`: Low-priority background tasks.
  - `5+`: High-priority tasks like networking or hardware drivers.

---

#### **2. System Tick Configuration**
FreeRTOS uses a "system tick" to schedule and manage tasks. The system tick rate determines the smallest time unit available for task delays and scheduling.

- **`pdMS_TO_TICKS`**:
  - Converts milliseconds to ticks based on the configured tick rate (`configTICK_RATE_HZ`).

- **Problem with Incorrect Configuration**:
  - If the tick rate is too low (e.g., `100 Hz`), delays and task scheduling become coarse and imprecise.
  - If it's too high, the system wastes resources handling frequent tick interrupts.

- **Solution**:
  - Ensure `configTICK_RATE_HZ` is set appropriately in your FreeRTOS configuration (usually in `sdkconfig` for ESP32 projects).

For most ESP32 applications, `configTICK_RATE_HZ` is set to `1000 Hz` (1 ms tick resolution), which is suitable for precise delays.

To verify:
1. Open `sdkconfig` (via `idf.py menuconfig` if using ESP-IDF).
2. Navigate to:
   ```
   Component config → FreeRTOS → Tick rate (Hz)
   ```
3. Ensure it is set to `1000 Hz`.

---

### **Why the ESP32 Reboots**
Reboots on the ESP32 typically happen due to one of the following issues:

#### **1. Stack Overflow**
- If the task running the `REPEAT` command uses too much stack (e.g., due to deep recursion, large local variables, or nested function calls), the stack overflows, causing the board to reboot.

- **Solution**:
  - Increase the stack size when creating the task.
    ```cpp
    xTaskCreatePinnedToCore(
        scriptTask,         // Task function
        "ScriptTask",       // Task name
        8192,               // Increase stack size
        NULL,               // Task parameters
        1,                  // Task priority
        NULL,               // Task handle
        tskNO_AFFINITY      // Core affinity
    );
    ```

- Check FreeRTOS stack usage:
  Use `uxTaskGetStackHighWaterMark()` to monitor stack usage:
  ```cpp
  debugI("Stack remaining: %u bytes", uxTaskGetStackHighWaterMark(NULL));
  ```

---

#### **2. Watchdog Timer (WDT) Reset**
- If the `REPEAT` command runs for too long without yielding, the watchdog timer resets the board.

- **Solution**:
  - Use `vTaskDelay(0)` or `taskYIELD()` within the loop to yield control to other tasks during long operations.

Example:
```cpp
for (unsigned int i = 0; i < repeatCount; i++) {
    for (const String &repeatLine : repeatBuffer) {
        CommandHandler::handleCommand(repeatLine);

        // Apply default delay
        if (defaultDelay > 0) {
            vTaskDelay(pdMS_TO_TICKS(defaultDelay));
        }

        // Yield control to avoid WDT reset
        vTaskDelay(0); // or taskYIELD();
    }
}
```

---

#### **3. Out-of-Memory (Heap Allocation) Issues**
- If `REPEAT` dynamically allocates memory (e.g., for buffers or strings) and fails to free it, the system may run out of heap memory.

- **Solution**:
  - Avoid excessive memory allocations in loops.
  - Use tools like `esp_get_free_heap_size()` to monitor available memory:
    ```cpp
    debugI("Free heap: %u bytes", esp_get_free_heap_size());
    ```

---

### **Checklist to Fix Reboot Issue**
1. **Increase Stack Size**:
   - Ensure the script handler task has sufficient stack space (e.g., `8192` bytes).

2. **Yield Control**:
   - Use `vTaskDelay(0)` or `taskYIELD()` inside long loops to prevent watchdog resets.

3. **Monitor Memory**:
   - Check for memory leaks or excessive allocations during the `REPEAT` loop.

4. **Set System Tick Rate**:
   - Confirm `configTICK_RATE_HZ` is set to `1000 Hz` for precise delays.

5. **Adjust Task Priority**:
   - Assign an appropriate priority to the script handler task to ensure it runs without interference.

---

### Example Fix for `REPEAT` Command
Here’s an updated `REPEAT` loop with all fixes applied:
```cpp
if (CommandHandler::equalsIgnoreCase(command, "REPEAT")) {
    repeatCount = args.toInt();
    debugI("Repeating buffered commands %u times", repeatCount);

    for (unsigned int i = 0; i < repeatCount; i++) {
        for (const String &repeatLine : repeatBuffer) {
            CommandHandler::handleCommand(repeatLine);

            // Apply default delay
            if (defaultDelay > 0) {
                vTaskDelay(pdMS_TO_TICKS(defaultDelay));
            }

            // Yield control to avoid WDT reset
            vTaskDelay(0);
        }
    }

    repeatBuffer.clear(); // Clear the buffer after execution
    return true; // Special command handled
}
```

---