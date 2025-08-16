# Workout Device Controller - ESP32 & React Native BLE Project

## Project Overview

This is a multi-phase project that combines ESP32 microcontrollers with a React Native mobile application to create a wireless workout device control system. Each ESP32 device controls a NeoPixel LED strip and communicates via Bluetooth Low Energy (BLE).

## Phase 1: Basic LED Control System ✅ COMPLETED

### Achievements
- **Multiple Device Support**: Successfully implemented a system that can discover, connect to, and control multiple ESP32 workout devices simultaneously
- **BLE Communication**: Established reliable Bluetooth Low Energy communication between React Native app and ESP32 devices
- **LED Pattern Control**: Implemented various LED patterns including solid colors, flash, chase, rainbow, and off states
- **Device Management**: Created a comprehensive device management system with connection status, battery monitoring, and individual/group controls
- **Automatic Device Discovery**: Devices auto-advertise with unique names and can be automatically discovered by the mobile app

### Hardware Setup
- **Board**: Seeed XIAO ESP32-C6
- **LED Strip**: 16 NeoPixel RGB LEDs (WS2812B compatible)
- **Device ID Detection**: Uses D2 pin (pull-up/ground) for primary device identification
- **Unique Naming**: Combines pin state with MAC address for unique device IDs (WorkoutDevice_1, WorkoutDevice_2, etc.)

### Software Architecture

#### ESP32 Firmware Features
- **BLE Server Implementation**: Each device acts as a BLE peripheral with custom service and characteristics
- **JSON Command Processing**: Commands sent from mobile app are parsed as JSON for flexibility
- **LED Pattern Engine**: Supports multiple patterns (solid, flash, chase, rainbow, off)
- **Device Status Reporting**: Real-time battery level, connection status, and device info
- **Auto-reconnection**: Automatically restarts advertising when disconnected

#### React Native Mobile App Features
- **Multi-Device Management**: Scan, connect, and manage multiple devices simultaneously
- **Real-time Status**: Shows connection status, battery levels, and device information
- **Individual Control**: Control each device's LEDs independently
- **Group Control**: Send commands to all connected devices at once
- **User-Friendly Interface**: Clean, intuitive UI with color-coded buttons and status indicators

### Technical Specifications

#### BLE Protocol
- **Service UUID**: `12345678-1234-5678-9abc-123456789abc`
- **Command Characteristic**: `87654321-4321-8765-cba9-876543210abc` (Write)
- **Status Characteristic**: `11111111-2222-3333-4444-555555555555` (Read/Notify)

#### Command Structure (JSON)
```json
{
  "type": "led_pattern",
  "pattern": "solid|flash|chase|rainbow|off", 
  "color": 0xFF0000,
  "duration": 1000
}
```

#### Supported LED Patterns
- **Solid**: All LEDs show the same color
- **Flash**: 3 quick flashes of the specified color
- **Chase**: LED chase effect moving across the strip
- **Rainbow**: Cycling rainbow pattern across all LEDs
- **Off**: Turn off all LEDs

### Hardware Configuration
- **LED Pin**: D9 (NEOPIXEL_PIN)
- **Device ID Pin**: D2 (ROLE_PIN)
- **LED Count**: 16 pixels
- **LED Brightness**: 50 (0-255 scale)

### File Structure
```
ESP32 Project:
├── src/
│   ├── WorkoutDevice.cpp    # Main device firmware
│   ├── WorkoutDevice.h      # Device class header
│   ├── LEDController.cpp    # LED pattern controller
│   ├── LEDController.h      # LED controller header
│   ├── BLEManager.cpp       # BLE communication manager
│   ├── BLEManager.h         # BLE manager header
│   └── Constants.h          # Hardware pin definitions
├── platformio.ini           # PlatformIO configuration
└── scripts/
    └── upload_all.py        # Multi-device upload script

React Native App:
├── App.tsx                  # Main application logic
├── package.json            # Dependencies
└── expo config files
```

### Dependencies

#### ESP32 Libraries
- Adafruit NeoPixel (^1.12.5)
- ArduinoJson (^7.2.1)
- ESP32 BLE Arduino (built-in)

#### React Native Libraries
- react-native-ble-plx (^3.5.0)
- react-native-base64 (^0.2.1)
- Expo framework (~52.0.47)

### Development Tools
- **PlatformIO**: ESP32 development and multi-device programming
- **Expo**: React Native development and testing
- **VS Code**: Primary development environment

### Tested Functionality ✅
- [x] Device discovery and connection
- [x] Multiple simultaneous device connections
- [x] All LED patterns working correctly
- [x] Individual device control
- [x] Group device control
- [x] Real-time status updates
- [x] Battery level monitoring
- [x] Automatic reconnection handling
- [x] Device disconnection and cleanup

### Known Limitations
- Battery level is currently simulated (decrements over time)
- No persistent device pairing (must scan/connect each session)
- Limited to devices advertising the specific service UUID

## Next Phases (Planned)

### Phase 2: Enhanced Sensor Integration
- Add accelerometer/gyroscope for motion detection
- Implement workout movement tracking
- Enhanced LED patterns based on movement

### Phase 3: Workout Programs
- Pre-defined workout routines
- Progress tracking and feedback
- Integration with fitness APIs

---

## Development Setup

### ESP32 Setup
1. Install PlatformIO in VS Code
2. Open the ESP32 project folder
3. Build and upload to devices using `upload_all.sh`

### React Native Setup
1. Install Node.js and npm
2. Install Expo CLI: `npm install -g expo-cli`
3. Navigate to `expo-ble-sample` folder
4. Run `npm install`
5. Start development: `expo start`

### Quick Start

### 1. Flash Workout Devices:
```bash
```bash
cd /path/to/platformio/project
pio run -e workout_device -t upload
# OR use upload script for multiple devices
./upload_all.sh
```

### 2. Launch React Native App:
```bash
cd expo-ble-sample
npm install
expo start
```

### 3. Test the System:
1. Power on ESP32 devices (they will auto-advertise)
2. Open the React Native app on your mobile device
3. Tap "Scan for Devices" to discover WorkoutDevice_X devices
4. Connect to devices and test LED controls

### Testing
1. Upload firmware to one or more ESP32 devices
2. Launch React Native app on mobile device
3. Scan for devices and connect
4. Test LED control functionality

---

**Phase 1 Status**: ✅ **COMPLETE** - Ready for Phase 2 development
```

### 3. Run React Native App:
```bash
cd /path/to/expo-project
npm start
```

### 4. Monitor Device Output (optional):
```bash
./build.sh monitor-hub      # Monitor hub device
./build.sh monitor-child    # Monitor child device
```

## Testing

1. Power on hub device (should show blue startup sequence)
2. Power on child devices (should show red startup sequence) 
3. Open React Native app and tap "Connect to Hub"
4. Child devices should appear in the app automatically
5. Test LED controls (Red/Green/Off) for individual and global control

## LED Indicators

### Hub Device:
- **Blue blinks (3x)**: Startup/BLE advertising
- **Green blinks (2x)**: BLE client connected
- **Red blink (1x)**: BLE client disconnected

### Child Devices:
- **Red blinks (3x)**: Startup
- **Blue blinks (2x)**: Discovery response sent
- **Green (1s)**: Ready state

## Communication Protocol

### BLE Commands (App → Hub):
- `GET_DEVICES`: Request device list
- `HUB_LED:color`: Control hub LED (RED/GREEN/OFF)
- `CHILD_LED:id:color`: Control specific child LED
- `ALL_LED:color`: Control all device LEDs

### ESP-NOW Messages (Hub ↔ Children):
- Discovery request/response
- LED commands
- Heartbeat/status updates

## Next Steps (Future Phases)

- Battery monitoring
- Tap detection
- Workout sequences  
- Performance optimizations
- Power management
