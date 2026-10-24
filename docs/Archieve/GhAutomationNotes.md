https://api.github.com/repos/mdelgert/ImprovWiFiWeb/releases

// Current releases
https://github.com/mdelgert/ImprovWiFiWeb/releases/download/v1.0.0/test.bin
https://github.com/mdelgert/ImprovWiFiWeb/releases/download/v1.0.1/test.bin

//index.html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ESP32 Web Installer</title>
    <link rel="stylesheet" href="css/styles.css" />
  </head>
  <body>
    <div class="content">
      <h1>Web Installer 1.0</h1>
      <div>
        <label for="firmware-select">Select Firmware:</label>
        <select id="firmware-select" class="dropdown">
          <option value="firmware/v1.0.0.json">v1.0.0</option>
          <option value="firmware/v1.0.1.json">v1.0.1</option>
        </select>
      </div>
      <br />
      <esp-web-install-button
        id="install-button"
        manifest="firmware/manifest1.json"
        class="firmware-installer"
      ></esp-web-install-button>
    </div>
    <script type="module">
      import 'esp-web-tools';
      const firmwareSelect = document.getElementById('firmware-select');
      const installButton = document.getElementById('install-button');

      firmwareSelect.addEventListener('change', (event) => {
        const selectedManifest = event.target.value;
        installButton.setAttribute('manifest', selectedManifest);
      });
    </script>
  </body>
</html>

//firmware/v1.0.0.json
{
    "name": "Test",
    "version": "v1.0.0",
    "home_assistant_domain": "esphome",
    "funding_url": "https://github.com/mdelgert/ImprovWiFiWeb",
    "new_install_prompt_erase": false,
    "builds": [
      {
        "chipFamily": "ESP32-S3",
        "parts": [
          { "path": "v1.0.0.bin", "offset": 0 }
        ]
      }
    ]
}

//build-and-release-pio.yml
name: PlatformIO Build

on:
  push:
    tags:
      - "v*.*.*"

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout code
        uses: actions/checkout@v3

      - name: Set up Python
        uses: actions/setup-python@v4
        with:
          python-version: 3.11

      - name: Set up PlatformIO
        run: |
          python3 -m pip install -U pip
          pip install platformio

      - name: Build firmware
        run: |
          pio run --environment esp32-s3-devkitc-1

      - name: Debug list firmware directory
        run: ls -R .pio/build/esp32-s3-devkitc-1

      - name: Create GitHub Release
        uses: softprops/action-gh-release@v1
        with:
          files: |
            .pio/build/esp32-s3-devkitc-1/test.bin
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

//deploy-github-pages.yml
name: GitHub Pages

on:
  workflow_run:
    workflows:
      - PlatformIO Build
    types:
      - completed

jobs:
  build-deploy:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v3

      - name: Set up Node.js
        uses: actions/setup-node@v3
        with:
          node-version: 18

      - name: Cache Node modules
        uses: actions/cache@v3
        with:
          path: ~/.npm
          key: ${{ runner.os }}-node-${{ hashFiles('**/package-lock.json') }}
          restore-keys: |
            ${{ runner.os }}-node-

      - name: Install dependencies
        working-directory: site
        run: npm install

      - name: Clean dist folder
        run: rm -rf site/dist

      - name: Build website
        working-directory: site
        run: NODE_ENV=production npm run build

      - name: Download firmware from GitHub Release
        run: |
          curl -L -o site/dist/firmware/test.bin https://github.com/mdelgert/ImprovWiFiWeb/releases/download/v1.0.0/test.bin

      - name: Debug list build output
        run: ls -R site/dist

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v4
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: site/dist
