#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "uno/sensor.h"
#include "uno/Send_command.h"

// สร้าง Object sensor
Sensor mySensor(dhtPin,DHT11,Voltage_Pin_solar,Voltage_Pin_Battery,Current_Pin_In,Current_Pin_Out);

CommManager comm(2, 3); // RX = 2, TX = 3

// 2. กำหนดช่วงเวลาทำงาน
const unsigned long INTERVAL_ANALOG = 100;    
const unsigned long INTERVAL_DHT = 2000;     
const unsigned long INTERVAL_SEND = 3000;    

// ตัวแปรเก็บเวลา
unsigned long lastAnalogRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSerialSend = 0;


void setup() {
  Serial.begin(115200);
  mySensor.begin();
  comm.begin(9600); // ความเร็วของ SoftwareSerial สำหรับคุยกับ ESP
}

void loop() {
  unsigned long currentMillis = millis();
  
  // --- Task 1: อ่านค่า Analog ---
  if (currentMillis - lastAnalogRead >= INTERVAL_ANALOG) {
      lastAnalogRead = currentMillis;
      mySensor.readData();
  }

  // --- Task 2: อ่านค่า DHT11 ---
  if (currentMillis - lastDHTRead >= INTERVAL_DHT) {
      lastDHTRead = currentMillis;
      mySensor.readDHTData();
  }
  // --- Task 3: รับค่าจาก ESP8266 ---
  comm.receiveCommands();

  // --- Task 4: ส่งข้อมูลไป ESP8266 ---
  if (currentMillis - lastSerialSend >= INTERVAL_SEND) {
      lastSerialSend = currentMillis;
      comm.sendSensorData(mySensor); // ส่ง Object sensor ให้ Comm จัดการดึงค่าไปส่ง
  }
}