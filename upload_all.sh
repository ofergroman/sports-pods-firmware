#!/bin/bash

# Script to upload firmware to all connected ESP32 devices
# Usage: ./upload_all.sh

echo "Building firmware..."
platformio run --environment workout_device

if [ $? -ne 0 ]; then
    echo "❌ Build failed! Exiting."
    exit 1
fi

echo ""
echo "Scanning for connected ESP32 devices..."

# Find potential ESP32 devices on macOS
DEVICES=()
for device in /dev/cu.usbserial-* /dev/cu.usbmodem* /dev/cu.wchusbserial* /dev/cu.SLAB_USBtoUART*; do
    if [ -e "$device" ]; then
        DEVICES+=("$device")
    fi
done

if [ ${#DEVICES[@]} -eq 0 ]; then
    echo "❌ No ESP32 devices found!"
    echo "Make sure your devices are connected and drivers are installed."
    exit 1
fi

echo "Found ${#DEVICES[@]} potential device(s):"
for device in "${DEVICES[@]}"; do
    echo "  - $device"
done

echo ""
echo "Starting upload to all devices..."

SUCCESS_COUNT=0
TOTAL_COUNT=${#DEVICES[@]}

for device in "${DEVICES[@]}"; do
    echo ""
    echo "📤 Uploading to $device..."
    
    platformio run --environment workout_device --target upload --upload-port "$device"
    
    if [ $? -eq 0 ]; then
        echo "✅ Successfully uploaded to $device"
        ((SUCCESS_COUNT++))
    else
        echo "❌ Failed to upload to $device"
    fi
    
    # Small delay between uploads
    sleep 2
done

echo ""
echo "================================================"
echo "Upload Summary: $SUCCESS_COUNT/$TOTAL_COUNT devices successful"
echo "================================================"

if [ $SUCCESS_COUNT -eq $TOTAL_COUNT ]; then
    echo "🎉 All uploads completed successfully!"
    exit 0
else
    echo "⚠️  Some uploads failed. Check the output above for details."
    exit 1
fi
