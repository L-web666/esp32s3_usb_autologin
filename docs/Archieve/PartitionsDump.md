The `esptool.py` command you tried does not include a direct `partition_table` operation. However, you can extract and inspect the partition table using the following steps:

---

### Step 1: Read the Partition Table from Flash
The ESP32 partition table is typically located at **offset 0x8000** in the flash memory. You can read this section using `esptool.py`:

```bash
esptool.py --port /dev/ttyACM0 read_flash 0x8000 0x1000 partition_table.bin
```

- Replace `/dev/ttyACM0` with your actual serial port.
- This command reads the partition table (4KB = 0x1000) starting at the default offset (0x8000) into a file called `partition_table.bin`.

---

### Step 2: Inspect the Partition Table
To inspect the contents of the partition table, you can use the `parttool.py` utility, which is included with the ESP-IDF.

#### Option A: Using `parttool.py` (Requires ESP-IDF)
1. Run the following command to inspect the extracted partition table:
   ```bash
   python $IDF_PATH/components/partition_table/parttool.py --partition-table-file partition_table.bin list
   ```
   - This lists the partitions and their details, including offsets, sizes, and types.

---

### Step 3: Validate the Table
Compare the extracted partition table with your custom `partitions.csv` to identify any discrepancies. Ensure:
1. The offsets and sizes do not overlap.
2. The total size fits within your flash memory (16MB = 0x1000000).

---

### If You Don’t Have `parttool.py`
If you're not using ESP-IDF, you can manually inspect the binary with a hex editor or parse it using a script.

#### Example Hexdump Command:
```bash
hexdump -C partition_table.bin
```

The partition table format is described in the [ESP-IDF Partition Table documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html).

---

### Next Steps
Once you have the extracted partition table, compare its details against your custom `partitions.csv` and let me know the results. We can further refine your partition configuration based on this information.

Your hexdump reveals the contents of a valid partition table from the ESP32 flash memory. Let's break it down and analyze it against your expectations.

---

### Decoding the Partition Table
Each partition entry in the table is 32 bytes, starting with the magic byte sequence `aa 50`.

#### **Extracted Partition Table**
| Offset (hex) | Name      | Type  | SubType | Start Addr (hex) | Size (hex) | Description      |
|--------------|-----------|-------|---------|------------------|------------|------------------|
| `0x0000`     | `nvs`     | `data`| `nvs`   | `0x9000`         | `0x5000`   | Non-volatile storage |
| `0x0020`     | `otadata` | `data`| `ota`   | `0xe000`         | `0x2000`   | OTA data partition |
| `0x0040`     | `app0`    | `app` | `ota_0` | `0x10000`        | `0x330000` | OTA app slot 0   |
| `0x0060`     | `app1`    | `app` | `ota_1` | `0x340000`       | `0x330000` | OTA app slot 1   |
| `0x0080`     | `spiffs`  | `data`| `spiffs`| `0x670000`       | `0x180000` | SPIFFS storage   |
| `0x00a0`     | `coredump`| `data`| `coredump` | `0x7f0000`     | `0x10000`  | Core dump area   |

---

### Comparison Against Your Custom Table
#### Observations:
1. **Partition Alignment**:
   - This table ensures all partitions are aligned and there is no overlap.
   - Partition offsets and sizes are properly configured.

2. **Larger App Slots**:
   - Each OTA slot (`app0` and `app1`) is **0x330000** (3.2MB). In your custom table, they were 4MB each.

3. **Smaller SPIFFS (or LittleFS) Partition**:
   - The extracted table allocates `0x180000` (1.5MB) for `spiffs`, while your custom table allocates 7.8MB for storage.

4. **Missing `phy_init` Partition**:
   - The extracted table does not include a `phy_init` partition, suggesting it is not needed for your board configuration or the firmware.

---

### Updated Custom Partition Table
To create a compatible partition table, align it closer to the working one extracted from the flash. Adjust for LittleFS and OTA requirements:

```csv
# Compatible Partition Table for 16MB Flash with LittleFS
# Name,      Type,   SubType,  Offset,    Size,       Flags
nvs,         data,   nvs,      0x009000,  0x005000    # NVS (20KB)
otadata,     data,   ota,      0x00E000,  0x002000    # OTA data (8KB)
app0,        app,    ota_0,    0x010000,  0x330000    # OTA slot 0 (3.2MB)
app1,        app,    ota_1,    0x340000,  0x330000    # OTA slot 1 (3.2MB)
storage,     data,   spiffs,   0x670000,  0x180000    # LittleFS storage (1.5MB)
coredump,    data,   coredump, 0x7f0000,  0x010000    # Core dump (64KB)
```

---

### Key Adjustments:
1. **OTA Slot Sizes**:
   - Reduced to 3.2MB (`0x330000`), matching the extracted table.

2. **LittleFS Storage**:
   - Reduced to 1.5MB (`0x180000`), aligning with the extracted `spiffs` partition.

3. **No `phy_init` Partition**:
   - Excluded, as it was not present in the extracted table and does not appear necessary.

---

### Steps to Test
1. Save the updated `partitions.csv` file.
2. Update `platformio.ini` to point to the new partition table:
   ```ini
   board_build.partitions = tools/partitions.csv
   board_build.flash_size = 16MB
   board_build.filesystem = littlefs
   ```
3. Erase the flash memory:
   ```bash
   esptool.py erase_flash
   ```
4. Rebuild and upload both the firmware and filesystem:
   ```bash
   pio run --target buildfs
   pio run --target uploadfs
   pio run --target upload
   ```
5. Monitor the serial output:
   ```bash
   pio device monitor --baud 115200
   ```

---

### Expected Result
The board should now:
1. Boot successfully.
2. Use LittleFS for file storage.
3. Support OTA updates with the adjusted partition sizes.

Let me know if this resolves the issue or if further tweaks are needed!

partition_table.bin
00000000  aa 50 01 02 00 90 00 00  00 50 00 00 6e 76 73 00  |.P.......P..nvs.|
00000010  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000020  aa 50 01 00 00 e0 00 00  00 20 00 00 6f 74 61 64  |.P....... ..otad|
00000030  61 74 61 00 00 00 00 00  00 00 00 00 00 00 00 00  |ata.............|
00000040  aa 50 00 10 00 00 01 00  00 00 33 00 61 70 70 30  |.P........3.app0|
00000050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000060  aa 50 00 11 00 00 34 00  00 00 33 00 61 70 70 31  |.P....4...3.app1|
00000070  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
00000080  aa 50 01 82 00 00 67 00  00 00 18 00 73 70 69 66  |.P....g.....spif|
00000090  66 73 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |fs..............|
000000a0  aa 50 01 03 00 00 7f 00  00 00 01 00 63 6f 72 65  |.P..........core|
000000b0  64 75 6d 70 00 00 00 00  00 00 00 00 00 00 00 00  |dump............|
000000c0  eb eb ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
000000d0  46 7e b8 96 e2 9d 1a a9  38 c5 57 f4 cd fc 4c 5b  |F~......8.W...L[|
000000e0  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff ff ff  |................|
*
00001000