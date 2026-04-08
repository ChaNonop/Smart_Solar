#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "sensor.h"

#define SCREEN_ADDRESS 0x3C // Address มาตรฐานของ OLED 128x64 I2C

class DisplayManager {
private:
    SSD1306AsciiWire _display; // เปลี่ยนมาใช้คลาสของ SSD1306Ascii
    bool _isAvailable;

public:
    DisplayManager();
    void begin();
    void update(Sensor& sensor);
};

#endif