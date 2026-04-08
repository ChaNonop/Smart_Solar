#include "uno/send_command.h"

CommManager::CommManager(uint8_t rxPin, uint8_t txPin) : _serial(rxPin, txPin) {}

void CommManager::begin(long baudRate) {
    _serial.begin(baudRate);
}

void CommManager::receiveCommands() { //อ่านค่า esp
    if (_serial.available()) {
        String jsonString = _serial.readStringUntil('\n'); 
        
        StaticJsonDocument<100> doc;
        DeserializationError error = deserializeJson(doc, jsonString);

        if (!error) {
            // ดึงค่าออกมาตาม Format
            int status = doc["status"]; // อัปเดตว่ารับค่ามาแล้ว
            int Num = doc["์Num"]; // อัปเดตว่าได้รับค่ามากี่ตัว

            Serial.println(F("--- Received Packet from ESP ---"));
            Serial.print(F("Statust: ")); 
            Serial.println(status);
            Serial.print(F("Num: ")); 
            Serial.println(Num);
            Serial.println(F("--------------------------------"));
        }
    }
}

void CommManager::sendSensorData(Sensor& sensor) {
    StaticJsonDocument<200> doc; 
    doc["Temp"] = sensor.getTemp();
    doc["Humid"] = sensor.getHumid();
    doc["V_solar"] = sensor.getVSolar();
    doc["V_battery"] = sensor.getVBattery();
    doc["I_In"] = sensor.getCurrent(_current_In);
    doc["I_Out"] = sensor.getCurrent(_current_Out);
    doc["Power_In"] = sensor.getPower(_power_In);
    doc["Power_Out"] = sensor.getPower(_power_out);
    doc["lux"] = sensor.veml_sensor();

    serializeJson(doc, _serial);
    _serial.println(); 

    // พิมพ์ออก Serial Monitor เพื่อเช็คความถูกต้อง
    serializeJson(doc, Serial);
    Serial.println();
}