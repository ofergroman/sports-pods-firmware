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
    // Sports patterns
    if (patternStr == "warmup") return PATTERN_WARMUP;
    if (patternStr == "pulse") return PATTERN_PULSE;
    if (patternStr == "breathing") return PATTERN_BREATHING;
    if (patternStr == "timer") return PATTERN_TIMER;
    if (patternStr == "intensity") return PATTERN_INTENSITY;
    if (patternStr == "rest") return PATTERN_REST;
    if (patternStr == "celebration") return PATTERN_CELEBRATION;
    if (patternStr == "wave") return PATTERN_WAVE;
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
            
        case PATTERN_WARMUP:
            // Gentle orange pulsing
            Serial.println("Warmup pattern - gentle orange pulse...");
            for (int intensity = 50; intensity <= 150; intensity += 10) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, strip.Color(intensity, intensity/2, 0));
                }
                strip.show();
                delay(50);
            }
            for (int intensity = 150; intensity >= 50; intensity -= 10) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, strip.Color(intensity, intensity/2, 0));
                }
                strip.show();
                delay(50);
            }
            break;
            
        case PATTERN_PULSE:
            // Rhythmic pulsing for active workout
            Serial.println("Pulse pattern - active workout...");
            for (int pulse = 0; pulse < 5; pulse++) {
                for (int i = 0; i < NUM_LEDS; i++) {
                    strip.setPixelColor(i, strip.Color(r, g, b));
                }
                strip.show();
                delay(200);
                strip.clear();
                strip.show();
                delay(200);
            }
            break;
            
        case PATTERN_BREATHING:
            // Slow breathing pattern for rest/meditation
            Serial.println("Breathing pattern - rest/meditation...");
            for (int breath = 0; breath < 3; breath++) {
                // Inhale
                for (int intensity = 0; intensity <= 100; intensity += 5) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        strip.setPixelColor(i, strip.Color(0, 0, intensity));
                    }
                    strip.show();
                    delay(80);
                }
                // Exhale
                for (int intensity = 100; intensity >= 0; intensity -= 5) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        strip.setPixelColor(i, strip.Color(0, 0, intensity));
                    }
                    strip.show();
                    delay(80);
                }
            }
            break;
            
        case PATTERN_CELEBRATION:
            // Victory celebration pattern
            Serial.println("Celebration pattern - workout complete!");
            for (int cycle = 0; cycle < 3; cycle++) {
                for (int j = 0; j < 256; j += 8) {
                    for (int i = 0; i < NUM_LEDS; i++) {
                        strip.setPixelColor(i, Wheel((i + j) & 255));
                    }
                    strip.show();
                    delay(20);
                }
            }
            strip.clear();
            strip.show();
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

void LEDController::showWorkoutState(int workoutState) {
    switch (workoutState) {
        case 0: // WORKOUT_IDLE
            clear();
            break;
        case 1: // WORKOUT_WARMUP
            executePattern(PATTERN_WARMUP, 0xFF8000, 1000); // Orange
            break;
        case 2: // WORKOUT_ACTIVE
            executePattern(PATTERN_PULSE, 0xFF0000, 1000); // Red
            break;
        case 3: // WORKOUT_REST
            executePattern(PATTERN_BREATHING, 0x0000FF, 2000); // Blue
            break;
        case 4: // WORKOUT_COOLDOWN
            executePattern(PATTERN_BREATHING, 0x00FF00, 3000); // Green
            break;
        case 5: // WORKOUT_COMPLETE
            executePattern(PATTERN_CELEBRATION, 0xFFFFFF, 3000); // Rainbow
            break;
    }
}

void LEDController::showTimer(int seconds, int totalSeconds) {
    // Show countdown timer as LED progress
    int ledsOn = map(seconds, 0, totalSeconds, 0, NUM_LEDS);
    
    strip.clear();
    for (int i = 0; i < ledsOn; i++) {
        if (seconds <= 10) {
            // Red for last 10 seconds
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        } else if (seconds <= 30) {
            // Yellow for last 30 seconds
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        } else {
            // Green for normal time
            strip.setPixelColor(i, strip.Color(0, 255, 0));
        }
    }
    strip.show();
}

void LEDController::showIntensity(int level) {
    // Show intensity level (1-10) as colored LEDs
    int ledsOn = map(level, 1, 10, 1, NUM_LEDS);
    
    strip.clear();
    for (int i = 0; i < ledsOn; i++) {
        if (level <= 3) {
            // Green for low intensity
            strip.setPixelColor(i, strip.Color(0, 255, 0));
        } else if (level <= 7) {
            // Yellow for medium intensity
            strip.setPixelColor(i, strip.Color(255, 255, 0));
        } else {
            // Red for high intensity
            strip.setPixelColor(i, strip.Color(255, 0, 0));
        }
    }
    strip.show();
}

void LEDController::showCaloriesBurned(int calories) {
    // Flash green once for every 10 calories
    int flashes = calories / 10;
    for (int i = 0; i < flashes && i < 5; i++) {
        for (int j = 0; j < NUM_LEDS; j++) {
            strip.setPixelColor(j, strip.Color(0, 255, 0));
        }
        strip.show();
        delay(200);
        strip.clear();
        strip.show();
        delay(200);
    }
}

void LEDController::showSyncPattern(int deviceId, int totalDevices, unsigned long timestamp) {
    // Synchronized wave pattern across all devices
    unsigned long phase = (timestamp / 100) % 360; // 100ms per degree
    int offset = (deviceId - 1) * (360 / totalDevices); // Distribute devices around circle
    
    for (int i = 0; i < NUM_LEDS; i++) {
        int ledPhase = (phase + offset + (i * 22)) % 360; // 22 degrees per LED
        int intensity = (sin(ledPhase * PI / 180) + 1) * 127; // 0-255 range
        
        strip.setPixelColor(i, strip.Color(intensity, 0, 255 - intensity));
    }
    strip.show();
}
