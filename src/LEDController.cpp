#include "LEDController.h"

LEDController::LEDController() 
    : strip(NUM_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800) {
}

void LEDController::begin(uint8_t deviceId) {
    Serial.println("Initializing LED strip...");
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'
    strip.setBrightness(50); // Set brightness (0-255)
    
    // Show device startup pattern
    showDeviceStartup(deviceId);
    
    Serial.println("LEDs initialized");
}

void LEDController::showDeviceStartup(uint8_t deviceId) {
    // Device startup pattern - flash device ID number of times
    Serial.printf("Flashing device ID %d times...\n", deviceId);
    for (int i = 0; i < deviceId; i++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            strip.setPixelColor(j, strip.Color(0, 255, 0)); // Green
        }
        strip.show();
        delay(300);
        strip.clear();
        strip.show();
        delay(200);
    }
}

void LEDController::handleLEDPattern(const JsonDocument& doc) {
    String patternStr = doc["pattern"];
    uint32_t color = doc["color"] | 0xFF0000; // Default to red
    int duration = doc["duration"] | 1000;   // Default 1 second
    
    LEDPattern pattern = stringToPattern(patternStr);
    
    // Extract RGB components for debugging
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    Serial.printf("LED Pattern: %s, Color: 0x%X (R:%d G:%d B:%d), Duration: %d\n", 
                 patternStr.c_str(), color, r, g, b, duration);
    
    executePattern(pattern, color, duration);
}

void LEDController::setPattern(LEDPattern pattern, uint32_t color, int duration) {
    executePattern(pattern, color, duration);
}

void LEDController::clear() {
    strip.clear();
    strip.show();
}

LEDPattern LEDController::stringToPattern(const String& patternStr) {
    if (patternStr == "solid") return PATTERN_SOLID;
    if (patternStr == "flash") return PATTERN_FLASH;
    if (patternStr == "chase") return PATTERN_CHASE;
    if (patternStr == "rainbow") return PATTERN_RAINBOW;
    if (patternStr == "off") return PATTERN_OFF;
    return PATTERN_UNKNOWN;
}

void LEDController::executePattern(LEDPattern pattern, uint32_t color, int duration) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    
    switch (pattern) {
        case PATTERN_SOLID:
            Serial.printf("Setting %d LEDs to solid color...\n", NUM_LEDS);
            for (int i = 0; i < NUM_LEDS; i++) {
                strip.setPixelColor(i, strip.Color(r, g, b));
            }
            strip.show();
            Serial.println("strip.show() called");
            break;
            
        case PATTERN_FLASH:
            // Flash pattern
            Serial.printf("Starting flash pattern with %d flashes...\n", 3);
            for (int flash = 0; flash < 3; flash++) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, strip.Color(r, g, b));
                }
                strip.show();
                delay(150);
                strip.clear();
                strip.show();
                delay(150);
            }
            Serial.println("Flash pattern completed");
            break;
            
        case PATTERN_CHASE:
            // Chase pattern
            Serial.println("Starting chase pattern...");
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.clear();
                    strip.setPixelColor(i, strip.Color(r, g, b));
                    strip.show();
                    delay(50);
                }
            }
            strip.clear();
            strip.show();
            Serial.println("Chase pattern completed");
            break;
            
        case PATTERN_RAINBOW:
            // Rainbow pattern
            Serial.println("Starting rainbow pattern...");
            for (int j = 0; j < 256; j++) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, Wheel((i + j) & 255));
                }
                strip.show();
                delay(10);
            }
            strip.clear();
            strip.show();
            Serial.println("Rainbow pattern completed");
            break;
            
        case PATTERN_OFF:
            Serial.println("Turning LEDs off...");
            strip.clear();
            strip.show();
            Serial.println("LEDs turned off");
            break;
            
        default:
            Serial.printf("Unknown pattern\n");
            break;
    }
}

// Rainbow wheel function for rainbow pattern
uint32_t LEDController::Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
