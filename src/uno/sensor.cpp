#include "sensor.h"

sensor::sensor(uint8_t dhtPin, uint8_t dhtType, uint8_t aPin)
:   _dht(dht),
    _analogPin(aPin),
    _temp(0),
    _humid(0),
    _analogValue(0)
    _R1(30000),
    _R2(7500)
{
    //constructure
    sampling = 10;
}

void sensor::begin()
{
    dht.begin();
}

void sensor::ProcessData(Data[4]){
    int light = Data[0];
    int vsolar = Data[1]*(R1+R2)/R2;
    int vbattery = Data[2]*(R1+R2)/R2;
    int current = Data[3];
    int power = vsolar * current;
}
void sensor::readData(){
    int readbutton = digitalRead(Pin_Button);
    for (int i = 0; i < Sensor_Pin; i++) {
        long sum = 0;
        for(int j = 0; j < sampling; j++){
            sum += analogRead(sensorPins[i]);
            delay(10);
        }
        sensorValues[i] = (float)sum / sampling;
    }
    Data(sensorValues[0],
                sensorValues[1], 
                sensorValues[2], 
                sensorValues[3]);
}
void Relay(bool state){
    digitalWrite(Relay_Pin, state);
}

float sensor::getTemp_humid(){
    _temp = dht.readTemperature();
    _humid = dht.readHumidity();
    if (isnan(_temp) || isnan(_humid)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    return _temp , _humid;
}
void sensor::sendData(){
    static unsigned long lastUpdateTime = 0; 
    // Local Variable (กระดาษทด) ดึงเวลาปัจจุบันมาเช็ค
    unsigned long currentTime = millis();
    if(currentTime - lastUpdateTime >= time_Read){
        lastUpdateTime = currentTime;
        SaticjsonDoc<200> doc;
        doc["Temp"] = sensor.PrecessData.getTemp_humid._temp();
        doc["Humid"] = sensor.PrecessData.getTemp_humid._humid();
        doc["V_solar"] = sensor.PrecessData.V_solar();
        doc["V_battery"] = sensor.PrecessData.V_battery();
        doc["I"] = sensor.PrecessData.current();
        doc["P"] = sensor.PrecessData.Power();
        doc["Light"] = sensor.PrecessData.light();
    }
    
    // ส่งข้อมูลออกไปทาง SoftwareSerial ไปหา ESP8266
    serializeJson(doc, espSerial);
    espSerial.println(); // ปิดท้ายด้วย New line เพื่อให้ฝั่งรับรู้ว่าจบแพ็กเกจ

    // ปริ้นท์ดูบน Serial Monitor ของคอมด้วย
    serializeJson(doc, Serial);
    Serial.println();
}