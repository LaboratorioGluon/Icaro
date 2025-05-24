#!/bin/bash
set -e

# ------------- CONFIGURATION -------------
# ⚠️ UNCOMMENT AND CHANGE THIS TO YOUR SD DEVICE! (e.g: /dev/sdb) ⚠️⚠️⚠️ It will be formatted ⚠️⚠️⚠️
# SD_DEVICE=""          
DEVICE_NAME="ICAROCAM"        # FAT32 volume label (max 11 chars)
MOUNT_DIR="/media/${USER}/${DEVICE_NAME}"

# ------------- ROOT CHECK -------------
if [ "$EUID" -ne 0 ]; then
    echo "❌ This script must be run as root (use sudo)"
    echo "⚠️ WARNING: This script will FORMAT $SD_DEVICE"
    exit 1
fi

# ------------- SAFETY CHECK -------------
echo "⚠️ WARNING: This script will FORMAT '$SD_DEVICE'"
echo "Press Ctrl+C to cancel or wait 5 seconds to continue..."
sleep 5

# ------------- FORMAT SD -------------
echo "🧽 Formatting $SD_DEVICE as FAT32 with label '$DEVICE_NAME'..."
mkfs.vfat -F 32 -n "$DEVICE_NAME" "$SD_DEVICE"
echo "✅ Format complete."

# ------------- MOUNT SD -------------
echo "📦 Mounting $SD_DEVICE to $MOUNT_DIR..."
sudo mkdir -p "$MOUNT_DIR"
sudo mount "$SD_DEVICE" "$MOUNT_DIR"

# ------------- CREATE DIRECTORIES -------------
echo "📁 Creating directories from 00000 to 09999..."
for i in $(seq -w 0 9999); do
    mkdir -p "$MOUNT_DIR/$i"
    if (( 10#$i % 1000 == 0 )); then
        echo "  - Created: $i"
    fi
done
echo "✅ All directories created."

# ------------- UNMOUNT SD -------------
echo "📤 Unmounting $SD_DEVICE..."
sudo umount "$SD_DEVICE"
echo "✅ Done."
