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
    
private:
    LEDPattern stringToPattern(const String& patternStr);
    void executePattern(LEDPattern pattern, uint32_t color, int duration);
    uint32_t Wheel(byte WheelPos);
    
    Adafruit_NeoPixel strip;
};

#endif // LED_CONTROLLER_H
