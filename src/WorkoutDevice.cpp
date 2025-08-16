#include "WorkoutDevice.h"
#include "Constants.h"
#include <WiFi.h>

// Role pin for device ID detection
#define ROLE_PIN 2  // D2 pin for device ID detection

WorkoutDevice::WorkoutDevice() 
    : deviceId(0), batteryLevel(85), lastBatteryUpdate(0) {
}

void WorkoutDevice::begin() {
    Serial.begin(115200);
    Serial.println("\n=== Workout Device Starting ===");
    
    // Determine device ID and name
    determineDeviceId();
    
    // Initialize LED controller
    ledController.begin(deviceId);
    
    // Initialize BLE
    bleManager.begin(deviceName, this, &ledController);
    
    Serial.println("=== Device Ready ===");
}

void WorkoutDevice::loop() {
    updateBatteryLevel();
    delay(100);
}

void WorkoutDevice::updateBatteryLevel() {
    // Simulate battery drain
    if (millis() - lastBatteryUpdate > 60000) { // Every minute
        if (batteryLevel > 0) {
            batteryLevel--;
        }
        lastBatteryUpdate = millis();
    }
}

void WorkoutDevice::determineDeviceId() {
    // Read role pin to determine device ID
    pinMode(ROLE_PIN, INPUT_PULLUP);
    delay(100); // Allow pin to stabilize
    
    bool rolePin = digitalRead(ROLE_PIN);
    
    // Get last byte of MAC address for additional uniqueness
    uint8_t mac[6];
    WiFi.macAddress(mac);
    uint8_t macLastByte = mac[5];
    
    if (!rolePin) {
        // Pin grounded - Device 1
        deviceId = 1;
        deviceName = "WorkoutDevice_1";
    } else {
        // Pin floating - Use MAC to determine ID (2, 3, 4...)
        deviceId = 2 + (macLastByte % 3); // Creates IDs 2, 3, or 4
        deviceName = "WorkoutDevice_" + String(deviceId);
    }
    
    Serial.printf("Device ID: %d, Name: %s\n", deviceId, deviceName.c_str());
    Serial.printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", 
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
