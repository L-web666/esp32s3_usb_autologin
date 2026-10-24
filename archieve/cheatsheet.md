
---

### 1. DuckyScript Cheatsheet (Markdown)
Save this as `DuckyScript_Cheatsheet.md`:

```markdown
# DuckyScript Cheatsheet
This cheatsheet covers the DuckyScript commands supported by your ESP32-S3 implementation as of March 28, 2025.

## Core Commands
| Command         | Description                                    | Example                |
|-----------------|------------------------------------------------|------------------------|
| `REM`           | Comment (ignored)                             | `REM This is a comment` |
| `DELAY`         | Pause execution (ms)                          | `DELAY 1000`           |
| `DEFAULT_DELAY` | Set delay between all commands (ms)           | `DEFAULT_DELAY 100`    |
| `STRING`        | Type a string                                 | `STRING Hello World`   |
| `STRINGLN`      | Type a string and press Enter                 | `STRINGLN dir`         |
| `REPEAT`        | Repeat the previous command N times           | `REPEAT 3`             |
| `ENTER`         | Press Enter key                               | `ENTER`                |
| `COMMAND`       | Custom: Pass text to CommandHandler           | `COMMAND reboot`       |

## Modifier Keys
| Command | Description                | Example       |
|---------|----------------------------|---------------|
| `GUI`   | Windows key (left)         | `GUI r`       |
| `CTRL`  | Control key (left)         | `CTRL c`      |
| `ALT`   | Alt key (left)             | `ALT F4`      |
| `SHIFT` | Shift key (left)           | `SHIFT a`     |
| *Note*: Combine with other keys (e.g., `CTRL ALT DELETE`).

## Navigation & Special Keys
| Command       | Description            | Alias             |
|---------------|------------------------|-------------------|
| `UP`          | Up arrow               | `UPARROW`         |
| `DOWN`        | Down arrow             | `DOWNARROW`       |
| `LEFT`        | Left arrow             | `LEFTARROW`       |
| `RIGHT`       | Right arrow            | `RIGHTARROW`      |
| `SPACE`       | Spacebar               |                   |
| `TAB`         | Tab key                |                   |
| `BACKSPACE`   | Backspace key          |                   |
| `DELETE`      | Delete key             |                   |
| `HOME`        | Home key               |                   |
| `END`         | End key                |                   |
| `PAGEUP`      | Page Up key            | `PAGE_UP`         |
| `PAGEDOWN`    | Page Down key          | `PAGE_DOWN`       |
| `INSERT`      | Insert key             |                   |
| `CAPSLOCK`    | Caps Lock key          |                   |
| `NUMLOCK`     | Num Lock key           |                   |
| `SCROLLLOCK`  | Scroll Lock key        |                   |
| `PRINTSCREEN` | Print Screen key       |                   |
| `PAUSE`       | Pause/Break key        |                   |
| `MENU`        | Context Menu key       |                   |

## Function Keys
| Command | Description |
|---------|-------------|
| `F1`    | Function 1  |
| `F2`    | Function 2  |
| ...     | ...         |
| `F12`   | Function 12 |

## Keypad Keys
| Command       | Description         |
|---------------|---------------------|
| `KP_0` - `KP_9` | Keypad numbers    |
| `KP_PLUS`     | Keypad +            |
| `KP_MINUS`    | Keypad -            |
| `KP_MULTIPLY` | Keypad *            |
| `KP_DIVIDE`   | Keypad /            |
| `KP_ENTER`    | Keypad Enter        |

## Punctuation
| Command         | Symbol | Alias         |
|-----------------|--------|---------------|
| `MINUS`         | -      | `-`           |
| `EQUAL`         | =      | `=`           |
| `BRACKET_LEFT`  | [      | `[`           |
| `BRACKET_RIGHT` | ]      | `]`           |
| `BACKSLASH`     | \      | `\`           |
| `SEMICOLON`     | ;      | `;`           |
| `APOSTROPHE`    | '      | `'`           |
| `GRAVE`         | `      | `` ` ``       |
| `COMMA`         | ,      | `,`           |
| `PERIOD`        | .      | `.`           |
| `SLASH`         | /      | `/`           |

## Notes
- Case sensitivity: `STRING` respects case; keys like `A` and `a` map to the same HID code.
- Unsupported: `ATTACKMODE`, `LED_*`, `WHILE`, `IF` (due to hardware/memory limits).
- Custom `COMMAND`: Passes all text after `COMMAND` to your board’s `CommandHandler`.
```

---

### 2. Full Example Scripts with Comments
Here are two test scripts to verify all features. Save them as `.txt` files in your `LittleFS` filesystem (e.g., `test1.txt` and `test2.txt`).

#### Test Script 1: Basic Features (`test1.txt`)
```
REM Test basic DuckyScript features
DEFAULT_DELAY 100       // Set 100ms delay between commands
GUI r                   // Open Run dialog (Windows + R)
DELAY 1000              // Wait 1 second
STRING cmd              // Type "cmd"
ENTER                   // Press Enter to open Command Prompt
DELAY 500               // Wait for CMD to open
STRINGLN dir            // Type "dir" and press Enter
REPEAT 2                // Repeat "dir" twice
STRING exit             // Type "exit"
ENTER                   // Close CMD
COMMAND status          // Custom command to your CommandHandler
```

**Purpose**: Tests core commands (`REM`, `DELAY`, `DEFAULT_DELAY`, `STRING`, `STRINGLN`, `ENTER`, `REPEAT`, `GUI`), modifier combos, and your custom `COMMAND`.

#### Test Script 2: Advanced Keys (`test2.txt`)
```
REM Test navigation, special, and keypad keys
DELAY 500               // Initial delay
GUI d                   // Minimize all windows (Win + D)
DELAY 1000              // Wait 1 second
STRING notepad          // Open Notepad
ENTER                   // Press Enter
DELAY 500               // Wait for Notepad
STRING Typing test:     // Type some text
SPACE                   // Space
SHIFT a                 // Capital "A"
BACKSPACE               // Delete last character
TAB                     // Tab over
F5                      // Insert timestamp (Notepad shortcut)
ENTER                   // New line
UP                      // Move cursor up
RIGHT                   // Move right
DELETE                  // Delete character
HOME                    // Go to line start
END                     // Go to line end
PAGEUP                  // Scroll up
PAGEDOWN                // Scroll down
PRINTSCREEN             // Take screenshot
CTRL v                  // Paste screenshot (if clipboard active)
MENU                    // Open context menu
DOWNARROW               // Navigate menu
KP_PLUS                 // Keypad +
KP_5                    // Keypad 5
CAPSLOCK                // Toggle Caps Lock
COMMAND reboot          // Custom command
```

**Purpose**: Tests navigation keys, special keys, function keys, keypad keys, punctuation, and modifiers, plus another `COMMAND` example.

---

### 3. Resources and Links
Here are official and supplementary resources for DuckyScript:

- **Official DuckyScript Documentation**:
  - [Hak5 USB Rubber Ducky Docs](https://docs.hak5.org/hc/en-us/categories/115001225368-USB-Rubber-Ducky) - General overview and payloads.
  - [DuckyScript 1.0 Reference](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript) - The original spec your code mostly follows.
  - [DuckyScript 3.0 Additions](https://docs.hak5.org/hc/en-us/articles/360010555153-DuckyScript-3-0) - Advanced features (some unsupported here).

- **Community Resources**:
  - [Payloads Repository](https://github.com/hak5/usbrubberducky-payloads) - Example scripts to adapt and test.
  - [Hak5 Forums](https://forums.hak5.org/forum/56-usb-rubber-ducky/) - Community discussion and troubleshooting.

- **HID Usage Tables**:
  - [USB HID Usage Tables](https://www.usb.org/sites/default/files/hut1_21.pdf) - Official spec for HID codes (e.g., `HID_USAGE_DESKTOP_SYSTEM_CONTEXT_MENU` is `0x84`).

- **ESP32-Specific**:
  - [ESP32 USB HID Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/usb_host.html) - If you’re customizing `USBHIDKeyboard`.
  - [Arduino-ESP32 HID Library](https://github.com/espressif/arduino-esp32/tree/master/libraries/USB) - Likely what you’re using.

- **Recommended Tools**:
  - [DuckyScript Encoder](https://github.com/hak5darren/USB-Rubber-Ducky/tree/master/Encoder) - Convert scripts to Rubber Ducky binary (not needed here, but useful for comparison).
  - [Payload Studio](https://payloadstudio.hak5.org/) - GUI for writing DuckyScript (test compatibility).

---

### 4. User Guide: Writing DuckyScripts for Your Implementation
Here’s everything a user needs to know to write scripts for your ESP32-S3 board:

#### Basics
- **Syntax**: One command per line, case-insensitive for commands (e.g., `delay` = `DELAY`).
- **Execution**: Upload scripts to `LittleFS` (e.g., `/script.txt`) and run via `DUCKY FILE /script.txt`.
- **Timing**: Use `DELAY` for explicit pauses; `DEFAULT_DELAY` for consistent gaps.

#### Supported Commands
- **Core**: See cheatsheet above (`REM`, `DELAY`, `STRING`, etc.).
- **Keys**: All letters (A-Z), numbers (0-9), punctuation, navigation, special, function, and keypad keys listed in `KeyMappings.cpp`.
- **Modifiers**: `GUI`, `CTRL`, `ALT`, `SHIFT` can be standalone or combined (e.g., `CTRL ALT DELETE`).
- **Custom `COMMAND`**: Anything after `COMMAND` is sent to your `CommandHandler`. Examples:
  - `COMMAND reboot` - Reboot your board (if supported).
  - `COMMAND led on` - Custom board command (define in `CommandHandler`).

#### Writing Tips
- **Timing**: Start with `DELAY 500` or `DEFAULT_DELAY 100` to ensure commands execute reliably on slow systems.
- **Case Sensitivity**: For `STRING`, write text as desired (e.g., `STRING Hello` types "Hello"); modifiers like `SHIFT` handle capitalization explicitly.
- **Repeats**: Use `REPEAT` after any command (e.g., `STRING a`, `REPEAT 3` types "aaaa").
- **Testing**: Start simple (e.g., `STRING test`, `ENTER`), then add complexity.

#### Limitations
- No `ATTACKMODE` (fixed HID mode on ESP32).
- No `LED_*` (add GPIO control if needed).
- No loops/conditionals (`WHILE`, `IF`) due to memory constraints.
- Mouse support requires `USBHIDMouse` integration (not included yet).

#### Example Workflow
1. Write script in a text editor (e.g., `test.txt`).
2. Upload to `LittleFS` using ESP32 tools (e.g., PlatformIO’s `pio run -t uploadfs`).
3. Run via serial command: `DUCKY FILE /test.txt`.
4. Debug with serial output (your `debugI` logs).

---

### Testing and Verification
- **Run Test Scripts**: Use `test1.txt` and `test2.txt` on a Windows PC (or adapt for macOS/Linux).
- **Verify Output**:
  - Test 1: CMD opens, runs `dir` three times, exits, and logs your `COMMAND`.
  - Test 2: Notepad opens, types text, navigates, uses special keys, and triggers your `COMMAND`.
- **Check Logs**: Ensure `debugI`/`debugE` messages match expected behavior.

---

### Additional Recommendations
- **Expand `COMMAND`**: Document supported `CommandHandler` commands (e.g., `reboot`, `status`) in your cheatsheet.
- **Add Mouse Support**: Integrate `USBHIDMouse` and use `mouseButtonMap` for `MOUSE_LEFT`, etc., if needed.
- **Error Handling**: Enhance `DuckyScriptHandler` to log unsupported commands more explicitly.

This should cover everything you need to write, test, and understand DuckyScript for your board. Let me know if you need more examples or help with specific payloads!