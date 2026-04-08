#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "uno/sensor.h"
#include "uno/Send_command.h"

// สร้าง Object sensor
Sensor mySensor(dhtPin,DHT11, Voltage_Pin_solar, Voltage_Pin_Battery,Current_Pin_In,Current_Pin_Out);

CommManager comm(3, 1); // RX=3, TX=1

// 2. กำหนดช่วงเวลาทำงาน
const unsigned long INTERVAL_ANALOG = 50;    
const unsigned long INTERVAL_DHT = 2000;     
const unsigned long INTERVAL_SEND = 3000;    

// ตัวแปรเก็บเวลา
unsigned long lastAnalogRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSerialSend = 0;


void setup() {
  Serial.begin(115200);
  
  mySensor.begin();
//btnManager.begin();
  comm.begin(9600);
  
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

//   // --- Task 3: อัปเดตหน้าจอ LCD ---
//   if (currentMillis - lastLCDUpdate >= INTERVAL_LCD) {
//       lastLCDUpdate = currentMillis;
//       mySensor.displayLCD();
//   }

  // --- Task 4: รับค่าจาก ESP8266 ---
  comm.receiveCommands();

//   // --- Task 5: ตรวจสอบการกดปุ่ม ---
//   for (int i = 0; i < 4; i++) {
//       if (btnManager.isPressed(i)) {
//           // Serial.printf("Button %d Pressed!\n", i + 1);
//           btnManager.clearState(i); // ล้างสถานะหลังประมวลผลเสร็จ
//           delay(10); 
//       }
//   }

  // --- Task 5: ส่งข้อมูลไป ESP8266 ---
  if (currentMillis - lastSerialSend >= INTERVAL_SEND) {
      lastSerialSend = currentMillis;
      comm.sendSensorData(mySensor); // ส่ง Object sensor ให้ Comm จัดการดึงค่าไปส่ง
  }
}