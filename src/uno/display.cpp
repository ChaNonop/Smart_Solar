#include "uno/display.h"

// Constructor กำหนดค่าเริ่มต้นให้จอ
DisplayManager::DisplayManager() : _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), _isAvailable(false) {}

void DisplayManager::begin() {
    Wire.begin();
    
    // แสกนหา Address 0x3C ว่ามีการเชื่อมต่อจออยู่หรือไม่
    Wire.beginTransmission(SCREEN_ADDRESS);
    byte error = Wire.endTransmission();

    if (error == 0) {
        Serial.println(F("OLED 128x64 found at address 0x3C!"));
        
        // ถ้าเจอจอ ให้ Initialize เลย
        if(_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
            _isAvailable = true;
            _display.clearDisplay();
            _display.setTextSize(1);
            _display.setTextColor(SSD1306_WHITE);
            _display.setCursor(0, 0);
            _display.println(F("Smart Solar Init..."));
            _display.display();
            delay(1000); // โชว์ข้อความเริ่มโปรแกรม 1 วินาที
        }
    } else {
        // ถ้าไม่เจอจอ ปริ้นบอกผ่าน Serial
        Serial.println(F("OLED 128x64 NOT found! Skipping display updates."));
        _isAvailable = false;
    }
}

void DisplayManager::update(Sensor& sensor) {
    if (!_isAvailable) return; // ถ้าไม่เจอจอตั้งแต่แรก ให้ข้ามฟังก์ชันนี้ไปเลย (ลดการกระตุก)

    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);

    // --- แถวที่ 1: ข้อมูล Solar ---
    _display.print(F("Sol: "));
    _display.print(sensor.getVSolar(), 1);
    _display.print(F("V "));
    _display.print(sensor.getPowerIn(), 1);
    _display.println(F("W"));

    // --- แถวที่ 2: ข้อมูล Battery ---
    _display.print(F("Bat: "));
    _display.print(sensor.getVBattery(), 1);
    _display.print(F("V "));
    _display.print(sensor.getPowerOut(), 1);
    _display.println(F("W"));

    // --- แถวที่ 3: สภาพแวดล้อม ---
    _display.print(F("T:"));
    _display.print(sensor.getTemp(), 1);
    _display.print(F("C H:"));
    _display.print(sensor.getHumid(), 0);
    _display.println(F("%"));

    // --- แถวที่ 4: ความสว่าง ---
    _display.print(F("Lux: "));
    _display.println(sensor.getLux(), 0);

    _display.display(); // สั่งวาดข้อมูลทั้งหมดลงจอ
}