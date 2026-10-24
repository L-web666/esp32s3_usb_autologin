#!/bin/bash

set -e  # Exit on any error
set -o pipefail  # Catch pipeline errors

# Variables
TOOLCHAIN_NAME="toolchain-xtensa-esp32s3"
SOURCE_DIR="$HOME/source/esp"
ESP_IDF_REPO="https://github.com/espressif/esp-idf.git"
ESP_IDF_BRANCH="v5.2.2"
ESP_IDF_DIR="$SOURCE_DIR/esp-idf"
ESPRESSIF_TOOLS_DIR="$HOME/.espressif/tools/xtensa-esp-elf-gdb/14.2_20240403/xtensa-esp-elf-gdb"
PLATFORMIO_PATH="$HOME/.platformio/packages/$TOOLCHAIN_NAME"
PLATFORMIO_BIN_DIR="$PLATFORMIO_PATH/bin"
PLATFORMIO_LIB_DIR="$PLATFORMIO_PATH/lib"

# Functions
verify_toolchain_path() {
    if [ ! -d "$PLATFORMIO_PATH" ]; then
        echo "Error: Toolchain path $PLATFORMIO_PATH does not exist. Please ensure PlatformIO and the toolchain are installed."
        exit 1
    fi
}

setup_esp_idf() {
    echo "Setting up ESP-IDF v5.2.2..."
    if [ ! -d "$ESP_IDF_DIR" ]; then
        mkdir -p "$SOURCE_DIR"
        git clone -b "$ESP_IDF_BRANCH" --recursive "$ESP_IDF_REPO" "$ESP_IDF_DIR"
    else
        echo "ESP-IDF repository already cloned at $ESP_IDF_DIR."
    fi

    echo "Installing ESP-IDF tools for ESP32-S3..."
    cd "$ESP_IDF_DIR"
    ./install.sh esp32s3
}

backup_and_copy_files() {
    echo "Backing up existing GDB binary..."
    if [ -f "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb" ]; then
        mv "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb" "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb.orig"
    fi

    echo "Copying new debugger files..."
    cp "$ESPRESSIF_TOOLS_DIR/bin/xtensa-esp32s3-elf-gdb" "$PLATFORMIO_BIN_DIR/"
    cp "$ESPRESSIF_TOOLS_DIR/bin/xtensa-esp-elf-gdb-no-python" "$PLATFORMIO_BIN_DIR/"
    cp "$ESPRESSIF_TOOLS_DIR/lib/xtensa_esp32s3.so" "$PLATFORMIO_LIB_DIR/"
    echo "Debugger files copied successfully."
}

restore_original() {
    echo "Restoring original GDB binaries..."
    if [ -f "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb.orig" ]; then
        mv "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb.orig" "$PLATFORMIO_BIN_DIR/xtensa-esp32s3-elf-gdb"
    fi
    rm -f "$PLATFORMIO_BIN_DIR/xtensa-esp-elf-gdb-no-python"
    rm -f "$PLATFORMIO_LIB_DIR/xtensa_esp32s3.so"
    echo "Original debugger setup restored."
}

complete_build() {
    echo "Performing a complete build..."
    setup_esp_idf
    backup_and_copy_files
    echo "Complete build and update completed successfully."
}

patch_toolchain() {
    echo "Patching the PlatformIO toolchain with prebuilt binaries..."
    backup_and_copy_files
    echo "Toolchain patched successfully."
}

# Main Script
echo "Starting ESP32 Debugger Setup..."

# Verify the toolchain path
verify_toolchain_path

# User options
echo "Select an option:"
echo "1) Complete build (build ESP-IDF and copy new files)"
echo "2) Patch existing toolchain with prebuilt files"
echo "3) Restore original toolchain files"
read -rp "Enter your choice (1/2/3): " user_choice

case $user_choice in
    1)
        complete_build
        ;;
    2)
        patch_toolchain
        ;;
    3)
        restore_original
        ;;
    *)
        echo "Invalid option. Exiting."
        exit 1
        ;;
esac

# Completion message
echo "ESP32 Debugger Setup completed."
echo "You can now use PlatformIO with the updated toolchain."
