# DuckyScript Cheat Sheet - Complete Command List

DuckyScript is a scripting language commonly used to create payloads for USB Rubber Ducky devices made by Hak5. These programmable USB devices are disguised as keyboards to inject keystrokes quickly. Below is a complete list of DuckyScript commands, along with explanations and examples.

---

## **Version**
The latest version of DuckyScript is **DuckyScript 3.0** (released by Hak5). Commands may vary slightly between versions, so make sure you're working with the correct version for your hardware/firmware.

---

## **DuckyScript Commands**

### **1. `REM`**
- **Description:** Adds a comment. Used for documentation purposes. Ignored during execution.
- **Syntax:** `REM <text>`
- **Example:**
  ```
  REM This is a comment in DuckyScript
  ```

---

### **2. `DELAY`**
- **Description:** Adds a delay in milliseconds.
- **Syntax:** `DELAY <time_in_ms>`
- **Example:**
  ```
  DELAY 1000  REM Waits for 1 second
  ```

---

### **3. `STRING`**
- **Description:** Types out a string of text.
- **Syntax:** `STRING <text>`
- **Example:**
  ```
  STRING Hello, World!
  ```

---

### **4. `ENTER` or `RETURN`**
- **Description:** Simulates pressing the `Enter` or `Return` key.
- **Syntax:** `ENTER` (or `RETURN`)
- **Example:**
  ```
  ENTER
  ```

---

### **5. `TAB`**
- **Description:** Simulates pressing the `Tab` key.
- **Syntax:** `TAB`
- **Example:**
  ```
  TAB
  ```

---

### **6. `WINDOWS` (or `GUI`)**
- **Description:** Simulates pressing the `Windows` key (GUI key). On macOS, it executes the Command key.
- **Syntax:** `WINDOWS` (or `GUI`)
- **Example:**
  ```
  WINDOWS R  REM Opens the Run dialog on Windows
  ```

---

### **7. `CONTROL` or `CTRL`**
- **Description:** Simulates pressing the `Ctrl` key.
- **Syntax:** `CONTROL` (or `CTRL`) + KEY
- **Example:**
  ```
  CONTROL C  REM Copies the selected text using Ctrl+C
  ```

---

### **8. `ALT`**
- **Description:** Simulates pressing the `Alt` key.
- **Syntax:** `ALT` + KEY
- **Example:**
  ```
  ALT F  REM Opens the File menu
  ```

---

### **9. `SHIFT`**
- **Description:** Simulates pressing the `Shift` key.
- **Syntax:** `SHIFT` + KEY
- **Example:**
  ```
  SHIFT TAB  REM Navigates backward in form fields
  ```

---

### **10. `CAPSLOCK`**
- **Description:** Toggles the Caps Lock key.
- **Syntax:** `CAPSLOCK`
- **Example:**
  ```
  CAPSLOCK
  ```

---

### **11. `NUMLOCK`**
- **Description:** Toggles the Num Lock key.
- **Syntax:** `NUMLOCK`
- **Example:**
  ```
  NUMLOCK
  ```

---

### **12. `FUNCTION` (or `FN`)**
- **Description:** Simulates the function key (`Fn`). Rarely used because of hardware-specific behavior.
- **Syntax:** `FUNCTION` (or `FN`)
- **Example:**
  ```
  FUNCTION
  ```

---

### **13. `DOWNARROW`, `UPARROW`, `LEFTARROW`, `RIGHTARROW`**
- **Description:** Simulates the arrow keys.
- **Syntax:** Arrow key command in uppercase.
- **Example:**
  ```
  DOWNARROW
  RIGHTARROW
  ```

---

### **14. `REPEAT`**
- **Description:** Repeats the last command a specified number of times.
- **Syntax:** `REPEAT <number>`
- **Example:**
  ```
  DOWNARROW
  REPEAT 5  REM Presses the down arrow key 5 more times
  ```

---

### **15. `DELAY`**
- **Description:** Pauses execution for a set amount of time.
- **Syntax:** `DELAY <milliseconds>`
- **Example:**
  ```
  DELAY 500  REM Wait half a second
  ```

---

### **16. `ESCAPE` or `ESC`**
- **Description:** Simulates pressing the `Esc` key.
- **Syntax:** `ESCAPE` (or `ESC`)
- **Example:**
  ```
  ESC
  ```

---

### **17. `BACKSPACE`**
- **Description:** Simulates pressing the `Backspace` key.
- **Syntax:** `BACKSPACE`
- **Example:**
  ```
  BACKSPACE
  ```

---

### **18. `INSERT`**
- **Description:** Simulates pressing the `Insert` key.
- **Syntax:** `INSERT`
- **Example:**
  ```
  INSERT
  ```

---

### **19. `DELETE`**
- **Description:** Simulates pressing the `Delete` key.
- **Syntax:** `DELETE`
- **Example:**
  ```
  DELETE
  ```

---

### **20. `PAGEUP` and `PAGEDOWN`**
- **Description:** Simulates pressing the `Page Up` and `Page Down` keys.
- **Syntax:** `PAGEUP` or `PAGEDOWN`
- **Example:**
  ```
  PAGEUP
  PAGEDOWN
  ```

---

### **21. `SPACE`**
- **Description:** Simulates pressing the spacebar.
- **Syntax:** `SPACE`
- **Example:**
  ```
  SPACE
  ```

---

### **22. `F1` to `F12`**
- **Description:** Simulates function keys (F1 through F12).
- **Syntax:** `F<number>`
- **Example:**
  ```
  F5  REM Refreshes the web page in most browsers
  ```

---

### **23. `DEFAULT_DELAY`**
- **Description:** Sets a default delay in milliseconds between each command.
- **Syntax:** `DEFAULT_DELAY <time_in_ms>`
- **Example:**
  ```
  DEFAULT_DELAY 100  REM Adds 0.1s delay between commands
  ```

---

### **24. `STOP`**
- **Description:** Stops script execution. Only available in DuckyScript 3.0.
- **Syntax:** `STOP`
- **Example:**
  ```
  STOP  REM Halts execution at this point
  ```

---

### **25. `REPEAT`**
- **Description:** Repeats the previous command for the specified number of times.
- **Syntax:** `REPEAT <times>`
- **Example:**
  ```
  STRING test
  REPEAT 5
  ```

---

### **26. `PRINT`**
- **Description:** Displays debug messages. Useful for testing/debugging.
- **Syntax:** `PRINT <message>`
- **Example:**
  ```
  PRINT Script started...
  ```

---

### **27. `DEFINE`**
- **Description:** Creates variables and reusable macros.
- **Syntax:**
  ```
  DEFINE <macro_name> [commands]
  ```
  **Example:**
  ```
  DEFINE SAYHELLO STRING Hello, World!
  SAYHELLO
  ```

---

### **28. `LOOP`**
- **Description:** Repeats a block of code a defined number of times (available in DuckyScript 3.0).
- **Syntax:** 
  ```
  LOOP <number>
  <commands>
  END_LOOP
  ```
  **Example:**
  ```
  LOOP 3
  STRING Hello!
  ENTER
  END_LOOP
  ```

---

## **Examples**

### Example 1: Open Notepad and Type Text
```DuckyScript
DELAY 1000
WINDOWS R
DELAY 200
STRING notepad
ENTER
DELAY 500
STRING Hello, this is a DuckyScript example!
ENTER
```

### Example 2: Open Command Prompt and Run a Command
```DuckyScript
DELAY 1000
WINDOWS R
DELAY 200
STRING cmd
ENTER
DELAY 500
STRING ipconfig
ENTER
```

---

This README serves as a quick reference guide for DuckyScript commands. For the most accurate and up-to-date information, always check the Hak5 documentation or the official DuckyScript GitHub page!