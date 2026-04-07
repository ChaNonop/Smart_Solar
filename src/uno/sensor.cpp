#include "uno/sensor.h"

Sensor::Sensor(uint8_t pinDht, uint8_t dhtType, uint8_t pinLight, uint8_t pinVSolar, uint8_t pinVBattery, uint8_t PinCurrent_1, uint8_t PinCurrent_2)
:   _dht(pinDht, dhtType),
    _lcd(0x27, 16, 2),
    _lightPin(pinLight),
    _vSolarPin(pinVSolar),
    _vBatteryPin(pinVBattery),
    _current_In(PinCurrent_1),
    _current_Out(PinCurrent_2),
    _temp(0.0),
    _humid(0.0),
    _light(0),
    _vSolar(0.0),
    _vBattery(0.0),
    _power_Out(0.0),
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
    pinMode(_current_In, INPUT);
    pinMode(_current_Out, INPUT);
    pinMode(Relay_Pin, OUTPUT);
    pinMode(Pin_Button, INPUT_PULLUP);

    if (!veml.begin()) {
    Serial.println("Sensor not found");
    while (1);
    }
    Serial.println("Sensor found");
}

void Sensor::displayLCD() {
    _lcd.setCursor(0, 0);
    // _lcd.print("V:"); _lcd.print(_vSolar, 1);
    // _lcd.print(" I:"); _lcd.print(_current, 2);
    // _lcd.print(" P:"); _lcd.print((int)_power);

    // _lcd.setCursor(0, 1);
    // _lcd.print("T:"); _lcd.print(_temp, 1);
    // _lcd.print("C H:"); _lcd.print((int)_humid);
    // _lcd.print("%  "); 
}

// ฟังก์ชันที่ 1: อ่านค่า Analog 
void Sensor::readData() {
    long sumLight = 0, sumVSolar = 0, sumVBattery = 0,sumCurrent_In = 0, sumCurrent_Out = 0;
    
    for(int j = 0; j < _sampling; j++){
        sumLight += analogRead(_lightPin);
        sumVSolar += analogRead(_vSolarPin);
        sumVBattery += analogRead(_vBatteryPin);
        sumCurrent_In += analogRead(_currentPin[1]);
        sumCureent_Out += analogRead(_currentPin[2]);
    }

    float avgLight = (float)sumLight / _sampling;
    float avgVSolar = (float)sumVSolar / _sampling;
    float avgVBattery = (float)sumVBattery / _sampling;
    float avgCurrent_In = (float)sumCurrent_In / _sampling;
    float avgCurrent_Out = (float)sumCurrent_Out / _sampling;

    _light = (int)avgLight;

    float analogToVoltage = 5.0 / 1023.0; 
    float resistorRatio = (_R1 + _R2) / _R2;
    
    _vSolar = (avgVSolar * analogToVoltage) * resistorRatio;
    _vBattery = (avgVBattery * analogToVoltage) * resistorRatio;
    _current_In = avgCurrent_In * analogToVoltage;
    _current_Out = avgCurrent_Out * analogToVoltage;  
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
void sensor::veml_sensor(){
      // to read lux using automatic method, specify VEML_LUX_AUTO
  float lux = veml.readLux(VEML_LUX_AUTO);

  Serial.println("------------------------------------");
  Serial.print("Lux = "); Serial.println(lux);
  Serial.println("Settings used for reading:");
  Serial.print(F("Gain: "));
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }
  Serial.print(F("Integration Time (ms): "));
  switch (veml.getIntegrationTime()) {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }
}


// ---- Getters ----
float Sensor::getTemp() { return .2f * (float)_temp; }
float Sensor::getHumid() { return .2f * (float)_humid; }
int Sensor::getLight() { return .2f * (float)_light; }
float Sensor::getVSolar() { return .2f * (float)_vSolar; }
float Sensor::getVBattery() { return .2f * (float)_vBattery; }
float Sensor::getCurrent(){ 
    return .2f * (float)_current_In && .2f * (float)_current_Out; 
}
float Sensor::getPower() { 
    return .2f * (float)_power && .2f * (float)_power_Out;
}
