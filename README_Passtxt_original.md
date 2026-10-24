
---

# PassTxt: Hardware-Based Password and Script Manager

**PassTxt** is a compact, powerful hardware device designed to securely manage passwords, execute automation scripts, and streamline workflows across all your devices. Powered by the [ESP32-S3 microcontroller](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) from Espressif Systems, it functions as a plug-and-play USB keyboard and mouse, requiring no software installation on the host device. With control options via a web browser, Bluetooth, or cloud services, *PassTxt* offers universal compatibility and versatility for users ranging from developers and gamers to studio professionals and everyday individuals.

---

## Key Features

- **Secure Password Management**: Store and autofill complex passwords on any USB-enabled device without additional software.
- **Automation Scripting**: Execute custom scripts, including [Ducky Script](https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript), for keystroke simulation and mouse control.
- **Macro Keys**: Program single-press commands for multi-step tasks, enhancing productivity in gaming, studios, or offices.
- **Multiple Control Options**: Manage via a web interface, Bluetooth, or cloud integration with [EMQX](https://www.emqx.com/en) (MQTT broker).
- **Built-In Code Editor**: Edit scripts and configuration files directly on the device using [CodeMirror](https://codemirror.net/).
- **AI-Powered Scripting**: Generate intelligent automation scripts with [Azure OpenAI](https://azure.microsoft.com/en-us/products/ai-services/openai-service/).

---

## Hardware and Compatibility

- **Microcontroller**: Runs on the [ESP32-S3](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf) by Espressif Systems.
- **Off-the-Shelf Hardware**: Compatible with the [LILYGO T-Dongle-S3](https://lilygo.cc/products/t-dongle-s3), an affordable and compact development board.
- **Device Compatibility**: Works with any USB-enabled device, including PCs, laptops, phones, tablets, smart TVs, and more.

---

## Development and Tools

- **Development Platform**: Built using [PlatformIO](https://platformio.org/), a professional collaborative platform for embedded development.
- **Built-In Editor**: Features a web-based code editor powered by [CodeMirror](https://codemirror.net/), allowing users to edit all scripts and configuration files directly on the device.

### Popular Libraries Used

- [RemoteDebug](https://github.com/JoaoLopesF/RemoteDebug) – Wi-Fi-based debugging.
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) @1.4.2 – Bluetooth Low Energy (BLE) support.
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX) @1.2.0 – High-performance graphics library.
- [OneButton](https://github.com/mathertel/OneButton) @2.6.1 – Simplified button handling.
- [FastLED](https://github.com/FastLED/FastLED) @3.9.9 – LED control library.
- [PubSubClient](https://github.com/knolleary/pubsubclient) @2.8 – MQTT client for IoT connectivity.
- [Improv WiFi Library](https://github.com/jnthas/Improv-WiFi-Library) @0.0.2 – Easy Wi-Fi provisioning.
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) @1.2.4 – Asynchronous web server.
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson) @7.3.0 – JSON parsing and generation.
- [base64_encode](https://github.com/dojyorin/base64_encode) @2.0.4 – Base64 encoding utility.

---

## Cloud and AI Integration

- **MQTT Compatibility**: Integrates with [EMQX](https://www.emqx.com/en), a scalable MQTT broker for IoT and cloud connectivity.
- **Ducky Script Agent with AI**: Uses [Azure OpenAI](https://azure.microsoft.com/en-us/products/ai-services/openai-service/) to power a Ducky Script agent, enabling intelligent script generation and automation.

---

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) installed on your development machine.
- [LILYGO T-Dongle-S3](https://lilygo.cc/products/t-dongle-s3) or a compatible ESP32-S3-based device.
- A Wi-Fi network for initial setup.

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/mdelgert/PassTxt.git
   ```
2. Open the project in PlatformIO:
   ```bash
   pio run
   ```
3. Upload the firmware to your ESP32-S3 device:
   ```bash
   pio run --target upload
   ```

### Basic Usage

1. **Connect the Device**: Plug *PassTxt* into a USB port on your target device.
2. **Access the Web Interface**: Connect to the device’s Wi-Fi access point or your local network, then open the web interface in a browser.
3. **Edit Scripts**: Use the built-in [CodeMirror](https://codemirror.net/) editor to create or modify scripts.
4. **Execute Actions**: Trigger scripts or macros via the web interface, Bluetooth, or physical buttons.

For more detailed instructions, refer to the [User Guide](docs/user-guide.md).

---

## Future Features

We’re excited to expand *PassTxt* with these planned enhancements:

- **Azure Event Grid Compatibility**: Enable real-time, event-driven automation with Azure’s cloud services.
- **Voice Control**: Add hands-free operation with voice command support.
- **Multi-Device Sync**: Synchronize scripts and settings across multiple *PassTxt* devices.
- **Advanced AI Features**: Enhance script generation with predictive automation and natural language processing.
- **Broader Hardware Support**: Extend compatibility to additional ESP32 variants and accessories.

---

## Contributing

Contributions are welcome! Whether you’re fixing bugs, adding features, or improving documentation, we’d love your help. Please review our [Contribution Guidelines](CONTRIBUTING.md) before submitting a pull request.

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Acknowledgments

- [Espressif Systems](https://www.espressif.com/) for the ESP32-S3 microcontroller.
- [LILYGO](https://lilygo.cc/) for the T-Dongle-S3 hardware.
- [PlatformIO](https://platformio.org/) for the development environment.
- [CodeMirror](https://codemirror.net/) for the built-in code editor.
- [Azure OpenAI](https://azure.microsoft.com/en-us/products/ai-services/openai-service/) for AI-powered scripting.
- [EMQX](https://www.emqx.com/en) for MQTT broker support.

---

## Contact

Have questions or suggestions? Open an issue on GitHub or email us at [mdelgert@yahoo.com](mailto:mdelgert@yahoo.com).

---

Below is a section you can add to your GitHub repository’s README to describe the **Web Installer** feature over serial using [Improv WiFi](https://www.improv-wifi.com/), along with details about the current release and installer built with GitHub Actions.

---

## Web Installer via Serial with Improv WiFi

*PassTxt* includes a user-friendly **Web Installer** powered by [Improv WiFi](https://www.improv-wifi.com/), enabling you to configure your device’s Wi-Fi settings effortlessly through a web browser. This eliminates the need for manual configuration files or complicated setup procedures, streamlining the process of getting *PassTxt* online.

### How It Works

1. **Connect via Serial**: Plug your *PassTxt* device into your computer using a USB cable.
2. **Access the Web Installer**: Open your web browser and navigate to the [Improv WiFi Web Installer](https://www.improv-wifi.com/).
3. **Select Your Device**: From the list of supported devices, choose *PassTxt*.
4. **Set Up Wi-Fi**: Input your Wi-Fi network credentials to connect *PassTxt* to your network.

After completing these steps, *PassTxt* will automatically connect to your specified Wi-Fi network, ready for immediate use—no further configuration required!

### Current Release and Installer

The latest version of *PassTxt*, along with its web installer, is built and deployed using **GitHub Actions**. You can find the most recent release and access the installer here:

**[PassTxt Web Installer](https://mdelgert.github.io/PassTxt)**

This link ensures you’re always using the latest version of *PassTxt*, complete with the most recent updates and enhancements.

---