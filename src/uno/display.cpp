#include "uno/display.h"

DisplayManager::DisplayManager() : _isAvailable(false) {}

void DisplayManager::begin() {
    Wire.begin();
    
    // แสกนหา Address 0x3C
    Wire.beginTransmission(SCREEN_ADDRESS);
    byte error = Wire.endTransmission();

    if (error == 0) {
        Serial.println(F("OLED found!"));
        
        // กำหนดค่าจอและฟอนต์
        _display.begin(&Adafruit128x64, SCREEN_ADDRESS);
        _display.setFont(Adafruit5x7); // เปลี่ยนฟอนต์เป็น Adafruit5x7 ซึ่งกลมและอ่านง่ายกว่า
        
        _display.clear();
        _display.set2X(); // ขยายตัวอักษร 2 เท่าหน้า Welcome
        _display.println(F("Smart"));
        _display.println(F("Solar"));
        _display.set1X(); // ปรับขนาดกลับเป็น 1 เท่าเพื่อแสดงค่าเซ็นเซอร์
        
        _isAvailable = true;
        delay(1500);
    } else {
        Serial.println(F("OLED NOT found!"));
        _isAvailable = false;
    }
}

void DisplayManager::update(Sensor& sensor) {
    if (!_isAvailable) return;

    // เคลียร์จอ
    _display.clear();
    
    // ใช้ setCursor(column, row) เพื่อกำหนดตำแหน่งเป๊ะๆ และเว้นระยะบรรทัด 
    // Row จะมีตั้งแต่ 0-7 เราจะใช้ Row 0, 2, 4, 6 เพื่อเว้นบรรทัดให้อ่านง่ายสบายตา
    
    // --- แถวที่ 1: ข้อมูล Solar (Row 0) ---
    _display.setCursor(0, 0);
    _display.print(F("Sol: "));
    _display.print(sensor.getVSolar(), 1);
    _display.print(F("V   "));
    _display.print(sensor.getPowerIn(), 1);
    _display.print(F("W"));

    // --- แถวที่ 2: ข้อมูล Battery (Row 2) ---
    _display.setCursor(0, 2);
    _display.print(F("Bat: "));
    _display.print(sensor.getVBattery(), 1);
    _display.print(F("V   "));
    _display.print(sensor.getPowerOut(), 1);
    _display.print(F("W"));

    // --- แถวที่ 3: สภาพแวดล้อม (Row 4) ---
    _display.setCursor(0, 4);
    _display.print(F("T: "));
    _display.print(sensor.getTemp(), 1);
    _display.print(F("C   H: "));
    _display.print(sensor.getHumid(), 0);
    _display.print(F("%"));

    // --- แถวที่ 4: ความสว่าง (Row 6) ---
    _display.setCursor(0, 6);
    _display.print(F("Lux: "));
    _display.print(sensor.getLux(), 0);
}