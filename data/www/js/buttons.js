// buttons.js

import { BASE_URL } from './config.js';

const endPoint = BASE_URL;

let allButtons = [];
let categories = [];

// Fetch Categories
async function fetchCategories() {
  try {
    const response = await fetch(`${endPoint}/categories`, {
      headers: { Authorization: `Bearer test` } // Match the token used in buttoneditor.js
    });
    if (response.ok) {
      const data = await response.json();
      categories = data.categories || [];
      return categories;
    } else {
      throw new Error("Failed to fetch categories");
    }
  } catch (err) {
    console.error("Error fetching categories:", err);
    showMessage("Failed to load categories. Please try again.");
    return [];
  }
}

// Helper function to get category name by ID
function getCategoryNameById(id) {
  const category = categories.find(cat => cat.id === id);
  return category ? category.name : "Unknown";
}

// Fetch and Render Buttons
export async function fetchButtons() {
  try {
    // Fetch categories first
    await fetchCategories();

    const response = await fetch(`${endPoint}/buttons`);
    if (!response.ok) throw new Error("Failed to fetch buttons");
    const data = await response.json();
    allButtons = data.buttons || [];
    populateCategoryDropdown(allButtons);
    renderButtons(allButtons);
  } catch (error) {
    console.error("Error fetching buttons:", error);
    const buttonContainer = document.getElementById("buttonContainer");
    if (buttonContainer) {
      buttonContainer.innerHTML = "<p>Error loading buttons.</p>";
    }
  }
}

// Show Message in Message Box
function showMessage(message) {
  const messageBox = document.getElementById("message-box");
  if (messageBox) {
    messageBox.textContent = message;
    messageBox.classList.remove("hidden");
    setTimeout(() => {
      messageBox.classList.add("hidden");
      messageBox.textContent = "";
    }, 3000); // Hide after 3 seconds
  }
}

// Populate Dropdown with Unique Categories
function populateCategoryDropdown(buttons) {
  const categorySet = new Set(buttons.map(button => button.categoryId));
  const categoryFilter = document.getElementById("categoryFilter");

  if (!categoryFilter) return;

  // Clear existing options except "All"
  categoryFilter.innerHTML = '<option value="all">All</option>';

  // Add categories dynamically
  categorySet.forEach(categoryId => {
    const categoryName = getCategoryNameById(categoryId);
    const option = document.createElement("option");
    option.value = categoryId;
    option.textContent = categoryName;
    categoryFilter.appendChild(option);
  });

  // Attach event listener for filtering
  categoryFilter.addEventListener("change", () => {
    const selectedCategoryId = categoryFilter.value;
    const filteredButtons = selectedCategoryId === "all" ? allButtons : allButtons.filter(button => button.categoryId === parseInt(selectedCategoryId));
    renderButtons(filteredButtons);
  });
}

// Render Buttons
function renderButtons(buttons) {
  const container = document.getElementById("buttonContainer");
  if (!container) {
    console.error("Error: #buttonContainer not found in the DOM.");
    return;
  }
  container.innerHTML = ""; // Clear previous content

  buttons.forEach(button => {
    const btn = document.createElement("button");
    btn.className = "button tooltip";
    btn.textContent = button.name;

    // Apply inline styles only if provided
    if (button.style?.color) btn.style.color = button.style.color;
    if (button.style?.fontSize) btn.style.fontSize = button.style.fontSize;
    if (button.style?.backgroundColor) {
      btn.style.setProperty("--dynamic-bg-color", button.style.backgroundColor);
    }
    if (button.style?.borderWidth && button.style?.borderColor) {
      btn.style.border = `${button.style.borderWidth} solid ${button.style.borderColor}`;
    }
    if (button.style?.borderRadius) btn.style.borderRadius = button.style.borderRadius;
    if (button.style?.padding) btn.style.padding = button.style.padding;
    if (button.style?.width) btn.style.width = button.style.width;

    // Attach click event to send the command
    //btn.onclick = () => sendCommand(button.command);

    // Modified click event to run the button instead of sending command
    btn.onclick = () => runButton(button.id);

    container.appendChild(btn);
  });
}

// New function to run a button by ID
export async function runButton(buttonId) {
  if (navigator.vibrate) {
    navigator.vibrate(50);
  }

  try {
    const response = await fetch(`${endPoint}/run-button?id=${buttonId}`, {
      method: "POST",
      headers: {
        "Content-Type": "application/x-www-form-urlencoded",
      },
    });

    if (response.ok) {
      console.log(`Button ${buttonId} executed successfully`);
      //showMessage(`Button ${buttonId} executed`);
    } else {
      const errorText = await response.text();
      console.error(`Failed to run button ${buttonId}: ${errorText}`);
      showMessage(`Failed to run button: ${errorText}`);
    }
  } catch (error) {
    console.error("Error running button:", error);
    showMessage("Error running button");
  }
}

// Send Command to Server
export async function sendCommand(command) {
  if (navigator.vibrate) {
    navigator.vibrate(50);
  }

  try {
    const response = await fetch(`${endPoint}/command/set`, {
      method: "POST",
      headers: {
        "Content-Type": "text/plain",
      },
      body: command,
    });

    if (response.ok) {
      console.log(`Command sent: ${command}`);
    } else {
      showMessage("Failed to send command");
    }
  } catch (error) {
    console.error("Error sending command:", error);
    showMessage("Error sending command");
  }
}

// Ensure `fetchButtons` runs after the DOM is loaded
document.addEventListener("DOMContentLoaded", () => {
  fetchButtons();
});