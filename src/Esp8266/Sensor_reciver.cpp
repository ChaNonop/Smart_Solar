#include "esp/Sensor_reciver.h"

SensorReceiver::SensorReceiver(uint8_t rxPin, uint8_t txPin) : _serial(rxPin, txPin) {}

void SensorReceiver::begin(long baudRate) {
    _serial.begin(baudRate);
}

bool SensorReceiver::receiveData(JsonDocument& doc) {
    if (_serial.available()) {
        String jsonString = _serial.readStringUntil('\n'); 
        
        // ทดสอบ Deserialize เพื่อเช็คว่าข้อมูลที่รับมาเป็น JSON ที่ถูกต้องหรือไม่
        DeserializationError error = deserializeJson(doc, jsonString);

        if (!error) {
            Serial.println(F("Received valid JSON from Nano."));
            return true;
        } else {
            Serial.print(F("Failed to parse JSON: "));
            Serial.println(error.c_str());
            return false;
        }
    }
    return false;
}