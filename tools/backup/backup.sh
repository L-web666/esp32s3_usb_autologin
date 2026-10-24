#!/usr/bin/env bash
#
# backup_device.sh
#
# A script that backs up all files from an ESP-like device (or similar)
# using the `/files` and `/file?filename=...` endpoints.
#
# Prerequisites:
#   - curl (for HTTP requests)
#   - jq   (for JSON parsing)
#   - python3 (for URL-encoding file paths)
#   - tar  (to pack the backup)
#
# If `jq` is missing on Debian/Ubuntu or macOS, the script will attempt to install it.
#

#######################
# Configuration
#######################
BASE_URL="http://demo1.local"
#BACKUP_DIR="device_backup_$(date +%Y%m%d_%H%M%S)"
BACKUP_DIR="device_backup"

#######################
# Functions
#######################

check_command() {
  local cmd="$1"
  command -v "$cmd" &>/dev/null
}

install_jq_linux() {
  echo "Attempting to install jq on Linux (Debian/Ubuntu). This may require sudo..."
  sudo apt-get update || {
    echo "Failed to run apt-get update. Please install jq manually."
    return 1
  }
  sudo apt-get install -y jq || {
    echo "Failed to install jq. Please install it manually."
    return 1
  }
}

install_jq_macos() {
  echo "Attempting to install jq on macOS via Homebrew..."
  if ! check_command brew; then
    echo "Homebrew not found. Please install Homebrew or jq manually."
    return 1
  fi
  brew install jq || {
    echo "Failed to install jq via Homebrew."
    return 1
  }
}


#######################
# Check prerequisites
#######################
echo "Checking prerequisites..."

# curl
if ! check_command curl; then
  echo "Error: curl not found on PATH. Please install curl."
  exit 1
fi

# jq
if ! check_command jq; then
  echo "jq not found. Attempting installation..."
  case "$(uname)" in
    Linux)
      install_jq_linux
      ;;
    Darwin)
      install_jq_macos
      ;;
    *)
      echo "Unsupported OS for automatic jq install. Please install jq manually."
      exit 1
      ;;
  esac

  # Check again
  if ! check_command jq; then
    echo "jq still not found. Exiting."
    exit 1
  fi
fi

# python3
if ! check_command python3; then
  echo "Error: python3 not found on PATH. Please install python3."
  exit 1
fi

# tar
if ! check_command tar; then
  echo "Error: tar not found on PATH. Please install tar."
  exit 1
fi

#######################
# Main Backup Process
#######################
echo "All prerequisites found (or installed). Proceeding with backup..."

# 1) Get the file list from device
echo "Fetching file list from: $BASE_URL/files"
curl -s "$BASE_URL/files" > /tmp/filelist.json

# 2) Parse out the array of files using jq
FILES=($(jq -r '.data.files[]' /tmp/filelist.json 2>/dev/null))

# Validate JSON parse
if [ $? -ne 0 ] || [ ${#FILES[@]} -eq 0 ]; then
  echo "Error: Failed to parse file list or no files found in JSON."
  cat /tmp/filelist.json
  exit 1
fi

echo "Found ${#FILES[@]} files: ${FILES[@]}"

# 3) Create a local backup directory
mkdir -p "$BACKUP_DIR"
echo "Created backup directory: $BACKUP_DIR"

# 4) Download each file
for FILEPATH in "${FILES[@]}"; do
  # Remove leading slash for local path
  LOCALNAME="${FILEPATH#/}"

  # URL-encode for query param
  ENCODED=$(python3 -c "import urllib.parse; print(urllib.parse.quote('$FILEPATH'))")

  # Ensure local subdirectory exists if the file has subfolders
  mkdir -p "$BACKUP_DIR/$(dirname "$LOCALNAME")"

  # Download
  echo "Downloading: $FILEPATH"
  curl -s -o "$BACKUP_DIR/$LOCALNAME" "$BASE_URL/file?filename=$ENCODED"

  # Optional: Check HTTP response codes or file size
done

# 5) Tar + gzip the directory
BACKUP_TGZ="${BACKUP_DIR}.tar.gz"
tar -czf "$BACKUP_TGZ" "$BACKUP_DIR"

echo "Backup finished. Archive created: $BACKUP_TGZ"
exit 0
