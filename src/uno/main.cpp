#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "uno/sensor.h"
#include "uno/Send_Callback.h"
#include "uno/Button_Manange.h"

// สร้าง Object sensor
Sensor mySensor(dhtPin,DHT11, Light_Pin, Voltage_Pin_solar, Voltage_Pin_Battery, Current_Pin);

// 1. สร้าง Objects ของระบบต่างๆ
ButtonManager btnManager;
CommManager comm(3, 1); // RX=3, TX=1

// 2. กำหนดช่วงเวลาทำงาน
const unsigned long INTERVAL_ANALOG = 50;    
const unsigned long INTERVAL_DHT = 2000;     
const unsigned long INTERVAL_SEND = 2500;    
const unsigned long INTERVAL_LCD = 1000; 

// ตัวแปรเก็บเวลา
unsigned long lastAnalogRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSerialSend = 0;
unsigned long lastLCDUpdate = 0;


void setup() {
  Serial.begin(115200);
  
  mySensor.begin();
  btnManager.begin();
  comm.begin(9600);
  
  pinMode(Relay_Pin, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // --- Task 1: อ่านค่า Analog ---
  if (currentMillis - lastAnalogRead >= INTERVAL_ANALOG) {
      lastAnalogRead = currentMillis;
      mySensor.readData();
      
      if(mySensor.getLight() >= 100) digitalWrite(Relay_Pin, HIGH);
      else digitalWrite(Relay_Pin, LOW);
  }

  // --- Task 2: อ่านค่า DHT11 ---
  if (currentMillis - lastDHTRead >= INTERVAL_DHT) {
      lastDHTRead = currentMillis;
      mySensor.readDHTData();
  }

  // --- Task 3: อัปเดตหน้าจอ LCD ---
  if (currentMillis - lastLCDUpdate >= INTERVAL_LCD) {
      lastLCDUpdate = currentMillis;
      mySensor.displayLCD();
  }

  // --- Task 4: รับค่าจาก ESP8266 ---
  comm.receiveCommands();

  // --- Task 5: ตรวจสอบการกดปุ่ม ---
  for (int i = 0; i < 4; i++) {
      if (btnManager.isPressed(i)) {
          // Serial.printf("Button %d Pressed!\n", i + 1);
          btnManager.clearState(i); // ล้างสถานะหลังประมวลผลเสร็จ
          delay(10); 
      }
  }

  // --- Task 6: ส่งข้อมูลไป ESP8266 ---
  if (currentMillis - lastSerialSend >= INTERVAL_SEND) {
      lastSerialSend = currentMillis;
      comm.sendSensorData(mySensor); // โยน Object sensor ให้ Comm จัดการดึงค่าไปส่ง
  }
}