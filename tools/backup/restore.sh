#!/usr/bin/env bash
#
# restore.sh
#
# Restores files to the device from a local backup folder.
# - Adds Content-Type: text/plain (you can change if needed).
# - Uses verbose curl (-v) to see if there's an error from the device.
#

BASE_URL="http://demo1.local"
BACKUP_TGZ="device_backup.tar.gz"
BACKUP_DIR="device_backup"

#############
# 1) Extract backup tar if needed
#############
if [ -f "$BACKUP_TGZ" ]; then
  echo "Extracting backup from $BACKUP_TGZ..."
  tar -xzf "$BACKUP_TGZ" || {
    echo "Error extracting $BACKUP_TGZ"
    exit 1
  }
fi

# Now $BACKUP_DIR should exist (if that’s how it was created).
if [ ! -d "$BACKUP_DIR" ]; then
  echo "Error: backup directory '$BACKUP_DIR' not found."
  exit 1
fi

#############
# 2) Iterate over files in the backup folder
#############
cd "$BACKUP_DIR" || {
  echo "Could not cd into $BACKUP_DIR"
  exit 1
}

ALLFILES=$(find . -type f)
if [ -z "$ALLFILES" ]; then
  echo "No files found in $BACKUP_DIR to restore."
  exit 0
fi

echo "Restoring files from: $BACKUP_DIR"

for LOCALFILE in $ALLFILES; do
  # Re-add leading slash for the device path
  DEVICEPATH="/${LOCALFILE#./}"

  echo "Uploading: $DEVICEPATH"

  # Encode path
  ENCODED=$(python3 -c "import urllib.parse; print(urllib.parse.quote('$DEVICEPATH'))")

  # Use verbose curl (-v) so we see the request & response
  # and set a Content-Type header
  RESPONSE=$(curl -v \
    -X POST \
    -H "Content-Type: text/plain" \
    --data-binary @"$LOCALFILE" \
    "$BASE_URL/file?filename=$ENCODED" 2>&1)

  # If you need a different content type for HTML or JSON files, you can guess by extension.
  # Or simply use 'application/octet-stream' for all files.

  echo "$RESPONSE"

  # Optional: check for HTTP status or known error in $RESPONSE
  # e.g. if you see 500 or 404, handle it:
  if echo "$RESPONSE" | grep -q "HTTP/1.1 50"; then
    echo "Server returned a 50x error on $DEVICEPATH"
    # break or continue
  fi
done

echo "Restore complete."
