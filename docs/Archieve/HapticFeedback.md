You can implement the same haptic feedback feature (buzz or vibration) for buttons on the web using the **Vibration API**, which is supported in most modern browsers on devices that have vibration capabilities (e.g., smartphones or tablets).

---

### **How to Add Vibration on Button Click**

#### **Step 1: Use the Vibration API**
The `navigator.vibrate()` method allows you to trigger vibration on compatible devices.

```javascript
function handleButtonClick() {
    // Vibrate for 50ms
    if (navigator.vibrate) {
        navigator.vibrate(50);
    }
    alert('Button clicked!'); // Example action
}
```

#### **Step 2: Attach to Buttons**
Update your HTML to include a button that triggers this vibration.

```html
<button onclick="handleButtonClick()">Click Me</button>
```

---

### **Full Example**

Here’s a complete example that adds vibration feedback when a button is pressed:

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Button Vibration Feedback</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background-color: #121212;
            color: #ffffff;
        }

        button {
            padding: 15px 30px;
            font-size: 1.2rem;
            color: #ffffff;
            background-color: #3c3c3c;
            border: 2px solid #444444;
            border-radius: 8px;
            cursor: pointer;
            transition: background-color 0.3s ease, transform 0.1s ease;
        }

        button:hover {
            background-color: #555555;
        }

        button:active {
            transform: scale(0.95); /* Slight press animation */
        }
    </style>
</head>
<body>
    <button onclick="handleButtonClick()">Click Me</button>

    <script>
        function handleButtonClick() {
            // Trigger vibration feedback for 50ms
            if (navigator.vibrate) {
                navigator.vibrate(50);
            } else {
                console.log("Vibration API not supported on this device.");
            }
            alert('Button clicked!');
        }
    </script>
</body>
</html>
```

---

### **How It Works**
1. **Vibration API**:
   - The `navigator.vibrate()` method accepts a single duration (e.g., `50` for 50ms) or an array (e.g., `[100, 50, 100]` for a pattern).
   - Only works on compatible devices (e.g., smartphones with vibration motors).

2. **Fallback for Unsupported Devices**:
   - If `navigator.vibrate` is not supported, no errors are thrown; you can handle this gracefully with a `console.log()` or alternate feedback.

3. **Styling Enhancements**:
   - Adds visual feedback (e.g., slight scaling when the button is pressed).

---

### **Browser Support**
- The Vibration API is widely supported on mobile browsers like Chrome, Edge, and Firefox on Android.
- **Not supported** on iOS devices (e.g., Safari on iPhone) as of now, since Apple does not allow web-based vibration control.

---

### **Enhancements for Web Apps**
You can combine this with other feedback mechanisms for a native-like experience:
1. **Sound Feedback**:
   - Play a short audio clip on button press.
   ```javascript
   const audio = new Audio('click-sound.mp3');
   audio.play();
   ```

2. **Visual Effects**:
   - Add animations like a button press effect (already in the `:active` styling above).

---

### **Limitations**
- **Vibration is device-dependent**: Only devices with vibration motors will provide feedback.
- **iOS Limitation**: The Vibration API is not supported on Safari for iPhones.
- **Permissions**: If a browser or device limits vibration due to security or user permissions, it won’t work.

---
