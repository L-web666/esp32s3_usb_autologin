### Not working at this time
https://community.platformio.org/t/solved-how-to-exclude-a-file-from-the-build/1031
https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/

When you upload a LittleFS (or SPIFFS) “data” folder in PlatformIO, there’s a mechanism to include/exclude files automatically, so only the files you want end up being flashed. This is handled by the `upload_data_files_filter` directive in your platformio.ini. 

Below is a quick rundown of how to set it up:

---

## 1. Basic Idea

In your `platformio.ini`, each `[env:xxxx]` section can define its own filters for which files to upload from the `data/` folder. By default, **all** files in `data/` get uploaded to the filesystem. The filter line in `platformio.ini` looks like this:

```ini
upload_data_files_filter = <filter rules>
```

Here you can use:
- `+<pattern>` to **include** files matching a pattern.
- `-<pattern>` to **exclude** files matching a pattern.

PlatformIO uses [Git-like glob patterns](https://git-scm.com/docs/gitignore#_pattern_format) where you can match on file names, extensions, and directories. Typical patterns are `*.txt`, `*.json`, `some_subfolder/*`, etc.

---

## 2. Common Examples

### Include all, exclude just a few

If you want to include everything but exclude a single file:
```ini
[env:myboard]
platform = espressif32
board = esp32dev
framework = arduino

; Include all, exclude one specific file
upload_data_files_filter = +<*> -<secret_config.json>
```
- `+<*>` means “take all files in data/”.
- `-<secret_config.json>` means “exclude exactly `secret_config.json` (wherever it is)”.

### Excluding an entire folder

If you have a subfolder that you never want in your final filesystem:
```ini
[env:myboard]
platform = espressif32
board = esp32dev
framework = arduino

upload_data_files_filter = +<*> -<exclude_this_folder/>
```
This says:
- `+<*>`: include everything,
- `-<exclude_this_folder/>`: but exclude the directory `exclude_this_folder/` and everything under it.

### Being selective

If you only want to include certain types of files, you could do:
```ini
[env:myboard]
upload_data_files_filter = +<*.txt> +<*.json> -<*>
```
- `+<*.txt>` and `+<*.json>` means you explicitly include these extensions.
- `-<*>` at the end excludes everything else.  
The order matters: once you exclude everything, you only bring back in the patterns you prefixed with `+`.

---

## 3. Verify It Works

1. **Check**: After editing `platformio.ini`, re-run the LittleFS upload (in VSCode, for example, by selecting “Upload File System Image”).  
2. **Inspect**: You can confirm the final contents by using an Arduino/ESP check or by reading the directory structure in your firmware.

---

## 4. Quick Reference

- **File**: `platformio.ini`
- **Directive**: `upload_data_files_filter`
- **Syntax**: `+<pattern>` to include, `-<pattern>` to exclude
- **Default**: If `upload_data_files_filter` isn’t specified, **all** files in `data` are uploaded.

For more details, see the [PlatformIO docs on `upload_data_files_filter`](https://docs.platformio.org/en/latest/projectconf/section_env_upload.html#upload-data-files-filter).

---

**That’s it!** By customizing `upload_data_files_filter`, you can precisely control which files from your `data/` folder end up in the device’s filesystem when using LittleFS.

Here’s a simple, self-contained example that excludes two specific folders and one file in the `data/` folder. Assume your `platformio.ini` looks like this:

```ini
[env:myboard]
platform = espressif32
board = esp32dev
framework = arduino

; Include everything, but exclude:
;   1. folder_one/
;   2. folder_two/
;   3. secret_config.json
upload_data_files_filter = +<*> -<folder_one/> -<folder_two/> -<secret_config.json>
```

Explanation:

1. `+<*>` includes **all files and subfolders** from `data/`.  
2. `-<folder_one/>` excludes the folder named `folder_one` and everything inside it.  
3. `-<folder_two/>` excludes the folder named `folder_two` and everything inside it.  
4. `-<secret_config.json>` excludes the single file `secret_config.json` wherever it appears under `data/`.  

With these rules, only the allowed files and folders remain in the final filesystem image.

If `node_modules` is still being uploaded after setting:

```ini
upload_data_files_filter = +<*> -<node_modules/>
```

it usually means the exclusion pattern isn’t matching how PlatformIO sees that folder path. Below are some troubleshooting steps and alternate patterns you can try:

---

## 1. Confirm You Are in the Correct `[env:xxxx]`

Make sure your `platformio.ini` has the correct environment section and that you are using (or selecting in VSCode) the same environment when you do the upload. For example:

```ini
[env:myboard]
platform = espressif32
board = esp32dev
framework = arduino

upload_data_files_filter = +<*> -<node_modules/>
```

- Then ensure you run `Upload File System Image` on that **same** environment (`myboard`).  
- Or explicitly run from the terminal:
  ```bash
  pio run -t uploadfs -e myboard
  ```

---

## 2. Try a Different Exclusion Pattern

Sometimes, you need to exclude recursively, or the trailing slash is not recognized. Test these variations:

```ini
upload_data_files_filter =
  +<*>
  -<node_modules/**>
```

**Why?**  
- `-<node_modules/**>` means “exclude any file or folder inside `node_modules`, recursively.”  
- Depending on your PlatformIO version, sometimes `-<node_modules/>` won’t catch deeper subdirectories or hidden files.

You can also try:
```ini
upload_data_files_filter =
  +<*>
  -<node_modules*>
```
That catches anything starting with `node_modules`, including `node_modules/`.

---

## 3. Check Folder Placement

PlatformIO’s LittleFS build process looks in your `data` folder (by default). If your file tree is like:
```
project/
  ├─ src/
  ├─ data/
  │   └─ node_modules/
  ├─ platformio.ini
```
Then your filter patterns (like `-<node_modules/**>`) should work. But if `node_modules` is inside a sub-subfolder, e.g. `data/assets/node_modules/`, you might need:

```ini
upload_data_files_filter =
  +<*>
  -<assets/node_modules/**>
```

---

## 4. Clean/Rebuild

PlatformIO can cache results. After updating the filter, do:
```
pio run -t clean
pio run -t uploadfs -e myboard
```
to ensure it picks up your new filter rules.

---

## 5. Verify with Verbose Output

From a terminal in your project folder:
```bash
pio run -t uploadfs -e myboard -v
```
This shows which files are actually being picked up. It can help confirm whether `node_modules` is excluded or not and which pattern is (or isn’t) taking effect.

---

## 6. Last Resort: Remove `node_modules` from `data/`

Most people don’t actually need `node_modules` in the final LittleFS. If you’re only using it for local web dev or build tooling, consider placing `node_modules` **outside** the `data` folder altogether.

---

### TL;DR

- Make sure you’re applying `upload_data_files_filter` **in the environment you’re actually building**.
- Use a more specific pattern like `-<node_modules/**>` or `-<assets/node_modules/**>` if it’s nested.
- Clean and run again with verbose output to confirm the filter took effect.
- If all else fails, move `node_modules` outside of `data/` entirely, because it’s typically not needed on the device.