#include "uno/Send_Callback.h"

CommManager::CommManager(uint8_t rxPin, uint8_t txPin) : _serial(rxPin, txPin) {}

void CommManager::begin(long baudRate) {
    _serial.begin(baudRate);
}

// void CommManager::receiveCommands() { //อ่านค่า esp
//     if (_serial.available()) {
//         String jsonString = _serial.readStringUntil('\n'); 
        
//         StaticJsonDocument<200> doc;
//         DeserializationError error = deserializeJson(doc, jsonString);

//         if (!error) {
//             // ดึงค่า int 4 ค่าออกมาตาม Format
//             int val1 = doc["val1"];
//             int val2 = doc["val2"];
//             int val3 = doc["val3"];
//             int val4 = doc["val4"];

//             Serial.println(F("--- Received Packet from ESP ---"));
//             // Serial.printf("Value 1: %d\n", val1);
//             // Serial.printf("Value 2: %d\n", val2);
//             // Serial.printf("Value 3: %d\n", val3);
//             // Serial.printf("Value 4: %d\n", val4);
//             Serial.println(F("--------------------------------"));
//         }
//     }
// }

void CommManager::sendSensorData(Sensor& sensor) {
    StaticJsonDocument<200> doc; 
    doc["Temp"] = sensor.getTemp();
    doc["Humid"] = sensor.getHumid();
    doc["V_solar"] = sensor.getVSolar();
    doc["V_battery"] = sensor.getVBattery();
    doc["I"] = sensor.getCurrent();
    doc["P"] = sensor.getPower();
    doc["Light"] = sensor.getLight();

    serializeJson(doc, _serial);
    _serial.println(); 

    // พิมพ์ออก Serial Monitor เพื่อเช็คความถูกต้อง
    serializeJson(doc, Serial);
    Serial.println();
}