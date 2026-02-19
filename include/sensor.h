#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class Sensor {
private:
    DHT _dht;
    
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
    Sensor( uint8_t dhtPin,
            uint8_t dhtType, 
            uint8_t lightPin, 
            uint8_t vSolarPin, 
            uint8_t vBatteryPin, 
            uint8_t currentPin);
    void begin();
        // แยกฟังก์ชันการอ่านค่าออกจากกัน
    void readData(); 
    void readDHTData(); 

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