#include <Arduino.h>
#include <DHT.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "sensor.h"

Sensor(uint8_t dhtPin, 11, uint8_t aPin);

// ใช้ขา 2(RX), 3(TX) คุยกับ ESP8266
sensor.sendata.SoftwareSerial spSerial(2, 3)

byte time_Read = 100;

void setup() {
    sensor.begin();
    Serial.begin(115200);
}

void loop() {
    sensor.readData();
    sensor.ProcessData();
    if(light >= 100){
      Relay(true);
    }else{
      Relay(false)
    }
    sensor.sendData();
}
