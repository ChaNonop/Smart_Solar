#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sensor.h"

#define SCREEN_WIDTH 128 // ความกว้างจอ OLED
#define SCREEN_HEIGHT 64 // ความสูงจอ OLED
#define OLED_RESET    -1 // ขา Reset (ถ้าไม่ได้ใช้ให้ตั้ง -1)
#define SCREEN_ADDRESS 0x3C // Address มาตรฐานของ OLED 128x64 I2C

class DisplayManager {
private:
    Adafruit_SSD1306 _display;
    bool _isAvailable; // ตัวแปรเก็บสถานะว่าเจอจอไหม

public:
    DisplayManager();
    void begin();
    void update(Sensor& sensor); // ส่ง object sensor เข้ามาเพื่อดึงค่าไปโชว์
};

#endif