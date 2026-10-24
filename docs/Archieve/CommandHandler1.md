
---

### Long-Term Design for Command Handling

To support complex logic like script parsing, key, and mouse commands, you can extend the current `CommandHandler` into a more robust system. Here’s how it can be structured:

#### 1. **CommandHandler**: Core Command Processing
- Acts as the central entry point for all commands.
- Delegates commands to specialized modules (e.g., KeyHandler, MouseHandler, ScriptHandler).

#### 2. **Modular Handlers**: Specialized Logic
- Each handler (e.g., `KeyHandler`, `MouseHandler`, `ScriptHandler`) encapsulates specific functionality.
- This keeps the `CommandHandler` clean and focused.

#### 3. **Command Registry**: Dynamic Command Registration
- Maintain a registry of available commands.
- Allows dynamic addition of new commands without modifying the core.

#### 4. **Task-Based Execution**: Asynchronous Operations
- Use FreeRTOS tasks for operations like script execution to prevent blocking.

---

### Updated Structure

#### `CommandHandler.h`
```cpp
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <map>
#include <functional>

class CommandHandler {
public:
    static void handleCommand(const String& command);

    static void registerCommand(const String& name, std::function<void(const String&)> handler);

private:
    static std::map<String, std::function<void(const String&)>> commandRegistry;
};

#endif // COMMAND_HANDLER_H
```

---

#### `CommandHandler.cpp`
```cpp
#include "CommandHandler.h"
#include "KeyHandler.h"
#include "MouseHandler.h"
#include "ScriptHandler.h"

std::map<String, std::function<void(const String&)>> CommandHandler::commandRegistry;

void CommandHandler::handleCommand(const String& command) {
    String cmd = command;
    String args = "";

    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
        cmd = command.substring(0, spaceIndex);
        args = command.substring(spaceIndex + 1);
    }

    if (commandRegistry.find(cmd) != commandRegistry.end()) {
        commandRegistry[cmd](args); // Call the registered handler
    } else {
        Serial.println("Unknown command: " + cmd);
    }
}

void CommandHandler::registerCommand(const String& name, std::function<void(const String&)> handler) {
    commandRegistry[name] = handler;
}
```

---

#### Example Handlers

##### `KeyHandler.h`
```cpp
#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H

#include <Arduino.h>

class KeyHandler {
public:
    static void pressKey(const String& args);
};

#endif // KEY_HANDLER_H
```

##### `KeyHandler.cpp`
```cpp
#include "KeyHandler.h"

void KeyHandler::pressKey(const String& args) {
    // Parse and handle key press commands
    Serial.println("Pressing key: " + args);
    // Example: Implement keypress logic here
}
```

##### `MouseHandler.h` and `MouseHandler.cpp`
Similar structure to `KeyHandler`, but for mouse commands (e.g., move, click).

##### `ScriptHandler.h` and `ScriptHandler.cpp`
Use logic inspired by the DuckScript example for parsing and executing scripts asynchronously.

---

### Example Usage

#### Initialization (`setup()`):
```cpp
#include "CommandHandler.h"
#include "KeyHandler.h"
#include "MouseHandler.h"
#include "ScriptHandler.h"

void setup() {
    Serial.begin(115200);

    // Register commands
    CommandHandler::registerCommand("key", KeyHandler::pressKey);
    CommandHandler::registerCommand("mouse", MouseHandler::handleCommand);
    CommandHandler::registerCommand("script", ScriptHandler::executeScript);
}
```

#### Handling Commands:
```cpp
void onCommandReceived(const String& command) {
    CommandHandler::handleCommand(command);
}
```

---

### Script Execution with FreeRTOS
For script execution, you can use a FreeRTOS task:
```cpp
void scriptTask(void* parameter) {
    String* fileName = (String*)parameter;

    ScriptHandler::run(*fileName);

    delete fileName; // Clean up
    vTaskDelete(NULL); // End task
}
```

To start a script:
```cpp
void ScriptHandler::executeScript(const String& fileName) {
    String* fileNameCopy = new String(fileName); // Allocate on the heap
    xTaskCreate(scriptTask, "ScriptTask", 4096, fileNameCopy, 1, NULL);
}
```

---

### Benefits of the Design
1. **Centralized Command Management**:
   - Commands are processed in a single, extensible `CommandHandler`.
2. **Specialized Handlers**:
   - Each handler focuses on a specific functionality (e.g., keys, mouse, scripts).
3. **Dynamic Command Registration**:
   - Easily add new commands without modifying the core `CommandHandler`.
4. **Asynchronous Execution**:
   - Scripts and other long-running tasks won’t block the main loop.
5. **Scalability**:
   - The structure can accommodate complex features like script parsing and execution.

This design ensures that adding advanced features, such as script-like scripting or additional device controls, is straightforward and maintains code readability and modularity.