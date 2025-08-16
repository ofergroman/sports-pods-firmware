#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Adafruit_NeoPixel.h>

// Pin definitions for XIAO ESP32-C6
#define SDA_PIN D4
#define SCL_PIN D5
#define NEOPIXEL_PIN D9
#define BATTERY_PIN A0

// NeoPixel settings
#define NUM_LEDS 16
#define LED_BRIGHTNESS 100

// RGB Colors
#define COLOR_OFF    Adafruit_NeoPixel::Color(0, 0, 0)
#define COLOR_RED    Adafruit_NeoPixel::Color(255, 0, 0)
#define COLOR_GREEN  Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_BLUE   Adafruit_NeoPixel::Color(0, 0, 255)

#endif // CONSTANTS_H
