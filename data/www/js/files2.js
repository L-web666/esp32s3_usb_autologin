//files.js
console.log("files.js loaded");

const endPoint = window.location.hostname === "localhost" ? `http://demo1.local` : "";

let fontSize = '18px';
let currentPath = "/";
let currentFile = "";

CodeMirror.defineSimpleMode("custom", {
    start: [
        { regex: /\b(?:rem|defaultdelay|delay|repeat)\b/i, token: "keyword" },
        { regex: /\b(?:aes|hid|crontab|device|download|jiggle|led|mqtt|script|tft|wget)\b/i, token: "command" },
        { regex: /\b(?:color|processkey|tapkey|add|enc|dec|print|brightness|on|off|toggle|msg)\b/i, token: "subcommand" },
        { regex: /\b\d+\b/, token: "number" },
        { regex: /"(?:[^\\]|\\.)*?"/, token: "string" },
        { regex: /\brem\b.*$/i, token: "comment" },
        { regex: /\b[a-zA-Z_]\w*\b/, token: null } // Identifiers (e.g., hello1)
    ],
    meta: {
        lineComment: "REM"
    }
});

const editor = CodeMirror(document.getElementById("editorContainer"), {
    mode: "text/plain",
    theme: "material-darker",
    lineNumbers: true,
    tabSize: 4,
    indentWithTabs: true
});

const mixedMode = {
    name: "htmlmixed",
    scriptTypes: [{matches: /\/x-handlebars-template|\/x-mustache/i,
                   mode: null},
                  {matches: /(text|application)\/(x-)?vb(a|script)/i,
                   mode: "vbscript"}]
};

function changeFontSize() {
    const fontSize = document.getElementById('fontSizeSelector').value;
    document.querySelector('.CodeMirror').style.fontSize = fontSize;
    editor.refresh();
}

export function toggleToolsModal() {
    document.getElementById("toolsModal").classList.toggle("active");
    document.getElementById("toolsModalOverlay").classList.toggle("active");
    document.getElementById("modalMessage").textContent = "";
}

function updateStatus(message) {
    document.getElementById("modalMessage").textContent = message;
}

function formatJson() {
    const content = editor.getValue();
    try {
        const formatted = JSON.stringify(JSON.parse(content), null, 2);
        editor.setValue(formatted);
        updateStatus("JSON formatted successfully.");
    } catch {
        updateStatus("Invalid JSON. Unable to format.");
    }
    toggleModal();
}

function minifyJson() {
    const content = editor.getValue();
    try {
        const minified = JSON.stringify(JSON.parse(content));
        editor.setValue(minified);
        updateStatus("JSON minified successfully.");
    } catch {
        updateStatus("Invalid JSON. Unable to minify.");
    }
    toggleModal();
}

function validateJson() {
    const content = editor.getValue();
    try {
        JSON.parse(content);
        updateStatus("JSON is valid.");
    } catch {
        updateStatus("Invalid JSON.");
    }
    toggleModal();
}

function showNotification(message, type = "info") {
    const statusMessage = document.getElementById("statusMessage");
    statusMessage.textContent = message;
    statusMessage.style.color = type === "error" ? "#ff5555" : "#ccc";
}

function breadcrumbPath(path) {
    const breadcrumbs = document.getElementById("breadcrumbs");
    breadcrumbs.innerHTML = path;
}

async function refreshFiles() {
    try {
        const response = await fetch(`${endPoint}/filemanager?path=${encodeURIComponent(currentPath)}`);
        const data = await response.json();

        if (data.status !== "success") {
            showNotification(`Error fetching directory contents: ${data.message}`, "error");
            return;
        }

        const fileList = document.getElementById("fileList");
        fileList.innerHTML = "";

        //updateBreadcrumbs(currentPath);
        breadcrumbPath(currentPath);

        if (currentPath !== '/') {
            const backButton = document.createElement('li');
            backButton.textContent = '.. (Back)';
            backButton.onclick = goBack;
            fileList.appendChild(backButton);
        }

        data.data.folders.forEach(folder => {
            const li = document.createElement("li");
            li.classList.add("folder");
            li.innerHTML = `
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor" class="icon">
            <path d="M19.5 21a3 3 0 0 0 3-3v-4.5a3 3 0 0 0-3-3h-15a3 3 0 0 0-3 3V18a3 3 0 0 0 3 3h15ZM1.5 10.146V6a3 3 0 0 1 3-3h5.379a2.25 2.25 0 0 1 1.59.659l2.122 2.121c.14.141.331.22.53.22H19.5a3 3 0 0 1 3 3v1.146A4.483 4.483 0 0 0 19.5 9h-15a4.483 4.483 0 0 0-3 1.146Z" />
        </svg>
        <span>${folder}</span>`;
            li.onclick = () => openFolder(folder);
            fileList.appendChild(li);
        });

        data.data.files.forEach(file => {
            const li = document.createElement("li");
            li.classList.add("file");
            li.innerHTML = `
        <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="currentColor" class="icon">
            <path fill-rule="evenodd" d="M5.625 1.5c-1.036 0-1.875.84-1.875 1.875v17.25c0 1.035.84 1.875 1.875 1.875h12.75c1.035 0 1.875-.84 1.875-1.875V12.75A3.75 3.75 0 0 0 16.5 9h-1.875a1.875 1.875 0 0 1-1.875-1.875V5.25A3.75 3.75 0 0 0 9 1.5H5.625ZM7.5 15a.75.75 0 0 1 .75-.75h7.5a.75.75 0 0 1 0 1.5h-7.5A.75.75 0 0 1 7.5 15Zm.75 2.25a.75.75 0 0 0 0 1.5H12a.75.75 0 0 0 0-1.5H8.25Z" clip-rule="evenodd" />
            <path d="M12.971 1.816A5.23 5.23 0 0 1 14.25 5.25v1.875c0 .207.168.375.375.375H16.5a5.23 5.23 0 0 1 3.434 1.279 9.768 9.768 0 0 0-6.963-6.963Z" />
        </svg>
        <span>${file}</span>`;
            li.onclick = () => openFile(file);
            fileList.appendChild(li);
        });

    } catch (err) {
        console.error("Error refreshing file list:", err);
        showNotification("Error fetching directory contents", "error");
    }
}

function highlightActiveFile(filename) {
    const fileListElement = document.getElementById('fileList');
    Array.from(fileListElement.children).forEach(li => {
        const span = li.querySelector('span');
        if (span && span.textContent === filename) {
            li.classList.add('active');
        } else {
            li.classList.remove('active');
        }
    });
}

async function openFile(file) {
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + file)}`);
        const content = await response.text();
        currentFile = file;
        showNotification(currentFile);
        highlightActiveFile(currentFile);
        editor.setValue(content);

        let mode;
        if (currentFile.endsWith('.json')) {
            mode = 'application/json';
        } else if (currentFile.endsWith('.html')) {
            //mode = 'text/html';
            mode = mixedMode; // Use the custom mode spec for HTML
        } else if (currentFile.endsWith('.css')) {
            mode = 'css';
        } else if (currentFile.endsWith('.js')) {
            mode = 'javascript';
        } else if (currentFile.endsWith('.sh')) {
            mode = 'shell';
        } else if (currentFile.endsWith('.ps1')) {
            mode = 'powershell';
        } else if (currentFile.endsWith('.txt')) {
            mode = 'custom';
        } else {
            mode = 'text/plain';
        }

        editor.setOption('mode', mode);
    } catch (err) {
        console.error("Error opening file:", err);
        showNotification("Error opening file", "error");
    }
}

async function downloadFile() {
    
    const filename = currentPath + currentFile;

    try {
        // Fetch the file content from the server
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(filename)}`);
        if (!response.ok) {
            throw new Error(`Failed to download file: ${response.statusText}`);
        }

        // Convert the response to a blob
        const blob = await response.blob();

        // Create a temporary anchor element
        const downloadLink = document.createElement('a');
        downloadLink.href = URL.createObjectURL(blob);

        // Set the file name for the download
        downloadLink.download = currentFile;

        // Append the anchor to the body (necessary for Firefox)
        document.body.appendChild(downloadLink);

        // Programmatically click the anchor to start the download
        downloadLink.click();

        // Clean up
        URL.revokeObjectURL(downloadLink.href); // Free memory
        document.body.removeChild(downloadLink);
    } catch (err) {
        console.error("Error downloading file:", err);
        showNotification("Error downloading file", "error");
    }
}

async function uploadFile() {
    const fileInput = document.getElementById('fileInput');
    const file = fileInput.files[0];

    if (!file) {
        showNotification("No file selected.", "error");
        return;
    }

    const filename = currentPath + file.name;

    // Read the file content
    const fileContent = await file.text();

    try {
        // Send the file content to the server
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(filename)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: fileContent,
        });

        const result = await response.json();
        if (result.status === 'success') {
            showNotification("File uploaded successfully");
            refreshFiles(); // Refresh the file list after upload
        } else {
            showNotification("Error uploading file: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error uploading file:", err);
        showNotification("Error uploading file", "error");
    }
}

function openFolder(folder) {
    currentPath = `${currentPath}${folder}/`;
    refreshFiles();
}

function goBack() {
    currentPath = currentPath.substring(0, currentPath.lastIndexOf('/', currentPath.length - 2) + 1) || '/';
    refreshFiles();
}

async function saveFile() {
    if (!currentFile) {
        showNotification("No file selected.", "error");
        return;
    }

    const content = editor.getValue();

    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + currentFile)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/octet-stream' },
            body: content,
            timeout: 60000
        });
        const result = await response.json();
        if (result.status === "success") {
            showNotification("File saved successfully");
        } else {
            showNotification("Error saving file: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error saving file:", err);
        showNotification("Error saving file", "error");
    }
}

async function createFile() {
    const fileName = prompt("Enter new file name:");
    if (!fileName) return;
    try {
        const response = await fetch(`${endPoint}/file?filename=${encodeURIComponent(currentPath + fileName)}`, {
            method: 'POST',
            headers: { 'Content-Type': 'text/plain' },
            body: ' ' // Create an empty file
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

async function createFolder() {
    const folderName = prompt("Enter new folder name:");
    if (!folderName) return;
    try {
        const response = await fetch(`${endPoint}/folder?foldername=${encodeURIComponent(currentPath + folderName)}`, {
            method: 'POST'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error creating folder: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error creating folder:", err);
        showNotification("Error creating folder: " + err.message, "error");
    }
}

async function runFile() {
    const filename = currentPath + currentFile;

    if (!currentFile) {
        showNotification('No file selected to run.', 'error');
        return;
    }

    try {
        const response = await fetch(`${endPoint}/command/set`, {
            method: 'POST',
            headers: {
                'Content-Type': 'text/plain',
            },
            //body: `script file ${filename}`,
            body: `ducky file ${filename}`,
        });

        const data = await response.json();
        if (data.status === 'success') {
            showNotification(`Command executed successfully for ${filename}!`);
        } else {
            showNotification(`Error executing command: ${data.message}`, 'error');
        }
    } catch (err) {
        console.error('Error running file:', err);
        showNotification('Failed to execute command.', 'error');
    }

    showNotification(`Running file ${filename}`);
}

async function deleteItem(isFolder) {

    const endpoint = isFolder ? '/folder?foldername=' : '/file?filename=';
    const item = isFolder ? currentPath : currentPath + currentFile;
    const fullPath = `${endPoint}${endpoint}${encodeURIComponent(item)}`;
    const confirmed = window.confirm(`Are you sure you want to delete ${item}?`);

    if (!confirmed) {
        return;
    }

    try {
        const response = await fetch(`${fullPath}`, {
            method: 'DELETE'
        });
        const result = await response.json();
        if (result.status === "success") {
            isFolder ? goBack() : refreshFiles();
            showNotification("Item deleted successfully");
        } else {
            showNotification("Error deleting item: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error deleting item:", err);
        showNotification("Error deleting item: " + err.message, "error");
    }
}

async function renameItem() {
    if (!currentFile) {
        showNotification('No file selected to rename.', 'error');
        return;
    }

    const newName = prompt("Enter new name:");

    if (!newName) return;
    try {
        const response = await fetch(`${endPoint}/rename?oldname=${currentPath}${currentFile}&newname=${currentPath}${newName}`, {
            method: 'POST'
        });
        const result = await response.json();
        if (result.status === "success") {
            refreshFiles();
        } else {
            showNotification("Error renaming item: " + result.message, "error");
        }
    } catch (err) {
        console.error("Error renaming item:", err);
        showNotification("Error renaming item: " + err.message, "error");
    }
}

function setDefaultFontSize() {
    const selector = document.getElementById("fontSizeSelector");
    selector.value = fontSize;
    document.querySelector('.CodeMirror').style.fontSize = fontSize;
}

// Function to initialize event listeners
function attachEventListeners() {

    document.getElementById("fontSizeSelector").addEventListener("change", changeFontSize);

    document.getElementById("newFileBtn").addEventListener("click", createFile);
    document.getElementById("deleteFileBtn").addEventListener("click", () => deleteItem(false));

    document.getElementById("newFolderBtn").addEventListener("click", createFolder);
    document.getElementById("deleteFolderBtn").addEventListener("click", () => deleteItem(true)); // Pass true

    document.getElementById("renameBtn").addEventListener("click", renameItem);
    document.getElementById("saveBtn").addEventListener("click", saveFile);

    document.getElementById("runBtn").addEventListener("click", runFile);
    document.getElementById("renameBtn").addEventListener("click", renameItem);
    document.getElementById("downloadBtn").addEventListener("click", downloadFile);
    
    document.getElementById("toolsModalOverlay").addEventListener("click", toggleToolsModal);
    document.getElementById("toolsBtn").addEventListener("click", toggleToolsModal);
    document.getElementById("formatJsonBtn").addEventListener("click", formatJson);
    document.getElementById("minifyJsonBtn").addEventListener("click", minifyJson);
    document.getElementById("validateJsonBtn").addEventListener("click", validateJson);
}

// Initialize everything when DOM is ready
document.addEventListener("DOMContentLoaded", () => {
    attachEventListeners();  // Attach all event listeners
    refreshFiles();          // Load file list after the page is ready
    setDefaultFontSize();    // Apply default font size after the page loads
});



