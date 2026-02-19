#include "sensor.h"

Sensor::Sensor(uint8_t dhtPin, uint8_t dhtType, uint8_t lightPin, uint8_t vSolarPin, uint8_t vBatteryPin, uint8_t currentPin)
:   _dht(dhtPin, dhtType),
    _lightPin(lightPin),
    _vSolarPin(vSolarPin),
    _vBatteryPin(vBatteryPin),
    _currentPin(currentPin),
    _temp(0.0),
    _humid(0.0),
    _light(0),
    _vSolar(0.0),
    _vBattery(0.0),
    _current(0.0),
    _power(0.0),
    _R1(30000.0), // ตัวต้านทาน 30k Ohm
    _R2(7500.0),  // ตัวต้านทาน 7.5k Ohm
    _sampling(10)
{
}

void Sensor::begin() {
    _dht.begin();
    pinMode(_lightPin, INPUT);
    pinMode(_vSolarPin, INPUT);
    pinMode(_vBatteryPin, INPUT);
    pinMode(_currentPin, INPUT);
}

// void sensor::readData(){
//     int readbutton = digitalRead(Pin_Button);
//     for (int i = 0; i < Sensor_Pin; i++) {
//         long sum = 0;
//         for(int j = 0; j < sampling; j++){
//             sum += analogRead(sensorPins[i]);
//             delay(10);
//         }
//         sensorValues[i] = (float)sum / sampling;
//     }
//     Data(sensorValues[0],
//                 sensorValues[1], 
//                 sensorValues[2], 
//                 sensorValues[3]);
// }

// ฟังก์ชันที่ 1: อ่านค่า Analog (ทำงานเร็ว ไม่มี delay)
// ---------------------------------------------------------
void Sensor::readData() {
    long sumLight = 0, sumVSolar = 0, sumVBattery = 0, sumCurrent = 0;
    
    // เอา delay(10) ออก เพื่อไม่ให้โปรแกรมค้าง การทำงานตรงนี้จะใช้เวลาแค่ไม่ถึง 5ms
    for(int j = 0; j < _sampling; j++){
        sumLight += analogRead(_lightPin);
        sumVSolar += analogRead(_vSolarPin);
        sumVBattery += analogRead(_vBatteryPin);
        sumCurrent += analogRead(_currentPin);
    }

    float avgLight = (float)sumLight / _sampling;
    float avgVSolar = (float)sumVSolar / _sampling;
    float avgVBattery = (float)sumVBattery / _sampling;
    float avgCurrent = (float)sumCurrent / _sampling;

    _light = (int)avgLight;

    float analogToVoltage = 5.0 / 1023.0; 
    float resistorRatio = (_R1 + _R2) / _R2;
    
    _vSolar = (avgVSolar * analogToVoltage) * resistorRatio;
    _vBattery = (avgVBattery * analogToVoltage) * resistorRatio;
    _current = avgCurrent * analogToVoltage; 
    _power = _vSolar * _current;
}

// อ่านค่า DHT 
void Sensor::readDHTData() {
    float t = _dht.readTemperature();
    float h = _dht.readHumidity();
    
    // ถ้าอ่านสำเร็จค่อยบันทึกค่า ถ้าไม่สำเร็จให้ใช้ค่าเดิมไปก่อน
    if (!isnan(t) && !isnan(h)) {
        _temp = t;
        _humid = h;
    } else {
        Serial.println(F("Failed to read DHT!"));
    }
}

// void Sensor::readData() {
//     float t = _dht.readTemperature();
//     float h = _dht.readHumidity();
//     if (!isnan(t) && !isnan(h)) {
//         _temp = t;
//         _humid = h;
//     } else {
//         Serial.println(F("Failed to read DHT!"));
//     }

//     // ---- 2. อ่านค่า Analog แบบ Sampling (ค่าเฉลี่ย) ----
//     long sumLight = 0, sumVSolar = 0, sumVBattery = 0, sumCurrent = 0;
    
//     for(int j = 0; j < _sampling; j++){
//         sumLight += analogRead(_lightPin);
//         sumVSolar += analogRead(_vSolarPin);
//         sumVBattery += analogRead(_vBatteryPin);
//         sumCurrent += analogRead(_currentPin);
//         delay(10);
//     }

//     float avgLight = (float)sumLight / _sampling;
//     float avgVSolar = (float)sumVSolar / _sampling;
//     float avgVBattery = (float)sumVBattery / _sampling;
//     float avgCurrent = (float)sumCurrent / _sampling;

//     // ---- 3. แปลงค่าดิบให้เป็นค่าจริง (Process Data) ----
//     _light = (int)avgLight;

//     // สมการแบ่งแรงดัน: (ค่าดิบ * อัตราส่วนแรงดันอ้างอิง) * ตัวคูณ R
//     float analogToVoltage = 5.0 / 1023.0; // Arduino Uno ทำงานที่ 5V 10-bit
//     float resistorRatio = (_R1 + _R2) / _R2;
    
//     _vSolar = (avgVSolar * analogToVoltage) * resistorRatio;
//     _vBattery = (avgVBattery * analogToVoltage) * resistorRatio;
    
//     // กระแสไฟฟ้า: ค่าชั่วคราว ดึงจาก Analog ตรงๆ (หากใช้ ACS712 ต้องลบออฟเซ็ต)
//     _current = avgCurrent * analogToVoltage; 
    
//     _power = _vSolar * _current;
// }

// ---- Getters ----
float Sensor::getTemp() { return _temp; }
float Sensor::getHumid() { return _humid; }
int Sensor::getLight() { return _light; }
float Sensor::getVSolar() { return _vSolar; }
float Sensor::getVBattery() { return _vBattery; }
float Sensor::getCurrent() { return _current; }
float Sensor::getPower() { return _power; }
