//settings.js
console.log("settings.js loaded");

import {httpGet, httpPost, showMessage} from "./global.js";

let wifiScan = false;

// Load settings from the server
async function loadSettings() {
  try {
    console.log("Fetching settings from /settings/get...");
    
    //const response = await httpGet("/settings/get");
    //const data = response.data;
    const data = await httpGet("/settings/get");

    console.log("Received settings:", data);

    // Device
    document.getElementById("device_name").value = data.device?.name || "";
    document.getElementById("time_zone").value = data.device?.timezone || "";
    document.getElementById("boot_command").value = data.device?.bootCommand || "";
    document.getElementById("single_press").value = data.device?.singlePress || "";
    document.getElementById("double_press").value = data.device?.doublePress || "";
    document.getElementById("long_press").value = data.device?.longPress || "";
    document.getElementById("key_press_delay").value = data.device?.keyPressDelay || 20;

    // Device security
    document.getElementById("device_user_name").value = data.device?.userName || "";
    document.getElementById("device_user_password").value = data.device?.userPassword || "";
    
    // Wi-Fi
    wifiScan = data.wifi?.scan || false; // Use the default value if not set
    document.getElementById("wifi_scan").checked = data.wifi?.scan || false;
    document.getElementById("wifi_network").value = data.wifi?.ssid || "";
    document.getElementById("wifi_password").value = data.wifi?.password || "";

    // MQTT
    document.getElementById("mqtt_enabled").checked = data.mqtt?.enabled || false;
    document.getElementById("mqtt_server").value = data.mqtt?.server || "";
    document.getElementById("mqtt_port").value = data.mqtt?.port || "";
    document.getElementById("mqtt_username").value = data.mqtt?.username || "";
    document.getElementById("mqtt_password").value = data.mqtt?.password || "";
    document.getElementById("mqtt_ssl").checked = data.mqtt?.ssl || false;
    document.getElementById("mqtt_topic_sub").value = data.mqtt?.subTopic || "";
    document.getElementById("mqtt_topic_pub").value = data.mqtt?.pubTopic || "";

    // Security
    document.getElementById("api_key").value = data.security?.apiKey || "";
    
    // Optional: Handle features like cors/webHandler if needed
    // Example: document.getElementById("cors").checked = data.features?.cors || false;
    
    // Load Wi-Fi networks if Wi-Fi scan is enabled
    if( wifiScan) {
      console.log("Wi-Fi scan is enabled. Loading Wi-Fi networks...");
      loadWifiNetworks();
      syncWifiSelection();
    }

  } catch (error) {
    showMessage("Failed to load settings.", "error");
    console.error("Error loading settings:", error);
  }
}

// Save settings to the server
async function saveSettings() {
  const deviceName = document.getElementById("device_name").value.trim();
  const deviceUserName = document.getElementById("device_user_name").value.trim();
  const deviceUserPassword = document.getElementById("device_user_password").value.trim();
  const bootCommand = document.getElementById("boot_command").value.trim();
  const singlePress = document.getElementById("single_press").value.trim();
  const doublePress = document.getElementById("double_press").value.trim();
  const longPress = document.getElementById("long_press").value.trim();
  const keyPressDelay = parseInt(document.getElementById("key_press_delay").value.trim(), 10) || 20; // Default to 20 if not set
  const timeZone = document.getElementById("time_zone").value.trim();
  const wifiSsid = document.getElementById("wifi_network").value.trim();
  const wifiScan = document.getElementById("wifi_scan").checked;
  const wifiPassword = document.getElementById("wifi_password").value.trim();
  const mqttEnabled = document.getElementById("mqtt_enabled").checked;
  const mqttServer = document.getElementById("mqtt_server").value.trim();
  const mqttPort = parseInt(document.getElementById("mqtt_port").value.trim(), 10) || 0;
  const mqttUsername = document.getElementById("mqtt_username").value.trim();
  const mqttPassword = document.getElementById("mqtt_password").value.trim();
  const mqttSsl = document.getElementById("mqtt_ssl").checked;
  const mqttTopicSub = document.getElementById("mqtt_topic_sub").value.trim();
  const mqttTopicPub = document.getElementById("mqtt_topic_pub").value.trim();
  const apiKey = document.getElementById("api_key").value.trim();

  // Validate required fields
  if (!deviceName || !deviceUserName || !deviceUserPassword || !wifiSsid || !wifiPassword) {
    showMessage("Please fill in all mandatory fields.", "error");
    console.error("Validation failed: Missing required fields.");
    return;
  }

  try {
    console.log("Saving settings to /settings/set...");

    const body = {
      device: {
        name: deviceName,
        timezone: timeZone,
        bootCommand: bootCommand,
        singlePress: singlePress,
        doublePress: doublePress,
        longPress: longPress,
        userName: deviceUserName,
        userPassword: deviceUserPassword,
        keyPressDelay: keyPressDelay
      },
      wifi: {
        ssid: wifiSsid,
        password: wifiPassword,
        scan: wifiScan
      },
      mqtt: {
        enabled: mqttEnabled,
        server: mqttServer,
        port: mqttPort,
        username: mqttUsername,
        password: mqttPassword,
        ssl: mqttSsl,
        subTopic: mqttTopicSub,
        pubTopic: mqttTopicPub
      },
      security: {
        apiKey: apiKey
      }
    };

    console.log("Payload being sent:", body);

    await httpPost("/settings/set", body);
    showMessage("Settings saved successfully!", "success");
    rebootDevice();
  } catch (error) {
    showMessage("Failed to save settings.", "error");
    console.error("Error saving settings:", error);
  }
}

// Fetch available Wi-Fi networks
async function loadWifiNetworks() {
  try {
    console.log("Fetching available Wi-Fi networks from /device/wifi/networks...");
    const response = await httpGet("/device/wifi/networks");
    const networks = response.data.networks || [];

    const dropdown = document.getElementById("wifi_network_dropdown");

    // Clear existing options
    dropdown.innerHTML = '<option value="" selected>-- Select Wi-Fi Network --</option>';

    // Populate dropdown with available networks
    networks.forEach((network) => {
      const option = document.createElement("option");
      option.value = network.ssid;
      option.textContent = `${network.ssid} (RSSI: ${network.rssi}, ${network.encryptionType !== 0 ? "Secured" : "Open"
        })`;
      dropdown.appendChild(option);
    });

    console.log("Wi-Fi networks loaded successfully:", networks);
  } catch (error) {
    console.error("Error loading Wi-Fi networks:", error);
    showMessage("Failed to load Wi-Fi networks.", "error");
  }
}

// Sync dropdown selection with the input field
function syncWifiSelection() {
  const dropdown = document.getElementById("wifi_network_dropdown");
  const manualInput = document.getElementById("wifi_network");

  dropdown.addEventListener("change", () => {
    manualInput.value = dropdown.value;
  });

  manualInput.addEventListener("input", () => {
    if (manualInput.value) {
      dropdown.value = "";
    }
  });
}

document.addEventListener("DOMContentLoaded", () => {

  // Load existing settings on page load
  console.log("Page loaded. Initializing settings load...");
  loadSettings();
  // loadWifiNetworks();
  // syncWifiSelection();

  // Appears to crash device too large of a JSON file removing dropdown for now
  //loadTimezones();
  //syncTimezoneSelection();

  const form = document.querySelector(".settings-form");
  if (form) {
    form.addEventListener("submit", (e) => {
      e.preventDefault();  // Stop default form submission
    });
  }

  // Then attach your button click
  const saveButton = document.querySelector(".settings-form button");
  if (saveButton) {
    saveButton.addEventListener("click", saveSettings);
    console.log("Save button event listener attached.");
  } else {
    console.error("Save button not found.");
  }

  // ... rest of your setup ...
});

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

/*
async function loadTimezones() {
  try {
    console.log("Fetching available timezones from /device/timezones...");
    const response = await httpGet("/data/timezones");

    // Extract timezone data
    const timezones = response.data || {};
    const dropdown = document.getElementById("time_zone_dropdown");

    // Clear existing options
    dropdown.innerHTML = '<option value="" selected>-- Select Timezone --</option>';

    // Populate dropdown with available timezones
    Object.entries(timezones).forEach(([timezone, offset]) => {
      const option = document.createElement("option");
      option.value = timezone;
      option.textContent = `${timezone} (UTC Offset: ${offset})`;
      dropdown.appendChild(option);
    });

    console.log("Timezones loaded successfully:", timezones);
  } catch (error) {
    console.error("Error loading timezones:", error);
    showMessage("Failed to load timezones.", "error");
  }
}

// Sync dropdown selection with the input field
function syncTimezoneSelection() {
  const dropdown = document.getElementById("time_zone_dropdown");
  const manualInput = document.getElementById("time_zone");

  if (!dropdown || !manualInput) {
    console.error("Timezone dropdown or input field not found!");
    return;
  }

  dropdown.addEventListener("change", () => {
    console.log(`Dropdown selected: ${dropdown.value}`);
    manualInput.value = dropdown.value; // Sync input with dropdown selection
  });

  manualInput.addEventListener("input", () => {
    if (manualInput.value) {
      console.log(`Manual input: ${manualInput.value}`);
      dropdown.value = ""; // Clear dropdown selection when typing manually
    }
  });
}

// Attach event listeners
document.addEventListener("DOMContentLoaded", () => {
  // Load existing settings on page load
  console.log("Page loaded. Initializing settings load...");
  loadSettings();

  // Load Wi-Fi networks and attach event listener
  loadWifiNetworks();
  syncWifiSelection();

  // Attach save button event
  const saveButton = document.querySelector(".settings-form button");
  if (saveButton) {
    saveButton.addEventListener("click", saveSettings);
    console.log("Save button event listener attached.");
  } else {
    console.error("Save button not found.");
  }
});

*/