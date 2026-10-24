//config.js
console.log("config.js loaded");

// Centralized device configuration
const DEV_DEVICE = "passtxt5.local";
//const DEV_DEVICE = "demo1.local";
//const DEV_DEVICE = "demo2.local";

// Determine Base URL and WebSocket URL based on environment
const BASE_URL = window.location.hostname === "localhost" 
    ? `http://${DEV_DEVICE}` 
    : "";

const WS_URL = `ws://${window.location.hostname === "localhost" 
    ? DEV_DEVICE 
    : window.location.hostname}:8232`;

console.log(`BASE_URL: ${BASE_URL}`);

console.log(`WS_URL: ${WS_URL}`);

export { WS_URL, BASE_URL};