#ifndef WORKOUT_DEVICE_H
#define WORKOUT_DEVICE_H

#include <Arduino.h>
#include "BLEManager.h"
#include "LEDController.h"

class WorkoutDevice {
public:
    WorkoutDevice();
    void begin();
    void loop();
    void updateBatteryLevel();
    
    // Getters
    uint8_t getDeviceId() const { return deviceId; }
    String getDeviceName() const { return deviceName; }
    uint8_t getBatteryLevel() const { return batteryLevel; }
    
private:
    void determineDeviceId();
    
    uint8_t deviceId;
    String deviceName;
    uint8_t batteryLevel;
    unsigned long lastBatteryUpdate;
    
    BLEManager bleManager;
    LEDController ledController;
};

#endif // WORKOUT_DEVICE_H
