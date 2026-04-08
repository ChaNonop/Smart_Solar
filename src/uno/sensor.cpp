#include "uno/sensor.h"

Sensor::Sensor(uint8_t pinDht, uint8_t dhtType, uint8_t pinVSolar, uint8_t pinVBattery, uint8_t PinCurrent_1, uint8_t PinCurrent_2)
:   _dht(pinDht, dhtType),
    _veml(),
    _vSolarPin(pinVSolar),
    _vBatteryPin(pinVBattery),
    _currentPin_In(PinCurrent_1),
    _currentPin_Out(PinCurrent_2),
    _R1(29900.0), // ตัวต้านทาน 29.9k Ohm
    _R2(7500.0),  // ตัวต้านทาน 7.5k Ohm
   
    _temp(0.0),
    _humid(0.0),
    
    _vSolar(0.0),
    _vBattery(0.0),
    _current_In(0.0),
    _current_Out(0.0),

    _power_In(0.0),
    _power_Out(0.0),

    _lux(0),
    _sampling(10),
    _offset(2.5),
    _sensitivity(0.185)
{
}

void Sensor::begin() {
    _dht.begin();
    pinMode(_vSolarPin, INPUT);
    pinMode(_vBatteryPin, INPUT);
    pinMode(_currentPin_In, INPUT);
    pinMode(_currentPin_Out, INPUT);
    Wire.begin();

    if (!_veml.begin()) {
    Serial.println("Sensor Veml7700 not found");
    while (1);
    }
    Serial.println("Sensor Veml7700 found");
}

// ฟังก์ชันที่ 1: อ่านค่า Analog (Voltage & Current)
void Sensor::readData() {
    float resistorRatio = (_R1 + _R2) / _R2;
    long sumVSolar = 0;
    long sumVBattery = 0;
    long sumCurrent_In = 0;
    long sumCurrent_Out = 0;

    for(int j = 0; j < _sampling; j++){
        sumVSolar += analogRead(_vSolarPin);
        sumVBattery += analogRead(_vBatteryPin);
        sumCurrent_In += analogRead(_current_In);
        sumCurrent_Out += analogRead(_current_Out);
        delay(2);
    }

    float avgVSolar = (float)sumVSolar / _sampling;
    float avgVBattery = (float)sumVBattery / _sampling;
    float avgCurrent_In = (float)sumCurrent_In / _sampling;
    float avgCurrent_Out = (float)sumCurrent_Out / _sampling;

    float analogToVoltage = 5.0 / 1023.0; 
    
    _vSolar = (avgVSolar * analogToVoltage) * resistorRatio;
    _vBattery = (avgVBattery * analogToVoltage) * resistorRatio;
    
    // การคำนวณสำหรับเซ็นเซอร์กระแส ACS712
    // ค่า Offset ของ ACS712 ปกติคือ VCC/2 = 2.5V

    // ค่า Sensitivity ขึ้นอยู่กับรุ่นของ ACS712:
    // รุ่น 5A  = 0.185 V/A
    // รุ่น 20A = 0.100 V/A

    // คำนวณกระแสโดยหักลบ Offset และหารด้วย Sensitivity
    _current_In = ((avgCurrent_In * analogToVoltage) - _offset) / _sensitivity; // 2 คือ offset && 0.185 คือ sensivity
    _current_Out = ((avgCurrent_Out * analogToVoltage) - _offset) / _sensitivity; 

    _power_In = _vSolar * _current_In;
    _power_Out = _vBattery * _current_Out;
}

// อ่านค่า DHT 
void Sensor::readDHTData() {
    float t = _dht.readTemperature();
    float h = _dht.readHumidity();
    
    // อ่านสำเร็จค่อยบันทึกค่า ถ้าไม่สำเร็จให้ใช้ค่าเดิมไปก่อน
    if (!isnan(t) && !isnan(h)) {
        _temp = t;
        _humid = h;
    } else {
        Serial.println(F("Failed to read DHT!"));
    }
}
// อ่านค่า Veml7700
void Sensor::veml_sensorData(){
      // to read lux using automatic method, specify VEML_LUX_AUTO
  _lux = _veml.readLux(VEML_LUX_AUTO);

}


// ---- Getters ----
float Sensor::getTemp() { return .2f * (float)_temp; }
float Sensor::getHumid() { return .2f * (float)_humid; }
float Sensor::getVSolar() { return .2f * (float)_vSolar; }
float Sensor::getVBattery() { return .2f * (float)_vBattery; }

float Sensor::getCurrentIn() { return .2f * (float)_current_In; }
float Sensor::getCurrentOut() { return .2f * (float)_current_Out; }

float Sensor::getPowerIn() { return .2f * (float)_power_In; }
float Sensor::getPowerOut() { return .2f * (float)_power_Out; }

float Sensor::getLux() { return .2f * (float)_lux; }


