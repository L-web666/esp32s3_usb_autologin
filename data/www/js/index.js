//index.js
console.log('index.js loaded');

import {httpGet, showMessage} from './global.js';

// Function to reboot the device
async function rebootDevice() {
  try {
    await httpGet('/device/reboot'); // Call the centralized GET function
    console.log("Device reboot initiated successfully!");
    showMessage("Device reboot initiated successfully!", "success");
  } catch (error) {
    console.error("Failed to reboot the device:", error);
    showMessage("Failed to reboot the device.", "error");
  }
}

// Event listeners
document.addEventListener("DOMContentLoaded", () => {
  const rebootButton = document.getElementById("rebootButton");
  rebootButton.addEventListener("click", rebootDevice);
});

window.addEventListener('DOMContentLoaded', async () => {
  try {
    const response = await httpGet('/device/info');
    const { data } = response; // Extract the 'data' object

    if (!data) {
      throw new Error('Data not found in response.');
    }

    const tableBody = document.getElementById('device-info');
    tableBody.innerHTML = ''; // Clear existing rows

    // Loop over each key-value pair in the 'data' object
    for (const [key, value] of Object.entries(data)) {
      const row = document.createElement('tr');

      const keyCell = document.createElement('td');
      keyCell.textContent = key;

      const valueCell = document.createElement('td');
      valueCell.textContent = value;

      row.appendChild(keyCell);
      row.appendChild(valueCell);
      tableBody.appendChild(row);
    }
    console.log("Device info loaded successfully!");
  } catch (error) {
    console.error('Error fetching device info:', error);
    showMessage('Error fetching device info.', 'error');
  }
});