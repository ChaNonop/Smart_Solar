#include "esp/Sensor_reciver.h"

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
            // ดึงค่า ออกมาตาม Format
            float Temp = doc["Temp"];
            float Humid = doc["Humid"];
            float V_solar = doc["V_solar"];
            float V_battery = doc["V_battery"];
            float I_In = doc["I_In"];
            float I_Out = doc["I_Out"];
            float Power_In = doc["Power"];
            float Power_Out = doc["Power_Out"];
            float Light = doc["Light"];
        }
            Serial.println(F("--- Received Packet from Uno ---"));
            Serial.printf("Value 1: %d\n", val1);
            Serial.printf("Value 2: %d\n", val2);
            Serial.printf("Value 3: %d\n", val3);
            Serial.printf("Value 4: %d\n", val4);
            Serial.println(F("--------------------------------"));
    }
}

// ส่งค่าไป
void CommManager::sendSensorData(Sensor& sensor) {
    StaticJsonDocument<200> doc; 
    doc["status"] = 1;
    doc["V_battery"] = sensor.getVBattery();

    serializeJson(doc, _serial);
    _serial.println(); 

    // พิมพ์ออก Serial Monitor เพื่อเช็คความถูกต้อง
    serializeJson(doc, Serial);
    Serial.println();
}