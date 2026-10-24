# Flash Download Tools
https://www.espressif.com/en/support/download/other-tools
https://dl.espressif.com/public/flash_download_tool.zip

If your ESP32-S3 has 16MB of detected flash and you have a combined firmware file, here's how to use the **ESP Flash Download Tool** to upload it:

---

### **Steps to Use ESP Flash Download Tool**

1. **Launch the ESP Flash Download Tool**
   - Download and open the ESP Flash Download Tool from [Espressif](https://www.espressif.com/en/support/download/other-tools).

2. **Select the ESP32-S3 Chip**
   - Choose **ESP32-S3** from the list of available chips.

3. **Load the Combined Firmware**
   - Under the "Download Path Config" section, click the "..." button to locate your combined firmware `.bin` file.

4. **Set the Starting Address**
   - For a combined firmware, the starting address is typically **0x0**.
     - This ensures the firmware is written to the correct location starting at the beginning of the flash memory.

5. **Configure the Flash Settings**
   - Check or adjust the following parameters based on your firmware and device:
     - **Crystal Frequency**: 40MHz (as detected).
     - **SPI Flash Size**: Set it to **16MB** (detected size of the flash).
     - **Baud Rate**: Set a suitable baud rate (e.g., 115200 or 921600 for faster uploads).

6. **Connect Your ESP32-S3**
   - Ensure the ESP32-S3 is in **bootloader mode**:
     - Press and hold the **BOOT** button (if available) while pressing the **RESET** button.
     - Release the **RESET** button first, then release the **BOOT** button.

7. **Select the COM Port**
   - Choose the appropriate COM port for your ESP32-S3 under "COM."

8. **Start the Upload**
   - Click **Start** to begin flashing.
   - Monitor the log for any errors or confirmation that the firmware upload was successful.

---

### **Important Notes**
- **Partition Table**: Ensure your combined firmware includes the partition table and bootloader. Most combined firmware files are designed to start at address `0x0` and include all required components.
- **Erasing Flash**: If you encounter issues, you can erase the flash before uploading:
  - Use the **"Erase"** button in the ESP Flash Download Tool.
- **Verify Flash**: Enable the "Verify" checkbox to ensure the firmware was written correctly.

---

### **Common Starting Addresses**
If you're not using a combined firmware or you need to manually upload components:
- **Bootloader**: `0x0`
- **Partition Table**: `0x8000`
- **Application Firmware**: `0x10000`

For a combined firmware, always start at **`0x0`** unless specified otherwise in the documentation.

Let me know if you need more help!