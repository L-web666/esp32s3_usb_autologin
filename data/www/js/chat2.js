//chat.js

import { config } from './secure.js';
import { BASE_URL } from './config.js';

const endPoint = BASE_URL;
var botReply = "";

async function sendMessage(userMessage) {

    const chatMessages = document.getElementById("chatMessages");

    // Add user message to chat UI
    appendMessage(userMessage, "user");

    try {

        const url = `${config.endpoint}/openai/deployments/${config.deployment}/chat/completions?api-version=${config.version}`;
        const response = await fetch(url, {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "api-key": config.apiKey
            },
            body: JSON.stringify({
                messages: [
                    { 
                        role: "system", 
                        content: "You are an AI that only communicates using DuckyScript." + 
                        " No explanations, no introductions—just raw DuckyScript. Every response" + 
                        " must be a valid DuckyScript payload. If an input is unclear, generate a" + 
                        " generic but functional DuckyScript payload. Do not include comments or" + 
                        " explanations unless they are part of the script. Stick strictly to DuckyScript syntax and commands."
                    },
                    { 
                        role: "user", 
                        content: userMessage 
                    }
                ],
                max_tokens: 150
            })
        });

        if (!response.ok) {
            throw new Error(`Error: ${response.status}`);
        }

        const data = await response.json();
        //const botReply = data.choices[0]?.message?.content || "No response from AI.";
        botReply = data.choices[0]?.message?.content || "No response from AI.";

        appendMessage(botReply, "bot");
    } catch (error) {
        console.error("Error:", error);
        appendMessage("Error: Unable to process request.", "bot");
    }
}

document.getElementById("userInput").addEventListener("keypress", async (event) => {
    if (event.key === "Enter") {
        event.preventDefault();
        document.getElementById("sendBtn").click();
    }
});

function appendMessage(message, sender) {
    const chatMessages = document.getElementById("chatMessages");
    const messageDiv = document.createElement("div");
    messageDiv.classList.add("chat-message", sender);
    messageDiv.innerText = message;
    chatMessages.appendChild(messageDiv);
    chatMessages.scrollTop = chatMessages.scrollHeight;
}

// document.getElementById("runBtn").addEventListener("click", async () => {
//     createFile();
// });

async function createFile() {
    const tempName = "/$temp.txt";

    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(tempName)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: botReply
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error creating file: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error creating file:", err);
        showNotification("Error creating file: " + err.message, "error");
    }
}

export { sendMessage };
