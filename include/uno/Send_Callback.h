#ifndef SEND_CALL_BACK_H
#define SEND_CALL_BACK_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "sensor.h" 

class CommManager {
private:
    SoftwareSerial _serial;

public:
    CommManager(uint8_t rxPin, uint8_t txPin);
    void begin(long baudRate);
    
    void receiveCommands();
    void sendSensorData(Sensor& sensor); // รับ Reference ของ Sensor มาใช้งาน
};

#endif