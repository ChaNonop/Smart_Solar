#ifndef SENSOR_RECEIVER_H
#define SENSOR_RECEIVER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

class SensorReceiver {
private:
    SoftwareSerial _serial;

public:
    SensorReceiver(uint8_t rxPin, uint8_t txPin);
    void begin(long baudRate);
    
    // คอยรับข้อมูล ถ้ามีข้อมูลสมบูรณ์จะคืนค่า true และใส่ข้อมูลลงใน doc
    bool receiveData(JsonDocument& doc); 
};

#endif