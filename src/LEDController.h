#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include "Constants.h"

enum LEDPattern {
    PATTERN_SOLID,
    PATTERN_FLASH,
    PATTERN_CHASE,
    PATTERN_RAINBOW,
    PATTERN_OFF,
    // Sports/Workout patterns
    PATTERN_WARMUP,
    PATTERN_PULSE,
    PATTERN_BREATHING,
    PATTERN_TIMER,
    PATTERN_INTENSITY,
    PATTERN_REST,
    PATTERN_CELEBRATION,
    PATTERN_WAVE,
    PATTERN_UNKNOWN
};

class LEDController {
public:
    LEDController();
    void begin(uint8_t deviceId);
    void handleLEDPattern(const JsonDocument& doc);
    void setPattern(LEDPattern pattern, uint32_t color = 0xFF0000, int duration = 1000);
    void showDeviceStartup(uint8_t deviceId);
    void clear();
    
    // Sports/Workout specific patterns
    void showWorkoutState(int workoutState);
    void showTimer(int seconds, int totalSeconds);
    void showIntensity(int level); // 1-10 intensity
    void showCaloriesBurned(int calories);
    void showSyncPattern(int deviceId, int totalDevices, unsigned long timestamp);
    
    // Reactive training patterns
    void showActiveTarget();    // Bright flashing for tap target
    void showSuccess();         // Green flash for successful tap
    void showMissed();          // Red flash for missed tap
    void showWaiting();         // Dim pattern while waiting
    
    // Power management
    void setBrightness(uint8_t brightness);
    
private:
    LEDPattern stringToPattern(const String& patternStr);
    void executePattern(LEDPattern pattern, uint32_t color, int duration);
    uint32_t Wheel(byte WheelPos);
    
    Adafruit_NeoPixel strip;
};

#endif // LED_CONTROLLER_H
