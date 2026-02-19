#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <DHT.h>

class Sensor {
private:
    DHT _dht;
    uint8_t _analogPin;
    float _temp;
    float _humid;
    int _analogValue;
    int sensorPins[4] ={Light_Pin,Voltage_Pin_solar,Voltage_Pin_battery,Current_Pin}
    int _R1;
    int _R2;


public:
    Sensor(uint8_t dhtPin, uint8_t dhtType, uint8_t aPin);
    byte sampling;

    void begin();
    void readData();
    int Data[4];
    void ProcessData();
    void getTemp_humid();
    void sendData();
    void Relay(bool state);
};

#endif 