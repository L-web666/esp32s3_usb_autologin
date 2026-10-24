To add icons to your buttons dynamically, you can follow this approach:

1. **Add an `icon` property in your JSON** to define the icon class (e.g., for Font Awesome or similar icon libraries).
2. **Include logic in the JavaScript** to create an `<i>` or `<span>` element for the icon and insert it into each button.
3. **Ensure the icon styling is consistent** by using CSS.

---

### **Step 1: Add Icons in JSON**

Update your JSON structure to include an `icon` property for each button. For example:

```json
{
  "buttons": [
    {
      "id": "btn1",
      "label": "Button 1",
      "icon": "fas fa-play-circle", // Font Awesome icon class
      "style": {
        "color": "#ffffff",
        "fontSize": "1rem",
        "backgroundColor": "#3c3c3c",
        "borderColor": "#444",
        "borderWidth": "2px",
        "borderRadius": "8px",
        "padding": "10px 20px",
        "width": "100%"
      },
      "command": "tft print button1",
      "tooltip": "This is Button 1"
    },
    {
      "id": "btn2",
      "label": "Button 2",
      "icon": "fas fa-envelope", // Font Awesome icon class
      "style": {
        "color": "#ffffff",
        "fontSize": "1rem",
        "backgroundColor": "#555555",
        "borderColor": "#666",
        "borderWidth": "2px",
        "borderRadius": "8px",
        "padding": "10px 20px",
        "width": "100%"
      },
      "command": "tft print button2",
      "tooltip": "Send a message"
    }
  ]
}
```

---

### **Step 2: Update JavaScript to Handle Icons**

Modify the `renderButtons` function to dynamically add icons to each button:

```javascript
function renderButtons(buttons) {
  const container = document.getElementById("buttonContainer");
  container.innerHTML = ""; // Clear previous content

  buttons.forEach(button => {
    const btn = document.createElement("button");
    btn.className = "button tooltip";

    // Apply inline styles with defaults
    btn.style.color = button.style?.color || "var(--text-color)";
    btn.style.fontSize = button.style?.fontSize || "var(--font-size)";
    btn.style.backgroundColor = button.style?.backgroundColor || "var(--button-bg)";
    btn.style.border = `${button.style?.borderWidth || "2px"} solid ${button.style?.borderColor || "var(--button-border)"}`;
    btn.style.borderRadius = button.style?.borderRadius || "var(--border-radius)";
    btn.style.padding = button.style?.padding || "var(--padding-small)";
    btn.style.width = button.style?.width || "100%";

    // Add tooltip if available
    if (button.tooltip) {
      btn.setAttribute("data-tooltip", button.tooltip);
    }

    // Create an icon element if specified
    if (button.icon) {
      const icon = document.createElement("i");
      icon.className = button.icon; // Assign the icon class from JSON
      icon.style.marginRight = "10px"; // Add spacing between icon and text
      btn.appendChild(icon);
    }

    // Add button label
    const label = document.createTextNode(button.label);
    btn.appendChild(label);

    // Attach click event to send the command
    btn.onclick = () => sendCommand(button.command);

    container.appendChild(btn);
  });
}
```

---

### **Step 3: Include Font Awesome**

To use Font Awesome icons, include the library in the `<head>` of your HTML:

```html
<head>
  <title>Buttons with Icons</title>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <meta name="description" content="Dynamic buttons with icons and tooltips" />
  <link rel="stylesheet" href="styles.css" />
  <script type="module" src="global.js" defer></script>
  <!-- Add Font Awesome -->
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css" />
</head>
```

---

### **Step 4: Add CSS for Icons**

You can customize the icon styling to ensure consistency with the buttons. For example:

```css
.button i {
  font-size: 1.2rem; /* Adjust icon size */
  vertical-align: middle; /* Align icon with text */
  margin-right: 10px; /* Add space between icon and text */
}
```

---

### **Final Example**

#### JSON
```json
{
  "buttons": [
    {
      "id": "btn1",
      "label": "Play",
      "icon": "fas fa-play-circle",
      "command": "tft print play"
    },
    {
      "id": "btn2",
      "label": "Send",
      "icon": "fas fa-envelope",
      "command": "tft print send"
    }
  ]
}
```

#### HTML and JavaScript

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <title>Buttons with Icons</title>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css" />
  <style>
    /* Button Styles */
    .button {
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 10px 20px;
      background-color: #3c3c3c;
      color: #ffffff;
      border: 2px solid #444444;
      border-radius: 8px;
      font-size: 1rem;
      cursor: pointer;
      transition: all 0.3s ease;
      max-width: 300px;
      margin: 10px auto;
    }

    .button i {
      margin-right: 10px;
    }

    .button:hover {
      background-color: #555555;
      border-color: #555555;
    }
  </style>
</head>
<body>
  <div id="buttonContainer"></div>

  <script>
    // Mock JSON Data
    const buttons = [
      {
        id: "btn1",
        label: "Play",
        icon: "fas fa-play-circle",
        command: "tft print play"
      },
      {
        id: "btn2",
        label: "Send",
        icon: "fas fa-envelope",
        command: "tft print send"
      }
    ];

    // Render Buttons with Icons
    function renderButtons(buttons) {
      const container = document.getElementById("buttonContainer");
      container.innerHTML = "";

      buttons.forEach(button => {
        const btn = document.createElement("button");
        btn.className = "button";

        // Add Icon
        if (button.icon) {
          const icon = document.createElement("i");
          icon.className = button.icon;
          btn.appendChild(icon);
        }

        // Add Label
        btn.appendChild(document.createTextNode(button.label));

        // Attach Command
        btn.onclick = () => {
          alert(`Command sent: ${button.command}`);
        };

        container.appendChild(btn);
      });
    }

    // Render Buttons on Page Load
    renderButtons(buttons);
  </script>
</body>
</html>
```

---

### **How It Works**
1. **Icons in JSON**:
   - The `icon` property specifies the class of the icon (e.g., Font Awesome classes).

2. **JavaScript Adds Icons**:
   - Dynamically creates an `<i>` element with the class from the JSON and appends it to the button.

3. **Styling**:
   - Icons are styled consistently using CSS.

4. **Responsive Design**:
   - Buttons are styled to look good on mobile and desktop, with aligned icons and labels.

This method makes your buttons visually appealing and flexible for various use cases.