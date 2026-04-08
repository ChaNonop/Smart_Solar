#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h> 
#include "Adafruit_VEML7700.h"

class Sensor : Adafruit_VEML7700 {
private:
    DHT _dht;

    uint8_t _vSolarPin;
    uint8_t _vBatteryPin;
    uint8_t _currentPin_In;
    uint8_t _currentPin_Out;
    uint8_t _R1;
    uint8_t _R2;

    Adafruit_VEML7700 veml = _Adafruit_VEML7700();

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
    void Button(bool state);
    void veml_sensor();
    

    // ฟังก์ชัน Getter ไว้สำหรับให้ main.cpp ดึงค่าไปใช้งาน
    float getTemp();
    float getHumid();
    int getLight();
    float getVSolar();
    float getVBattery();
    float getCurrent();
    float getPower();
    float getLux();
};

#endif 