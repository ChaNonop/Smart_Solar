#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
class Sensor {
private:
    DHT _dht;
    LiquidCrystal_I2C _lcd; // สร้าง Object สำหรับ LCD

    uint8_t _lightPin;
    uint8_t _vSolarPin;
    uint8_t _vBatteryPin;
    uint8_t _currentPin;
    
    // ค่าที่ประมวลผลแล้ว
    float _temp;
    float _humid;
    int _light;
    float _vSolar;
    float _vBattery;
    float _current;
    float _power;

    // ค่าคงที่วงจร (Voltage Divider)
    float _R1;
    float _R2;
    byte _sampling;

public:
        // Constructor รับค่าขาสัญญาณทั้งหมด
        Sensor( uint8_t pinDht,
            uint8_t dhtType, 
            uint8_t pinLight, 
            uint8_t pinVSolar, 
            uint8_t pinVBattery, 
            uint8_t pinCurrent);
            
    void begin();
    void readData(); 
    void readDHTData(); 
    void Button(bool state);
    
    // ฟังก์ชันใหม่สำหรับแสดงค่าขึ้นจอ LCD
    void displayLCD(); 

    // ฟังก์ชัน Getter ไว้สำหรับให้ main.cpp ดึงค่าไปใช้งาน
    float getTemp();
    float getHumid();
    int getLight();
    float getVSolar();
    float getVBattery();
    float getCurrent();
    float getPower();
};

#endif 