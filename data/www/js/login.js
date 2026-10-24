//login.js
console.log("login.js loaded");

import {BASE_URL} from './config.js';

const endPoint = BASE_URL;

function logout() {
    fetch(`${endPoint}/auth/logout`, {
        method: "POST",
        credentials: "include"
    })
        .then(response => response.json())
        .then(data => {
            console.log(data.message);
            document.cookie = "session=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT;";
            window.location.href = "/login.html";
        })
        .catch(error => console.error("Logout failed", error));
}

async function login() {
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    const messageElement = document.getElementById("message");

    if (!username || !password) {
        messageElement.textContent = "Please enter both username and password.";
        return;
    }

    try {
        const response = await fetch(`${endPoint}/auth/login`, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username, password }),
            credentials: "include",
            redirect: "manual"
        });

        const rawResponse = await response.text();
        console.log("Raw Response:", rawResponse);

        if (!response.headers.get("Content-Type")?.includes("application/json")) {
            throw new Error("Server returned non-JSON response.");
        }

        const result = JSON.parse(rawResponse);
        
        if (response.ok) {
            messageElement.style.color = "green";
            messageElement.textContent = "Login successful!";
            window.location.href = "/index.html";
            //setTimeout(() => { window.location.href = "/index.html"; }, 1000);
        } else {
            messageElement.style.color = "red";
            messageElement.textContent = result.message || "Login failed.";
        }
    } catch (error) {
        messageElement.style.color = "red";
        messageElement.textContent = "An error occurred. Please try again.";
        console.error("Login error:", error);
    }
}

// Attach event listeners
document.addEventListener("DOMContentLoaded", () => {
    const loginButton = document.getElementById("login-btn");
    const logoutButton = document.getElementById("logout-btn");

    if (loginButton) {
        loginButton.addEventListener("click", login);
    }
    if (logoutButton) {
        logoutButton.addEventListener("click", logout);
    }
});

// Export functions (optional, only if needed elsewhere)
export { login, logout };