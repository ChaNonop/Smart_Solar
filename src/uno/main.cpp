#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "sensor.h"

// SoftwareSerial เป็น 10(RX), 11(TX)
SoftwareSerial espSerial(RX, TX); // 10 , 11

// สร้าง Object sensor
Sensor mySensor(dhtPin, DHT11, Light_Pin, Voltage_Pin_solar, Voltage_Pin_Battery, Current_Pin);

// กำหนดเวลา Timers (มิลลิวินาที)
const unsigned long INTERVAL_ANALOG = 50;    // อ่าน Analog ทุกๆ 50ms 
const unsigned long INTERVAL_DHT = 2000;     // อ่าน DHT ทุกๆ 2 วินาที
const unsigned long INTERVAL_SEND = 2500;    // ส่งข้อมูลทาง Serial ทุกๆ 2.5 วินาที

// ตัวแปรเก็บเวลา
unsigned long lastAnalogRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSerialSend = 0;

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);

  mySensor.begin();
  pinMode(Relay_Pin, OUTPUT);
  pinMode(Pin_Button, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Task 1: อ่านค่า Analog และเช็ค Relay (ทุก 50ms)
  if (currentMillis - lastAnalogRead >= INTERVAL_ANALOG) {
      lastAnalogRead = currentMillis;
      
      mySensor.readData();
      
      if(mySensor.getLight() >= 100){
          digitalWrite(Relay_Pin, HIGH);
      } else {
          digitalWrite(Relay_Pin, LOW);
      }
  }
  // Task 2: อ่านค่า DHT11 (ทุก 2 วินาที)
  if (currentMillis - lastDHTRead >= INTERVAL_DHT) {
    lastDHTRead = currentMillis;
    mySensor.readDHTData();
  }
  // Task 3: ส่งข้อมูลไป ESP8266 และขึ้นจอคอม (ทุก 3 วินาที)
  if (currentMillis - lastSerialSend >= INTERVAL_SEND) {
      lastSerialSend = currentMillis;

      StaticJsonDocument<200> doc;
      doc["Temp"] = mySensor.getTemp();
      doc["Humid"] = mySensor.getHumid();
      doc["V_solar"] = mySensor.getVSolar();
      doc["V_battery"] = mySensor.getVBattery();
      doc["I"] = mySensor.getCurrent();
      doc["P"] = mySensor.getPower();
      doc["Light"] = mySensor.getLight();
      doc["Button"] = !digitalRead(Pin_Button); // สลับค่า (ถ้าต่อแบบ PULLUP กดจะเป็น 0, ไม่กดเป็น 1)

      serializeJson(doc, espSerial);
      espSerial.println(); 

      serializeJson(doc, Serial);
      Serial.println();
  }
}
