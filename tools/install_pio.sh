#!/bin/bash

set -e  # Exit on any error
set -o pipefail  # Catch pipeline errors

# Variables
PLATFORMIO_BIN="$HOME/.local/bin"
PLATFORMIO_INSTALLER="get-platformio.py"
PLATFORMIO_PATH="$HOME/.platformio/penv/bin"
UDEV_RULES_URL="https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules"
UDEV_RULES_FILE="/etc/udev/rules.d/99-platformio-udev.rules"

echo "Starting PlatformIO Developer Setup..."

# Check and install prerequisites
echo "Checking and installing required packages..."
REQUIRED_PACKAGES=(python3 python3-venv curl)
for pkg in "${REQUIRED_PACKAGES[@]}"; do
    if ! dpkg -l | grep -qw "$pkg"; then
        echo "Installing $pkg..."
        sudo apt install -y "$pkg"
    else
        echo "$pkg is already installed."
    fi
done

# Verify Python version
echo "Python version: $(python3 -V)"

# Download and install PlatformIO
echo "Downloading and installing PlatformIO..."
curl -fsSL -o "$PLATFORMIO_INSTALLER" https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
python3 "$PLATFORMIO_INSTALLER"
rm "$PLATFORMIO_INSTALLER"

# Create symbolic links
echo "Setting up PlatformIO binary links..."
mkdir -p "$PLATFORMIO_BIN"
for cmd in platformio pio piodebuggdb; do
    if [ ! -L "$PLATFORMIO_BIN/$cmd" ]; then
        ln -s "$PLATFORMIO_PATH/$cmd" "$PLATFORMIO_BIN/$cmd"
        echo "Linked $cmd."
    else
        echo "Link for $cmd already exists."
    fi
done

# Add PlatformIO to PATH
if ! grep -qxF "export PATH=\$PATH:$PLATFORMIO_BIN" ~/.bashrc; then
    echo "export PATH=\$PATH:$PLATFORMIO_BIN" >> ~/.bashrc
    echo "Added PlatformIO binaries to PATH."
else
    echo "PlatformIO binaries already in PATH."
fi

# Install udev rules
echo "Installing udev rules for PlatformIO..."
curl -fsSL "$UDEV_RULES_URL" | sudo tee "$UDEV_RULES_FILE"
sudo service udev restart

# Add user to necessary groups
echo "Adding $USER to required groups..."
for group in dialout plugdev; do
    if ! groups "$USER" | grep -qw "$group"; then
        sudo usermod -a -G "$group" "$USER"
        echo "Added $USER to $group."
    else
        echo "$USER is already a member of $group."
    fi
done

# Final messages
echo "PlatformIO Developer Setup completed successfully."
echo "Please run 'source ~/.bashrc' or restart your terminal to apply changes."
echo "Verify installation by running: pio --version"
