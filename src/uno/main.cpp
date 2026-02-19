#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "sensor.h"

// ⚠️ ข้อควรระวัง: 
// ใน platformio.ini ของคุณ Pin_Button=2 และ dhtPin=3 (D3)
// ถ้าเราใช้ SoftwareSerial ขา 2, 3 มันจะชนกับ Button และ DHT ครับ!
// ผมจึงเปลี่ยนขา SoftwareSerial เป็น 10(RX), 11(TX) ชั่วคราว
SoftwareSerial espSerial(10, 11); 

// สร้าง Object เซ็นเซอร์ โดยโยนขาที่ประกาศใน platformio.ini เข้าไป
Sensor mySensor(dhtPin, DHT11, Light_Pin, Voltage_Pin_solar, Voltage_Pin_Battery, Current_Pin);

const unsigned long READ_INTERVAL = 2000; 

void setup() {
    Serial.begin(115200);
    espSerial.begin(9600);

    mySensor.begin();
    // กำหนดขา Relay และ Button ที่ main ดีที่สุด (เพราะอยู่นอกหน้าที่การอ่านค่าของ Sensor class)
    pinMode(Relay_Pin, OUTPUT);
    pinMode(Pin_Button, INPUT_PULLUP);
}

void loop() {
    static unsigned long lastUpdateTime = 0; 
    unsigned long currentTime = millis();
    
    // ProcessData();
    mySensor.readData();
    
    if(mySensor.getLight() >= 100){
        digitalWrite(Relay_Pin, HIGH);
    } else {
        digitalWrite(Relay_Pin, LOW);
    }
    if(currentTime - lastUpdateTime >= READ_INTERVAL) {
        lastUpdateTime = currentTime;

        // 3. สร้าง JSON (แยกโค้ดส่วนนี้มาไว้ใน main.cpp)
        StaticJsonDocument<200> doc;
        doc["Temp"] = mySensor.getTemp();
        doc["Humid"] = mySensor.getHumid();
        doc["V_solar"] = mySensor.getVSolar();
        doc["V_battery"] = mySensor.getVBattery();
        doc["I"] = mySensor.getCurrent();
        doc["P"] = mySensor.getPower();
        doc["Light"] = mySensor.getLight();
        doc["Button"] = digitalRead(Pin_Button);

        // 4. ส่งออกทางสาย UART ไปหา ESP8266
        serializeJson(doc, espSerial);
        espSerial.println(); // ต้องมี \n ปิดท้ายเสมอ

        // พิมพ์ดูในคอมพิวเตอร์เพื่อเช็ค
        serializeJson(doc, Serial);
        Serial.println();
    }
}
