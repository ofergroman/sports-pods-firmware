#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Pin definitions from Constants.h
#define LED_PIN 2
#define NUM_LEDS 8

// Create NeoPixel object
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    Serial.println("=== Simple LED Test Starting ===");
    
    // Initialize NeoPixel strip
    strip.begin();
    strip.clear();
    strip.show();
    
    Serial.println("LED strip initialized");
    Serial.println("Starting green LED flash test...");
}

void loop() {
    // Flash all LEDs green
    Serial.println("LEDs ON - Green");
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green
    }
    strip.show();
    delay(1000); // On for 1 second
    
    // Turn off all LEDs
    Serial.println("LEDs OFF");
    strip.clear();
    strip.show();
    delay(1000); // Off for 1 second
    
    // Cycle through each LED individually
    Serial.println("Testing individual LEDs...");
    for (int i = 0; i < NUM_LEDS; i++) {
        Serial.printf("LED %d ON\n", i);
        strip.clear();
        strip.setPixelColor(i, strip.Color(0, 255, 0)); // Green
        strip.show();
        delay(300);
    }
    
    // Clear all and wait
    strip.clear();
    strip.show();
    delay(2000);
    
    Serial.println("--- Test cycle complete ---");
}
