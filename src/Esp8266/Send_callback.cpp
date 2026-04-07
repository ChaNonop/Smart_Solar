#include "esp/Send_Callback.h"

CommManager::CommManager(uint8_t rxPin, uint8_t txPin) : _serial(rxPin, txPin) {}

void CommManager::begin(long baudRate) {
    _serial.begin(baudRate);
}

void CommManager::receiveCommands() { //อ่านค่าจาก uno
    if (_serial.available()) {
        String jsonString = _serial.readStringUntil('\n'); 
        
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, jsonString);

        if (!error) {
            // ดึงค่า int 4 ค่าออกมาตาม Format
            int Temp = doc["Temp"];
            int Humid = doc["Humid"];
            int V_solar = doc["V_solar"];
            int V_battery = doc["V_battery"];
            int I = doc["I"];
            int P = doc["P"];
            int Light = doc["Light"];
        }
            Serial.println(F("--- Received Packet from Uno ---"));
            Serial.printf("Value 1: %d\n", val1);
            Serial.printf("Value 2: %d\n", val2);
            Serial.printf("Value 3: %d\n", val3);
            Serial.printf("Value 4: %d\n", val4);
            Serial.println(F("--------------------------------"));
    }
}

void CommManager::sendSensorData(Sensor& sensor) {
    StaticJsonDocument<200> doc; 
    doc["Temp"] = sensor.getTemp();
    doc["Humid"] = sensor.getHumid();
    doc["V_solar"] = sensor.getVSolar();
    doc["V_battery"] = sensor.getVBattery();

    serializeJson(doc, _serial);
    _serial.println(); 

    // พิมพ์ออก Serial Monitor เพื่อเช็คความถูกต้อง
    serializeJson(doc, Serial);
    Serial.println();
}