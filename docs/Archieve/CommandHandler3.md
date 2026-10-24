# CommandHandler Documentation and Examples

## Overview

**CommandHandler** is a utility for registering, parsing, and handling text-based commands.

### Features

1. **Simple Commands**: Register commands with no arguments or subcommands.
2. **Commands with Arguments**: Handle commands with user-provided arguments.
3. **Complex Commands with Subcommands**: Use a primary command with multiple subcommands and arguments.
4. **Command Aliases**: Register multiple names for the same command.
5. **Help Functionality**: Provide a list of all registered commands and descriptions.
6. **Default Handler**: Handle unrecognized commands gracefully.

---

## Complete Example Code

```cpp
#include "CommandHandler.h"

/**
 * CommandHandler Overview:
 * -------------------------
 * CommandHandler is a utility for registering, parsing, and handling text-based commands.
 * Features include:
 * 1. Simple commands
 * 2. Commands with arguments
 * 3. Complex commands with subcommands
 * 4. Command aliases
 * 5. Help functionality
 * 6. Default handler for unrecognized commands
 */

/**
 * Simple Command Example
 * -----------------------
 * A command with no arguments or subcommands.
 */
void registerSimpleCommand() {
    CommandHandler::registerCommand("hello", [](const String&) {
        debugI("Hello, world!");
    }, "Greets the user with 'Hello, world!'");
}

/**
 * Command with Arguments Example
 * -------------------------------
 * A command that accepts a single argument.
 */
void registerCommandWithArgs() {
    CommandHandler::registerCommand("echo", [](const String& args) {
        debugI("Echo: %s", args.c_str());
    }, "Echoes the provided arguments. Usage: echo [message]");
}

/**
 * Complex Command with Subcommands Example
 * -----------------------------------------
 * A command with multiple subcommands and their respective arguments.
 */
void registerComplexCommand() {
    CommandHandler::registerCommand("led", [](const String& command) {
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args); // Parse subcommand and arguments

        if (cmd == "color") {
            debugI("Set LED color to %s", args.c_str());
        } else if (cmd == "brightness") {
            debugI("Set LED brightness to %d", args.toInt());
        } else if (cmd == "clear") {
            debugI("Clear all LEDs");
        } else {
            debugW("Unknown LED subcommand: %s", cmd.c_str());
        }
    }, "Controls the LED. Usage: led <color|brightness|clear> [value]");
}

/**
 * Command Alias Example
 * ----------------------
 * Register multiple names for the same command.
 */
void registerCommandAlias() {
    CommandHandler::registerCommand("setcolor", [](const String& args) {
        debugI("Set color to %s", args.c_str());
    }, "Sets the LED color. Usage: setcolor [color]");

    // Register aliases for the same command
    CommandHandler::registerCommandAlias("color", "setcolor");
    CommandHandler::registerCommandAlias("c", "setcolor");
}

/**
 * Help Command Example
 * ---------------------
 * A command that lists all registered commands and their descriptions.
 */
void registerHelpCommand() {
    CommandHandler::registerCommand("help", [](const String&) {
        CommandHandler::listCommands();
    }, "Lists all available commands.");
}

/**
 * Default Handler Example
 * ------------------------
 * A fallback handler for unrecognized commands.
 * This can be used to provide user feedback or trigger fallback actions.
 */
void registerDefaultHandler() {
    CommandHandler::setDefaultHandler([](const String& command) {
        debugW("Unknown command: %s. Showing help menu.", command.c_str());
        CommandHandler::listCommands(); // Optionally display the help menu
    });
}

/**
 * Setup Example
 * --------------
 * Register all commands and set the default handler.
 */
void setup() {
    Serial.begin(115200);

    debugI("* Initializing CommandHandler...");

    // Register commands
    registerSimpleCommand();      // Registers 'hello'
    registerCommandWithArgs();    // Registers 'echo'
    registerComplexCommand();     // Registers 'led'
    registerCommandAlias();       // Registers 'setcolor', 'color', and 'c'
    registerHelpCommand();        // Registers 'help'
    registerDefaultHandler();     // Sets default handler for unrecognized commands

    debugI("* CommandHandler initialized.");
}

/**
 * Loop Example
 * -------------
 * Simulate user input by handling commands from Serial input.
 */
void loop() {
    if (Serial.available()) {
        String input = Serial.readStringUntil('\n'); // Read user input from Serial
        input.trim(); // Remove trailing spaces or newline characters
        CommandHandler::handleCommand(input); // Process the command
    }
}
```

---

## Usage Scenarios

### **Example Input and Outputs**

#### **Registered Command**

**Input**:
```plaintext
hello
```

**Output**:
```plaintext
Hello, world!
```

#### **Command with Arguments**

**Input**:
```plaintext
echo Hello ESP32
```

**Output**:
```plaintext
Echo: Hello ESP32
```

#### **Complex Command with Subcommands**

**Input**:
```plaintext
led color blue
```

**Output**:
```plaintext
Set LED color to blue
```

#### **Command Alias**

**Input**:
```plaintext
color red
```

**Output**:
```plaintext
Set color to red
```

#### **Help Command**

**Input**:
```plaintext
help
```

**Output**:
```plaintext
* Listing all available commands:
 - hello: Greets the user with 'Hello, world!'
 - echo: Echoes the provided arguments. Usage: echo [message]
 - led: Controls the LED. Usage: led <color|brightness|clear> [value]
 - setcolor: Sets the LED color. Usage: setcolor [color]
```

#### **Unknown Command**

**Input**:
```plaintext
unknowncmd
```

**Output**:
```plaintext
Unknown command: unknowncmd. Showing help menu.
* Listing all available commands:
 - hello: Greets the user with 'Hello, world!'
 - echo: Echoes the provided arguments. Usage: echo [message]
 - led: Controls the LED. Usage: led <color|brightness|clear> [value]
 - setcolor: Sets the LED color. Usage: setcolor [color]
```

---

## Summary

### Key Features:
1. **Simple Commands**: For basic actions.
2. **Commands with Arguments**: For actions requiring additional parameters.
3. **Complex Commands**: For multi-level functionality.
4. **Command Aliases**: To provide alternative names for commands.
5. **Help Functionality**: Guides users with available commands.
6. **Default Handler**: Ensures graceful handling of unrecognized commands.

This code serves as a robust and extensible foundation for any command-driven application.
