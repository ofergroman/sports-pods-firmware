# Multi-Device Upload Setup

This project now supports building and uploading firmware to all connected ESP32 devices simultaneously.

## Usage Options

### Option 1: Using PlatformIO Custom Target (Recommended)

Build and upload to all connected devices:
```bash
platformio run --environment all_devices --target upload_all
```

Or using the shorter form:
```bash
pio run -e all_devices -t upload_all
```

### Option 2: Using Shell Script

Run the shell script directly:
```bash
./upload_all.sh
```

## How It Works

1. **Device Detection**: The script automatically scans for connected ESP32 devices by looking for common USB-to-serial device patterns on macOS:
   - `/dev/cu.usbserial-*` (Generic USB-to-serial adapters)
   - `/dev/cu.usbmodem*` (USB CDC devices)
   - `/dev/cu.wchusbserial*` (WCH USB-to-serial chips)
   - `/dev/cu.SLAB_USBtoUART*` (Silicon Labs CP210x chips)

2. **Sequential Upload**: The firmware is uploaded to each detected device sequentially to avoid conflicts.

3. **Status Reporting**: The script provides clear feedback on the success/failure of each upload.

## Environments

- `workout_device`: Single device upload (original environment)
- `all_devices`: Multi-device upload with custom script

## Prerequisites

- All target devices must be connected via USB
- Proper USB-to-serial drivers must be installed
- Devices should be in programming mode (if required by your hardware)

## Troubleshooting

If uploads fail:
1. Check that devices are properly connected
2. Verify USB drivers are installed
3. Ensure devices are not being used by other applications (like serial monitors)
4. Try uploading to individual devices first to verify they work

## Files Added

- `scripts/upload_all.py`: PlatformIO custom script for multi-device upload
- `upload_all.sh`: Standalone shell script for multi-device upload
- Updated `platformio.ini` with `all_devices` environment
