#ifndef WORKOUT_DEVICE_H
#define WORKOUT_DEVICE_H

#include <Arduino.h>
#include "BLEManager.h"
#include "LEDController.h"

enum WorkoutState {
    WORKOUT_IDLE,
    WORKOUT_WARMUP,
    WORKOUT_ACTIVE,
    WORKOUT_REST,
    WORKOUT_COOLDOWN,
    WORKOUT_COMPLETE,
    // Reactive training states
    REACTIVE_WAITING,    // Waiting to flash
    REACTIVE_ACTIVE,     // Flashing - waiting for tap
    REACTIVE_SUCCESS,    // Tapped in time
    REACTIVE_MISSED      // Missed the tap
};

class WorkoutDevice {
public:
    WorkoutDevice();
    void begin();
    void loop();
    void updateBatteryLevel();
    
    // Workout management
    void startWorkout();
    void stopWorkout();
    void setWorkoutState(WorkoutState state);
    void updateWorkoutTimer();
    
    // Reactive training
    void startReactiveTraining();
    void stopReactiveTraining();
    void activateForTap(int timeoutMs = 0);  // Flash this device for user to tap (0 = use default)
    void deactivateForTap();    // Turn off flash
    bool checkTapDetected();    // Check if piezo detected a tap
    void handleTapSuccess();    // Handle successful tap
    void handleTapMissed();     // Handle missed tap
    
    // Debug mode
    void enableDebugMode();     // Enable debug mode for piezo testing
    void disableDebugMode();    // Disable debug mode
    
    // Power management
    void enableLowPowerMode();  // Reduce LED brightness and update frequency
    void disableLowPowerMode(); // Return to normal power mode
    void enterSleepMode();      // Enter deep sleep to save battery
    bool checkBatteryLevel();   // Check battery and warn if low
    float getBatteryVoltage();  // Get current battery voltage
    
    // Getters
    uint8_t getDeviceId() const { return deviceId; }
    String getDeviceName() const { return deviceName; }
    uint8_t getBatteryLevel() const { return batteryLevel; }
    WorkoutState getWorkoutState() const { return workoutState; }
    unsigned long getWorkoutTime() const { return workoutTime; }
    uint16_t getCaloriesBurned() const { return caloriesBurned; }
    uint16_t getReactionTime() const { return lastReactionTime; }
    uint16_t getSuccessCount() const { return successCount; }
    uint16_t getMissedCount() const { return missedCount; }
    
private:
    void determineDeviceId();
    void updatePiezoReading();
    
    uint8_t deviceId;
    String deviceName;
    uint8_t batteryLevel;
    unsigned long lastBatteryUpdate;
    
    // Workout tracking
    WorkoutState workoutState;
    unsigned long workoutStartTime;
    unsigned long workoutTime;
    unsigned long lastStateChange;
    uint16_t caloriesBurned;
    uint16_t repCount;
    
    // Reactive training
    unsigned long tapActivationTime;    // When the device started flashing
    unsigned long lastTapTime;          // Last time a tap was detected
    uint16_t lastReactionTime;          // Last reaction time in milliseconds
    uint16_t successCount;              // Number of successful taps
    uint16_t missedCount;               // Number of missed taps
    bool isActiveForTap;                // Whether this device is currently active for tapping
    int currentTapTimeout;              // Current tap timeout in ms (configurable per activation)
    int lastPiezoReading;               // Last piezo sensor reading
    bool debugMode;                     // Debug mode for piezo testing
    
    // Power management
    bool lowPowerMode;                  // Whether device is in low power mode
    unsigned long lastActivityTime;     // Last time device was active
    unsigned long lastBatteryCheck;     // Last time battery was checked
    float lastBatteryVoltage;           // Last measured battery voltage
    
    BLEManager bleManager;
    LEDController ledController;
};

#endif // WORKOUT_DEVICE_H
