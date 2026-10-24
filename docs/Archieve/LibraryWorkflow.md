# Comprehensive Guide: PlatformIO Library Development with Release Branch

This document explains the entire process of developing a PlatformIO library, organizing your repository for testing, and automating the creation of a `release` branch for sharing your library with others via `lib_deps`. It includes folder structure, setup, automation using GitHub Actions, a fallback bash script, and best practices.

---

## 1. Folder Structure

The following folder structure is recommended for developing your PlatformIO library and testing it in a single repository:

```
RemoteDebug/
|
├── lib/                        # Library development folder
│   ├── RemoteDebug/            # Library folder (develop here)
│   │   ├── src/                # Core library source files
│   │   │   ├── utility/
│   │   │   │   ├── RemoteDebug.cpp
│   │   │   │   ├── RemoteDebug.h
│   │   │   │   └── telnet.h
│   │   │   ├── RemoteDebug.cpp
│   │   │   └── RemoteDebug.h
│   │   ├── examples/           # Example sketches
│   │   │   ├── BasicDebug/
│   │   │   │   └── BasicDebug.ino
│   │   │   └── WiFiDebug/
│   │   │       └── WiFiDebug.ino
│   │   ├── library.json        # Metadata for PlatformIO
│   │   ├── library.properties  # Metadata for Arduino IDE
│   │   └── LICENSE.txt         # License
|
├── src/                        # PlatformIO test project
│   └── main.cpp                # Test code for the library
|
├── platformio.ini              # PlatformIO configuration file
├── README.md                   # Documentation
├── .gitignore                  # Git ignore rules
└── .github/
    └── workflows/
        └── release-branch.yml  # GitHub Action for automating `release` branch
```

---

## 2. Setting Up the Repository

1. **Initialize the Repository**:
   - Create a new repository on GitHub.
   - Clone it locally and set up the folder structure above.

2. **Develop the Library**:
   - Place your library code inside `lib/RemoteDebug/src/`.
   - Add examples in `lib/RemoteDebug/examples/`.

3. **Test the Library**:
   - Write test code in `src/main.cpp`.
   - Use the `platformio.ini` file to configure your PlatformIO environment:
     ```ini
     [env:esp32]
     platform = espressif32
     board = esp32dev
     framework = arduino
     ```
   - Build and upload the project to your device to ensure the library works.

---

## 3. Automating the `release` Branch

The `release` branch contains only the library files and is structured so others can use it via PlatformIO's `lib_deps`.

### 3.1 GitHub Action to Automate the `release` Branch

Create the following GitHub Action file in `.github/workflows/release-branch.yml`:

```yaml
name: Update Release Branch

on:
  push:
    branches:
      - main

jobs:
  update-release:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout Code
        uses: actions/checkout@v3

      - name: Prepare Release Branch
        run: |
          git config user.name "GitHub Actions"
          git config user.email "actions@github.com"

          # Create or switch to release branch
          git checkout -B release

          # Remove everything except the library folder
          find . -mindepth 1 ! -regex '^./lib/RemoteDebug\(.*\)?' ! -name '.git' ! -name '.gitignore' -exec rm -rf {} +

          # Move library contents to the root
          mv lib/RemoteDebug/* .
          rm -rf lib

          # Commit changes
          git add .
          git commit -m "Update release branch" || echo "No changes to commit"
          git push origin release --force
```

#### How It Works
- **Triggers on Push**: The action runs whenever you push to the `main` branch.
- **Prepares `release` Branch**: It creates or updates the `release` branch with only the library content at the root.

### 3.2 Fallback Bash Script

Use the following bash script to manually update the `release` branch if needed:

```bash
#!/bin/bash

# Ensure you're on the main branch and it's up-to-date
git checkout main
git pull origin main

# Switch to release branch (create if it doesn't exist)
if git show-ref --quiet refs/heads/release; then
  git checkout release
else
  git checkout -b release
fi

# Reset the branch to match main
git reset --hard main

# Remove everything except the library folder
find . -mindepth 1 ! -regex '^./lib/RemoteDebug\(.*\)?' ! -name '.git' ! -name '.gitignore' -exec rm -rf {} +

# Move library contents to the root
mv lib/RemoteDebug/* .
rm -rf lib

# Commit and push changes
git add .
git commit -m "Update release branch" || echo "No changes to commit"
git push origin release --force
```

Run this script locally whenever you need to update the `release` branch.

---

## 4. Sharing the Library

### PlatformIO Users
To use your library, users can add the following to their `platformio.ini`:
```ini
lib_deps = https://github.com/mdelgert/RemoteDebug.git#release
```

### Arduino IDE Users
- Download the repository and copy the `lib/RemoteDebug` folder to their Arduino `libraries/` directory.

---

## 5. Testing the Workflow

1. Develop your library in `lib/RemoteDebug`.
2. Push changes to the `main` branch.
3. Verify that the GitHub Action updates the `release` branch.
4. Test installing the library via `lib_deps` in another PlatformIO project:
   ```ini
   lib_deps = https://github.com/mdelgert/RemoteDebug.git#release
   ```

---

## 6. Best Practices

1. **Use Semantic Versioning**:
   - Update the `version` field in `library.json` for each release.

2. **Document Everything**:
   - Include clear instructions in `README.md` for installing and using the library.

3. **Automate as Much as Possible**:
   - Use the GitHub Action for consistent updates to the `release` branch.

4. **Test Thoroughly**:
   - Regularly test the library in the `main` branch using your test project.

---

By following this guide, you can develop, test, and distribute your PlatformIO library efficiently, ensuring a smooth experience for both you and your users.

