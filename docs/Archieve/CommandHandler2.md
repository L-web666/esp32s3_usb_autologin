### **What is `registerCommandAlias`?**

The `registerCommandAlias` method allows you to associate an alias (alternate name) with an existing command. This means multiple names can trigger the same functionality. For example, both `setColor` and `set` could call the same handler.

### **Complete Usages Note**

Below are examples of using `CommandHandler` for various scenarios, from simple commands to complex ones with aliases.

---

#### **1. Simple Command**
**Description**: A single command with no subcommands or arguments.

```cpp
CommandHandler::registerCommand("hello", [](const String&) {
    debugI("Hello, world!");
}, "Greets the user with 'Hello, world!'");
```

**Usage**:
```plaintext
hello
```

**Output**:
```plaintext
Hello, world!
```

---

#### **2. Command with Arguments**
**Description**: A command that accepts arguments.

```cpp
CommandHandler::registerCommand("echo", [](const String& args) {
    debugI("Echo: %s", args.c_str());
}, "Echoes the provided arguments. Usage: echo [message]");
```

**Usage**:
```plaintext
echo This is a test
```

**Output**:
```plaintext
Echo: This is a test
```

---

#### **3. Complex Command with Subcommands**
**Description**: A command with multiple subcommands.

```cpp
CommandHandler::registerCommand("led", [](const String& command) {
    String cmd = command;
    String args = "";

    int spaceIndex = command.indexOf(' ');
    if (spaceIndex > 0) {
        cmd = command.substring(0, spaceIndex);
        args = command.substring(spaceIndex + 1);
    }

    if (cmd == "color") {
        debugI("Set LED color to %s", args.c_str());
    } else if (cmd == "brightness") {
        debugI("Set LED brightness to %d", args.toInt());
    } else {
        debugW("Unknown LED command: %s", cmd.c_str());
    }
}, "Controls the LED. Usage: led <color|brightness> [value]");
```

**Usage**:
```plaintext
led color red
led brightness 50
```

**Output**:
```plaintext
Set LED color to red
Set LED brightness to 50
```

---

#### **4. Command Aliases**
**Description**: Register alternate names for a command.

```cpp
CommandHandler::registerCommand("setcolor", [](const String& args) {
    debugI("Set color to %s", args.c_str());
}, "Sets the LED color. Usage: setcolor [color]");

CommandHandler::registerCommandAlias("color", "setcolor");
CommandHandler::registerCommandAlias("c", "setcolor");
```

**Usage**:
```plaintext
setcolor blue
color green
c yellow
```

**Output**:
```plaintext
Set color to blue
Set color to green
Set color to yellow
```

---

#### **5. Help Command to List All Commands**
**Description**: A command to list all registered commands and their descriptions.

```cpp
CommandHandler::registerCommand("help", [](const String&) {
    CommandHandler::listCommands();
}, "Lists all available commands.");
```

**Usage**:
```plaintext
help
```

**Output**:
```plaintext
Available commands:
 - hello: Greets the user with 'Hello, world!'
 - echo: Echoes the provided arguments. Usage: echo [message]
 - led: Controls the LED. Usage: led <color|brightness> [value]
 - setcolor: Sets the LED color. Usage: setcolor [color]
```

---

#### **6. Default Handler for Unknown Commands**
**Description**: A fallback mechanism for unrecognized commands.

```cpp
CommandHandler::setDefaultHandler([](const String& command) {
    debugW("Unknown command: %s", command.c_str());
});
```

**Usage**:
```plaintext
unknowncommand
```

**Output**:
```plaintext
Unknown command: unknowncommand
```

---

### **Summary Table**

| Feature                        | Example Code                                                                                      | Notes                                                                                   |
|--------------------------------|--------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------|
| Simple Command                 | `registerCommand("hello", ...);`                                                                | For commands without arguments.                                                        |
| Command with Arguments         | `registerCommand("echo", ...);`                                                                 | Handles user-provided arguments.                                                       |
| Complex Commands with Subcommands | `registerCommand("led", ...);`                                                                 | Subcommands allow multi-level functionality.                                            |
| Command Aliases                | `registerCommandAlias("alias", "original");`                                                    | Provides multiple names for the same functionality.                                     |
| Help Command                   | `registerCommand("help", ...);`                                                                 | Lists all available commands.                                                          |
| Default Handler                | `setDefaultHandler(...);`                                                                       | Handles commands that are not recognized.                                              |

---

### **Best Practices**

1. **Use Aliases Judiciously**:
   - Aliases improve usability but should not clutter the system with redundant names.
   
2. **Provide Descriptions**:
   - Always include descriptions for better help output and documentation.

3. **Default Command Behavior**:
   - Include a default handler for debugging and fallback operations.

This structured approach ensures flexibility, extensibility, and ease of use in your command handling system.