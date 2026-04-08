#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h> 
#include "Adafruit_VEML7700.h"

class Sensor {
private:
    DHT _dht;
    Adafruit_VEML7700 _veml; // สร้าง Object VEML7700

    uint8_t _vSolarPin;
    uint8_t _vBatteryPin;
    uint8_t _currentPin_In;
    uint8_t _currentPin_Out;
    float _R1;
    float _R2;

    // ค่าที่ประมวลผลแล้ว
    float _temp;
    float _humid;
    float _vSolar;
    float _vBattery;
    float _current_In;
    float _current_Out;
    float _power_In;
    float _power_Out;
    int _lux;

    byte _sampling;
    float _offset;
    float _sensitivity;


public:
        // Constructor รับค่าขาสัญญาณทั้งหมด
        Sensor( 
            uint8_t pinDht,
            uint8_t dhtType, 
            uint8_t pinVSolar, 
            uint8_t pinVBattery, 
            uint8_t pinCurrent_1,
            uint8_t pinCurrent_2
        );
            
    void begin();
    void readData(); 
    void readDHTData(); 
    void veml_sensorData();
    
    // ฟังก์ชัน Getter ไว้สำหรับให้ main.cpp ดึงค่าไปใช้งาน
    float getTemp();
    float getHumid();

    float getVSolar();
    float getVBattery();

    float getCurrentIn();
    float getCurrentOut();

    float getPowerIn();
    float getPowerOut();
    
    float getLux();
};

#endif 