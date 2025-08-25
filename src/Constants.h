#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Adafruit_NeoPixel.h>

// Pin definitions for XIAO ESP32-C6
#define SDA_PIN D4
#define SCL_PIN D5
#define NEOPIXEL_PIN D9
#define BATTERY_PIN A0
#define PIEZO_PIN A1
#define PIEZO_GPIO_NUM GPIO_NUM_2  // A1 maps to GPIO2 on ESP32-C6

// NeoPixel settings
#define NUM_LEDS 16
#define LED_BRIGHTNESS 30  // Reduced from 100 to 30 (70% power saving)
#define LED_BRIGHTNESS_LOW 10  // Ultra-low power mode
#define LED_BRIGHTNESS_HIGH 60  // High visibility mode

// Power management
#define BATTERY_CHECK_INTERVAL 30000  // Check battery every 30s instead of constantly
#define LOW_BATTERY_THRESHOLD 3.2  // Volts - warn when battery low
#define SLEEP_TIMEOUT_MS 300000  // 5 minutes - enter deep sleep if inactive

// RGB Colors
#define COLOR_OFF    Adafruit_NeoPixel::Color(0, 0, 0)
#define COLOR_RED    Adafruit_NeoPixel::Color(255, 0, 0)
#define COLOR_GREEN  Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_BLUE   Adafruit_NeoPixel::Color(0, 0, 255)
#define COLOR_YELLOW Adafruit_NeoPixel::Color(255, 255, 0)
#define COLOR_PURPLE Adafruit_NeoPixel::Color(255, 0, 255)
#define COLOR_ORANGE Adafruit_NeoPixel::Color(255, 165, 0)
#define COLOR_WHITE  Adafruit_NeoPixel::Color(255, 255, 255)

// Reactive Training Settings
#define PIEZO_THRESHOLD 100  // Increased from 50 to 100 to avoid noise triggers
#define DEBUG_PIEZO_THRESHOLD 80  // Special lower threshold for debug mode
#define TAP_TIMEOUT_MS 1000  // Default fallback timeout (when none specified via BLE)
#define DEBOUNCE_MS 100      // Increased debounce time to 100ms

#endif // CONSTANTS_H
