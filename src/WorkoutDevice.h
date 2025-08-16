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
    WORKOUT_COMPLETE
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
    
    // Getters
    uint8_t getDeviceId() const { return deviceId; }
    String getDeviceName() const { return deviceName; }
    uint8_t getBatteryLevel() const { return batteryLevel; }
    WorkoutState getWorkoutState() const { return workoutState; }
    unsigned long getWorkoutTime() const { return workoutTime; }
    uint16_t getCaloriesBurned() const { return caloriesBurned; }
    
private:
    void determineDeviceId();
    
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
    
    BLEManager bleManager;
    LEDController ledController;
};

#endif // WORKOUT_DEVICE_H
