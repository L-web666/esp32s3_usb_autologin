
---

### **README.md**

```markdown
# Custom Script Editor with CodeMirror

This project implements a custom script editor using [CodeMirror](https://codemirror.net/) with syntax highlighting for a custom scripting language. It includes support for commands, numbers, strings, and comments, styled with the Dracula dark theme.

---

## **Features**
- Syntax highlighting for:
  - **Keywords** (e.g., `REM`, `DEFAULTDELAY`, `DELAY`, `REPEAT`, `SCRIPT`)
  - **Numbers** (e.g., `500`, `1000`)
  - **Strings** (e.g., `"example.scr"`)
  - **Comments** (e.g., `REM This is a comment`)
- Dark-themed editor using the Dracula theme.
- Case-insensitive matching for commands and comments.
- Line numbers for better readability.

---

## **Setup Instructions**
### Prerequisites
- A browser to test the HTML file.
- [CodeMirror](https://codemirror.net/) library included in your project.

### How to Use
1. Save the provided `HTML` example code (from this project) to a file, e.g., `index.html`.
2. Open the file in a web browser.
3. Write your custom script in the editor. Syntax highlighting will automatically apply based on the rules.

---

## **Syntax Rules**
### Commands
- The following commands are supported and highlighted as **keywords**:
  - `REM` (treated as a comment)
  - `DEFAULTDELAY`
  - `DELAY`
  - `REPEAT`
  - `SCRIPT`
  - `LED`
  - `TFT`

### Numbers
- Numbers (e.g., `500`, `1000`, `5`) are highlighted regardless of their position in a line.

### Strings
- Quoted text (e.g., `"example.scr"`) is highlighted as a string.

### Comments
- Lines starting with `REM` are treated as comments.

### Line Example
```plaintext
REM This is a comment
DEFAULTDELAY 500 1000
DELAY 1000 2000
repeat 5 10
SCRIPT FILE "example.scr"
1000 2000 3000
```

---

## **Customization**
### Adding New Keywords
To add new commands or keywords:
1. Edit the `defineSimpleMode` section in the JavaScript code.
2. Add your command to the regex for keywords:
   ```javascript
   { regex: /\b(?:rem|defaultdelay|delay|repeat|script|led|tft|newcommand)\b/i, token: "keyword" },
   ```

### Changing Colors
To modify token colors:
1. Update the corresponding CSS classes:
   ```css
   .cm-keyword { color: #00ff00; /* Bright Green */ }
   .cm-number { color: #ff4500; /* Orange */ }
   .cm-string { color: #1e90ff; /* Dodger Blue */ }
   .cm-comment { color: #808080; /* Gray */ font-style: italic; }
   ```

---

## **Dependencies**
- [CodeMirror 5.65.13](https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.13/codemirror.min.js)
- Dracula theme CSS: `https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.13/theme/dracula.min.css`
- CodeMirror Simple Mode Addon: `https://cdnjs.cloudflare.com/ajax/libs/codemirror/5.65.13/addon/mode/simple.min.js`

---

## **Files**
- `index.html`: Main HTML file for the editor.
- `README.md`: Documentation for the project.

---

## **License**
This project is licensed under the MIT License. Feel free to use, modify, and distribute it as needed.

---

## **Acknowledgments**
- [CodeMirror](https://codemirror.net/) for the powerful text editor library.
- [Dracula Theme](https://draculatheme.com/) for the dark-themed color palette.
```

---