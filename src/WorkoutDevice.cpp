#include "WorkoutDevice.h"
#include "Constants.h"
#include <WiFi.h>
#include <esp_sleep.h>
#include <driver/gpio.h>
#include <soc/rtc.h>

// Role pin for device ID detection
#define ROLE_PIN 2  // D2 pin for device ID detection

WorkoutDevice::WorkoutDevice() 
    : deviceId(0), batteryLevel(85), lastBatteryUpdate(0),
      workoutState(WORKOUT_IDLE), workoutStartTime(0), workoutTime(0),
      lastStateChange(0), caloriesBurned(0), repCount(0),
      tapActivationTime(0), lastTapTime(0), lastReactionTime(0),
      successCount(0), missedCount(0), isActiveForTap(false), 
      currentTapTimeout(TAP_TIMEOUT_MS), lastPiezoReading(50),
      debugMode(false), lowPowerMode(false), lastActivityTime(0),
      lastBatteryCheck(0), lastBatteryVoltage(0.0) {
}

void WorkoutDevice::begin() {
    Serial.begin(115200);
    Serial.println("=== Workout Device Starting ===");
    
    // Check wake-up cause and provide visual feedback
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    
    // Initialize LED controller first for wake-up indication
    ledController.begin(deviceId);
    
    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("Device woke up from piezo tap!");
        // Flash blue LEDs to indicate wake-up from sleep
        for (int flash = 0; flash < 3; flash++) {
            ledController.setPattern(PATTERN_SOLID, COLOR_BLUE, 200);
            delay(200);
            ledController.clear();
            delay(200);
        }
        Serial.println("Wake-up indication complete - device ready");
    } else {
        Serial.println("Device powered on normally");
    }
    
    // Initialize piezo pin
    pinMode(PIEZO_PIN, INPUT);
    
    // Initialize piezo baseline reading
    delay(100); // Allow pin to stabilize
    lastPiezoReading = analogRead(PIEZO_PIN);
    Serial.printf("Initial piezo baseline: %d\n", lastPiezoReading);
    
    // Determine device ID and name
    determineDeviceId();
    
    // Initialize BLE
    bleManager.begin(deviceName, this, &ledController);
    
    Serial.println("=== Device Ready ===");
}

void WorkoutDevice::loop() {
    updateBatteryLevel();
    updateWorkoutTimer();
    updatePiezoReading();
    
    // Power management checks
    checkBatteryLevel();
    
    // Auto-sleep after inactivity
    if (millis() - lastActivityTime > SLEEP_TIMEOUT_MS) {
        Serial.println("Entering sleep mode due to inactivity...");
        enterSleepMode();
    }
    
    // Handle reactive training timeout
    if (workoutState == REACTIVE_ACTIVE && isActiveForTap) {
        if (millis() - tapActivationTime > currentTapTimeout) {
            Serial.printf("TIMEOUT! Device %d timed out after %d ms\n", deviceId, currentTapTimeout);
            handleTapMissed();
        }
    }
    
    // Handle state transitions back to waiting (non-blocking)
    if ((workoutState == REACTIVE_SUCCESS || workoutState == REACTIVE_MISSED) && 
        millis() - lastStateChange > 300) {
        workoutState = REACTIVE_WAITING;
        ledController.showWorkoutState(workoutState);
    }
    
    delay(10); // Shorter delay for better responsiveness
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

void WorkoutDevice::startWorkout() {
    workoutState = WORKOUT_WARMUP;
    workoutStartTime = millis();
    workoutTime = 0;
    caloriesBurned = 0;
    repCount = 0;
    lastStateChange = millis();
    
    Serial.println("Workout started - entering warmup phase");
    ledController.showWorkoutState(workoutState);
}

void WorkoutDevice::stopWorkout() {
    workoutState = WORKOUT_IDLE;
    workoutTime = 0;
    lastStateChange = millis();
    
    Serial.println("Workout stopped");
    ledController.clear();
}

void WorkoutDevice::setWorkoutState(WorkoutState state) {
    workoutState = state;
    lastStateChange = millis();
    
    Serial.printf("Workout state changed to: %d\n", state);
    ledController.showWorkoutState(state);
}

void WorkoutDevice::updateWorkoutTimer() {
    if (workoutState != WORKOUT_IDLE) {
        workoutTime = (millis() - workoutStartTime) / 1000; // Convert to seconds
        
        // Simulate calorie burn (very basic)
        if (workoutState == WORKOUT_ACTIVE) {
            if (millis() - lastStateChange > 5000) { // Every 5 seconds
                caloriesBurned += 1;
                lastStateChange = millis();
            }
        }
    }
}

// Reactive Training Methods
void WorkoutDevice::startReactiveTraining() {
    workoutState = REACTIVE_WAITING;
    workoutStartTime = millis();
    successCount = 0;
    missedCount = 0;
    isActiveForTap = false;
    Serial.println("Reactive training started - waiting mode");
    ledController.showWorkoutState(workoutState);
}

void WorkoutDevice::stopReactiveTraining() {
    workoutState = WORKOUT_IDLE;
    isActiveForTap = false;
    Serial.printf("Reactive training stopped - Success: %d, Missed: %d\n", successCount, missedCount);
    ledController.showWorkoutState(workoutState);
}

void WorkoutDevice::activateForTap(int timeoutMs) {
    if (workoutState == REACTIVE_WAITING) {
        workoutState = REACTIVE_ACTIVE;
        isActiveForTap = true;
        tapActivationTime = millis(); // Start timing from now
        lastActivityTime = millis(); // Update activity time for power management
        
        // Set the timeout - use provided value or default
        currentTapTimeout = (timeoutMs > 0) ? timeoutMs : TAP_TIMEOUT_MS;
        
        // Initialize lastPiezoReading to current baseline to avoid false spikes
        lastPiezoReading = analogRead(PIEZO_PIN);
        Serial.printf("Device %d activated for tap! Timeout: %d ms, Baseline piezo: %d\n", 
                     deviceId, currentTapTimeout, lastPiezoReading);
        
        ledController.showActiveTarget(); // Show bright white color
        
        // Read piezo again after LED activation and update baseline
        delay(10); // Small delay for LED to stabilize
        int postLedReading = analogRead(PIEZO_PIN);
        Serial.printf("Device %d post-LED piezo: %d\n", deviceId, postLedReading);
        
        // Use the post-LED reading as the new baseline to avoid false triggers
        lastPiezoReading = postLedReading;
    }
}

void WorkoutDevice::deactivateForTap() {
    if (isActiveForTap) {
        isActiveForTap = false;
        workoutState = REACTIVE_WAITING;
        Serial.printf("Device %d deactivated\n", deviceId);
        ledController.showWorkoutState(workoutState);
    }
}

bool WorkoutDevice::checkTapDetected() {
    if (!isActiveForTap) return false;
    
    // Longer grace period to avoid LED activation noise
    if (millis() - tapActivationTime < 300) return false;
    
    int piezoReading = analogRead(PIEZO_PIN);
    
    // Much more conservative thresholds for workout mode
    // Check for tap in two ways:
    // 1. Absolute threshold (higher than debug mode)
    // 2. Change from previous reading (spike detection)
    bool thresholdExceeded = piezoReading > 150; // Higher threshold for workout
    bool spikeDetected = (piezoReading - lastPiezoReading) > 50; // Require significant spike
    
    if ((thresholdExceeded || spikeDetected) && (millis() - lastTapTime) > DEBOUNCE_MS) {
        lastTapTime = millis();
        lastReactionTime = lastTapTime - tapActivationTime;
        Serial.printf("TAP DETECTED! Reading: %d, Previous: %d, Reaction: %d ms\n", 
                     piezoReading, lastPiezoReading, lastReactionTime);
        handleTapSuccess();
        return true;
    }
    
    // Update previous reading
    lastPiezoReading = piezoReading;
    return false;
}

void WorkoutDevice::handleTapSuccess() {
    if (isActiveForTap) {
        successCount++;
        workoutState = REACTIVE_SUCCESS;
        isActiveForTap = false;
        lastActivityTime = millis(); // Update activity time for power management
        Serial.printf("SUCCESS! Reaction time: %d ms, Total: %d\n", lastReactionTime, successCount);
        ledController.showSuccess(); // Show green flash
        
        // Send success notification via BLE
        bleManager.notifyTapResult(true, lastReactionTime);
        
        // Schedule return to waiting state (non-blocking)
        lastStateChange = millis();
    }
}

void WorkoutDevice::handleTapMissed() {
    if (isActiveForTap) {
        missedCount++;
        workoutState = REACTIVE_MISSED;
        isActiveForTap = false;
        lastActivityTime = millis(); // Update activity time for power management
        Serial.printf("MISSED! Timeout after %d ms, Total missed: %d\n", currentTapTimeout, missedCount);
        
        // Just turn off LEDs instead of showing red flash
        ledController.clear();
        
        // Send miss notification via BLE
        bleManager.notifyTapResult(false, currentTapTimeout);
        
        // Schedule return to waiting state (non-blocking)
        lastStateChange = millis();
    }
}

void WorkoutDevice::updatePiezoReading() {
    static unsigned long lastTapTime_static = 0;  // Local static to avoid conflicts
    static unsigned long lastDebugTime = 0;
    static unsigned long lastBaselineTime = 0;
    static int lastReading = 0;
    
    int currentReading = analogRead(PIEZO_PIN);
    
    // Continuously monitor for taps during active state
    if (isActiveForTap) {
        // Debug output every 100ms when active
        if (millis() - lastDebugTime > 100) {
            unsigned long timeLeft = currentTapTimeout - (millis() - tapActivationTime);
            unsigned long gracePeriodLeft = 300 - (millis() - tapActivationTime);
            Serial.printf("ACTIVE - Piezo: %d, Threshold: 150, Grace left: %lu ms, Time left: %lu ms\n", 
                         currentReading, gracePeriodLeft > 1000 ? 0 : gracePeriodLeft, timeLeft);
            lastDebugTime = millis();
        }
        
        checkTapDetected();
    } else if (debugMode) {
        // In debug mode, continuously check for taps with more stable logic
        bool thresholdExceeded = currentReading > DEBUG_PIEZO_THRESHOLD;
        bool spikeDetected = (currentReading - lastReading) > 30; // Require bigger spike
        
        if ((thresholdExceeded || spikeDetected) && (millis() - lastTapTime_static) > DEBOUNCE_MS) {
            lastTapTime_static = millis();
            Serial.printf("DEBUG TAP DETECTED! Reading: %d, Previous: %d, Spike: %d\n", 
                         currentReading, lastReading, (currentReading - lastReading));
            
            // Flash LED briefly to show tap detected
            ledController.showSuccess();
            
            // Send debug tap notification
            bleManager.notifyTapResult(true, 0); // 0ms reaction time for debug
        }
        
        // Debug output every 1 second with current readings
        if (millis() - lastDebugTime > 1000) {
            Serial.printf("DEBUG - Current: %d, Threshold: %d, Last: %d\n", 
                         currentReading, DEBUG_PIEZO_THRESHOLD, lastReading);
            lastDebugTime = millis();
        }
    } else {
        // Baseline monitoring when idle
        if (millis() - lastBaselineTime > 2000) { // Every 2 seconds
            Serial.printf("IDLE - Baseline piezo: %d\n", currentReading);
            lastBaselineTime = millis();
        }
    }
    
    // Update last reading
    lastReading = currentReading;
}

// Debug Mode Methods
void WorkoutDevice::enableDebugMode() {
    debugMode = true;
    Serial.printf("Device %d: Debug mode enabled - tap testing active\n", deviceId);
    ledController.showWaiting(); // Clear any existing patterns
}

void WorkoutDevice::disableDebugMode() {
    debugMode = false;
    Serial.printf("Device %d: Debug mode disabled\n", deviceId);
    ledController.showWaiting(); // Clear any existing patterns
}

// Power Management Functions
void WorkoutDevice::enableLowPowerMode() {
    lowPowerMode = true;
    ledController.setBrightness(LED_BRIGHTNESS_LOW);
    Serial.printf("Device %d: Low power mode enabled\n", deviceId);
}

void WorkoutDevice::disableLowPowerMode() {
    lowPowerMode = false;
    ledController.setBrightness(LED_BRIGHTNESS);
    Serial.printf("Device %d: Normal power mode restored\n", deviceId);
}

void WorkoutDevice::enterSleepMode() {
    Serial.printf("Device %d: Entering deep sleep...\n", deviceId);
    
    // Turn off all LEDs
    ledController.clear();
    
    // Configure wake-up on touch/piezo sensor using EXT1 (ESP32-C6 compatible)
    // GPIO2 is within the supported range (0-7) for ESP32-C6
    uint64_t ext_wakeup_pin_mask = 1ULL << PIEZO_GPIO_NUM;
    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
    
    Serial.printf("Device %d: Entering deep sleep. Wake on GPIO%d (piezo tap)\n", deviceId, PIEZO_GPIO_NUM);
    
    // Enter deep sleep
    esp_deep_sleep_start();
}

bool WorkoutDevice::checkBatteryLevel() {
    unsigned long currentTime = millis();
    
    // Only check battery every BATTERY_CHECK_INTERVAL
    if (currentTime - lastBatteryCheck < BATTERY_CHECK_INTERVAL) {
        return lastBatteryVoltage > LOW_BATTERY_THRESHOLD;
    }
    
    lastBatteryCheck = currentTime;
    lastBatteryVoltage = getBatteryVoltage();
    
    if (lastBatteryVoltage < LOW_BATTERY_THRESHOLD) {
        Serial.printf("Device %d: LOW BATTERY WARNING! %.2fV\n", deviceId, lastBatteryVoltage);
        
        // Flash red LEDs to indicate low battery
        ledController.setPattern(PATTERN_PULSE, COLOR_RED, 2000);
        
        // Enable low power mode automatically
        if (!lowPowerMode) {
            enableLowPowerMode();
        }
        
        return false;
    }
    
    return true;
}

float WorkoutDevice::getBatteryVoltage() {
    // Read battery voltage through voltage divider
    int rawReading = analogRead(BATTERY_PIN);
    
    // Convert to voltage (assuming 3.3V reference and voltage divider)
    // Adjust these values based on your voltage divider circuit
    float voltage = (rawReading / 4095.0) * 3.3 * 2.0; // *2 for voltage divider
    
    return voltage;
}
