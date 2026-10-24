Given that you already have the key modules (command handler, USB connectivity, file management, web server, Bluetooth, etc.) developed, here’s how you can structure **script parsing logic** for a robust, reusable, and extensible design:

---

## **1. Key Design Principles**

### **a) Separation of Concerns**
- **Parser**: Focuses on interpreting script lines into commands and arguments.
- **Executor**: Maps parsed commands to their handlers and executes them.
- **Command Registry**: Stores all available commands and their handlers.
- **Error Handling**: Deals with invalid commands, arguments, or execution issues.

---

### **b) Extensibility**
- Use a modular approach to add new commands without modifying core logic.
- Ensure the parser and executor are decoupled, so adding features like loops or conditions is easy.

---

### **c) Flexibility**
- Support for advanced scripting features:
  - Variables (e.g., `${variable}`)
  - Labels and `GOTO`
  - Loops and conditionals
  - Asynchronous operations

---

## **2. Script Parsing Architecture**

### **a) Core Components**
1. **Script Manager**
   - Manages loading and executing scripts line-by-line.
   - Handles state (e.g., variables, labels).

2. **Command Parser**
   - Breaks a script line into a command and arguments.

3. **Command Executor**
   - Matches parsed commands with registered handlers and executes them.

4. **Command Registry**
   - Maps commands (e.g., `STRING`, `DELAY`) to their respective handler functions.

---

### **b) Workflow**
1. **Load Script**
   - Read the script from a source (e.g., SD card, file system, or memory).

2. **Parse Script**
   - Split the script into lines.
   - Parse each line into:
     - **Command**: Primary action (e.g., `STRING`).
     - **Arguments**: Parameters for the action.

3. **Execute Commands**
   - Look up the command in the registry and execute its handler.
   - Pass arguments to the handler.

4. **Error Handling**
   - Log errors for unknown commands or invalid arguments.
   - Provide graceful fallback (e.g., skip invalid lines).

---

## **3. Implementation Plan**

### **a) Command Registration**

Create a **command registry** where commands and their handlers are stored dynamically. Use a hash map or similar data structure.

```cpp
#include <map>
#include <functional>
#include <Arduino.h>

class CommandRegistry {
public:
    using CommandHandler = std::function<void(const String&)>;

    static void registerCommand(const String& command, CommandHandler handler) {
        registry()[command] = handler;
    }

    static CommandHandler getCommandHandler(const String& command) {
        if (registry().count(command)) {
            return registry()[command];
        }
        return nullptr;
    }

private:
    static std::map<String, CommandHandler>& registry() {
        static std::map<String, CommandHandler> commands;
        return commands;
    }
};
```

---

### **b) Parsing Logic**

Implement a **CommandParser** that parses script lines.

```cpp
#include <Arduino.h>

class CommandParser {
public:
    static bool parse(const String& line, String& command, String& args) {
        // Trim whitespace
        String trimmed = line;
        trimmed.trim();

        // Ignore comments and empty lines
        if (trimmed.isEmpty() || trimmed.startsWith("REM")) {
            return false;
        }

        // Split into command and arguments
        int spaceIndex = trimmed.indexOf(' ');
        if (spaceIndex == -1) {
            command = trimmed;
            args = "";
        } else {
            command = trimmed.substring(0, spaceIndex);
            args = trimmed.substring(spaceIndex + 1);
        }

        return true;
    }
};
```

---

### **c) Script Execution**

The **ScriptManager** reads and executes commands from the script file.

```cpp
#include <Arduino.h>
#include "CommandParser.h"
#include "CommandRegistry.h"

class ScriptManager {
public:
    void executeScript(const String& script) {
        // Split script into lines
        int lineStart = 0;
        while (lineStart < script.length()) {
            int lineEnd = script.indexOf('\n', lineStart);
            if (lineEnd == -1) lineEnd = script.length();
            String line = script.substring(lineStart, lineEnd);

            // Parse line
            String command, args;
            if (CommandParser::parse(line, command, args)) {
                executeCommand(command, args);
            }

            lineStart = lineEnd + 1;
        }
    }

private:
    void executeCommand(const String& command, const String& args) {
        auto handler = CommandRegistry::getCommandHandler(command);
        if (handler) {
            handler(args);
        } else {
            Serial.println("Unknown command: " + command);
        }
    }
};
```

---

### **d) Example Command Handlers**

```cpp
void registerCommands() {
    CommandRegistry::registerCommand("STRING", [](const String& args) {
        Serial.println("Typing: " + args);
        // Add USB keyboard functionality here
    });

    CommandRegistry::registerCommand("DELAY", [](const String& args) {
        int delayTime = args.toInt();
        delay(delayTime);
    });

    CommandRegistry::registerCommand("LOOP", [](const String& args) {
        // Handle looping logic here
    });

    CommandRegistry::registerCommand("SET", [](const String& args) {
        // Handle variable assignment here
    });
}
```

---

## **4. Advanced Features**

### **a) Variables and Dynamic Data**
- Add support for variables using a map.

```cpp
#include <map>

class ScriptVariables {
public:
    static void setVariable(const String& key, const String& value) {
        variables()[key] = value;
    }

    static String getVariable(const String& key) {
        if (variables().count(key)) {
            return variables()[key];
        }
        return "";
    }

    static String replaceVariables(const String& input) {
        String output = input;
        for (const auto& [key, value] : variables()) {
            output.replace("${" + key + "}", value);
        }
        return output;
    }

private:
    static std::map<String, String>& variables() {
        static std::map<String, String> vars;
        return vars;
    }
};
```

---

### **b) Labels and GOTO**
- Add a pre-scan phase to store label positions.
- Jump to labels during execution.

```cpp
void parseLabels(const String& script) {
    // Store line numbers of labels for GOTO
}
```

---

### **c) Error Recovery**
- Skip problematic lines and log errors to Serial or a file.

---

## **5. Example Script**

```
REM This is a comment
STRING Hello, World!
DELAY 1000
SET myVar Test
STRING ${myVar}
```

---

## **6. Extensibility**

- **Add new commands**: Simply register them using `CommandRegistry::registerCommand`.
- **Support advanced scripting**: Add blocks for conditionals, loops, or asynchronous events.

This architecture ensures that your script parsing logic is modular, reusable, and extensible, supporting future enhancements seamlessly.