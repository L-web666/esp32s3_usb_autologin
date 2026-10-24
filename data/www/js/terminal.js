//terminal.js
console.log('terminal.js loaded');

import {WS_URL} from './config.js';

console.log(`WebSocket URL: ${WS_URL}`);

(() => {
    const terminal = document.getElementById('terminal');
    const input = document.getElementById('input');
    let socket;

    // Function to append messages to the terminal
    function appendMessage(message, isOwnMessage = false) {
        const msgDiv = document.createElement('div');
        msgDiv.textContent = (isOwnMessage ? 'You: ' : 'Server: ') + message;
        terminal.appendChild(msgDiv);
        terminal.scrollTop = terminal.scrollHeight; // Auto-scroll to bottom
    }

    // Function to clear the terminal content
    function clearTerminal() {
        terminal.innerHTML = ''; // Remove all child elements from the terminal
    }

    // Initialize WebSocket connection
    function initWebSocket() {
        socket = new WebSocket(WS_URL);

        socket.onopen = () => {
            appendMessage("Connected to the server.", false);
        };

        socket.onmessage = (event) => {
            appendMessage(event.data, false);
        };

        socket.onclose = (event) => {
            appendMessage("Disconnected from the server.", false);
            // Optionally attempt to reconnect after a delay
            setTimeout(initWebSocket, 3000);
        };

        socket.onerror = (error) => {
            appendMessage("WebSocket error: " + error, false);
        };
    }

    // Handle user input
    input.addEventListener('keydown', (event) => {
        if (event.key === 'Enter') {
            const message = input.value.trim();
            if (message !== "") {
                if (message.toLowerCase() === "clear") {
                    // If the user types "clear", clear the terminal
                    clearTerminal();
                } else if (socket.readyState === WebSocket.OPEN) {
                    socket.send(message);
                    appendMessage(message, true);
                } else {
                    appendMessage("Cannot send message. WebSocket is not open.", false);
                }
                input.value = ''; // Clear the input field
            }
        }
    });

    // Initialize the WebSocket connection on page load
    window.addEventListener('load', initWebSocket);
})();
