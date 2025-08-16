# Claude Assistant Guide for ESP32 Workout Pod System

## Project Structure Overview

### Main ESP32 Project (PlatformIO)
**Location**: `/Users/ofergroman/Documents/PlatformIO/Projects/whatever/`
- **Platform**: ESP32-C6 (Seeed XIAO ESP32C6)
- **Build Command**: `~/.platformio/penv/bin/platformio run`
- **Upload Command**: `~/.platformio/penv/bin/platformio upload`
- **Monitor Command**: `~/.platformio/penv/bin/platformio device monitor`

### React Native App (Expo)
**Location**: `/Users/ofergroman/Documents/Projects/whatever/expo-ble-sample/`
- **Platform**: Expo + React Native with BLE
- **Start Command**: `npm start` or `npx expo start`
- **iOS Build**: `npx expo run:ios`
- **Android Build**: `npx expo run:android`

## Key Files to Modify

### ESP32 Arduino Code:
- `src/main.cpp` - Main application logic
- `src/BatteryMonitor.cpp/.h` - Battery monitoring functions
- `src/BluetoothManager.cpp/.h` - BLE communication
- `src/BLECallbacks.cpp/.h` - BLE event handling
- `src/SleepManager.cpp/.h` - Power management
- `src/OTAManager.cpp/.h` - Over-the-air updates
- `src/ESPNowCallbacks.cpp/.h` - ESP-NOW mesh communication
- `src/LEDControl.cpp/.h` - NeoPixel LED control
- `src/Constants.h` - Configuration and pin definitions
- `platformio.ini` - Build configuration

### React Native App:
- `App.tsx` - Main UI component
- `useBLE.ts` - BLE communication hook
- `ConfigurationModal.tsx` - Settings interface
- `DeviceConnectionModal.tsx` - Device connection UI
- `package.json` - Dependencies and scripts

## Hardware Configuration

### ESP32-C6 Pin Mapping (Seeed XIAO):
- **Battery Pin**: A0 (analog voltage divider)
- **Piezo Sensor**: A1 (tap detection)
- **Role Detection**: D2 (GND=Hub, Float=Pod)
- **NeoPixel Data**: D9 (16 LEDs)
- **I2C SDA**: D4 (optional sensors)
- **I2C SCL**: D5 (optional sensors)

### Device Roles:
- **Hub**: D2 connected to GND, runs BLE server
- **Pod**: D2 floating, ESP-NOW client only

## Development Workflow

### ESP32 Development:
1. `cd /Users/ofergroman/Documents/PlatformIO/Projects/whatever`
2. Make code changes
3. `~/.platformio/penv/bin/platformio run` - Compile
4. `~/.platformio/penv/bin/platformio upload` - Flash device
5. `~/.platformio/penv/bin/platformio device monitor` - Debug serial

### React Native Development:
1. `cd /Users/ofergroman/Documents/Projects/whatever/expo-ble-sample`
2. Make code changes
3. `npx expo start` - Start development server
4. Use Expo Go app or `npx expo run:ios` for testing

## Current Project Status

### Phase 1 ✅ - Battery Monitoring (COMPLETED)
- Real battery voltage reading and percentage conversion
- BLE battery status communication
- Battery level display in React Native app
- Low battery warnings and automatic updates

### Phase 2 🔄 - Sleep Mode Optimization (NEXT)
- Intelligent power management
- Wake-up coordination between devices
- Battery-based sleep decisions

### Phase 3 📋 - OTA Updates (PLANNED)
- Wireless firmware updates
- Update progress monitoring
- Rollback mechanisms

## Troubleshooting

### Common Issues:
- **PlatformIO not found**: Use full path `~/.platformio/penv/bin/platformio`
- **Device not detected**: Check USB connection and drivers
- **BLE connection issues**: Verify permissions and proximity
- **Build errors**: Check library dependencies in `platformio.ini`

### Debug Commands:
```bash
# List connected devices
~/.platformio/penv/bin/platformio device list

# Clean build
~/.platformio/penv/bin/platformio run --target clean

# Verbose build
~/.platformio/penv/bin/platformio run -v

# React Native debug
npx expo start --clear
```

## Quick Reference Commands

```bash
# ESP32 Build & Upload
cd /Users/ofergroman/Documents/PlatformIO/Projects/whatever
~/.platformio/penv/bin/platformio run
~/.platformio/penv/bin/platformio upload

# React Native Start
cd /Users/ofergroman/Documents/Projects/whatever/expo-ble-sample
npx expo start

# iOS Build
npx expo run:ios
```

This guide should help maintain context and speed up development across sessions.
