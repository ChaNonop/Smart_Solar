#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include "uno/sensor.h"
#include "uno/Send_command.h"
#include "uno/display.h"

// สร้าง Object sensor
Sensor mySensor(dhtPin,DHT11,Voltage_Pin_solar,Voltage_Pin_Battery,Current_Pin_In,Current_Pin_Out);

CommManager comm(2, 3); // RX = 2, TX = 3
DisplayManager oled; // สร้าง Object จอ OLED

// 2. กำหนดช่วงเวลาทำงาน
const unsigned long INTERVAL_ANALOG = 100;    
const unsigned long INTERVAL_DHT = 2000;     
const unsigned long INTERVAL_SEND = 3000; 
const unsigned long INTERVAL_OLED = 1000; // อัปเดตจอทุกๆ 1 วินาที   

// ตัวแปรเก็บเวลา
unsigned long lastAnalogRead = 0;
unsigned long lastDHTRead = 0;
unsigned long lastSerialSend = 0;
unsigned long lastOledUpdate = 0;


void setup() {
  Serial.begin(115200);
  mySensor.begin();
  oled.begin(); // เริ่มทำงานจอ (จะแสกนหาเอง)
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

    // --- Task 4: อัปเดตจอ ---
  if (currentMillis - lastOledUpdate >= INTERVAL_OLED) {
      lastOledUpdate = currentMillis;
      oled.update(mySensor); // โยน object sensor ไปให้จอจัดการวาดค่า
    
    Serial.println("------------------------------------");
    Serial.print("Lux = "); Serial.println(mySensor.getLux()); 
    Serial.print("Temp = "); Serial.println(mySensor.getTemp());
    Serial.print("Humid = "); Serial.println(mySensor.getHumid());
    Serial.print("V_solar = "); Serial.println(mySensor.getVSolar());
    Serial.print("V_battery = "); Serial.println(mySensor.getVBattery());
    Serial.print("I_In = "); Serial.println(mySensor.getCurrentIn());
    Serial.print("I_Out = "); Serial.println(mySensor.getCurrentOut());
    Serial.print("Power_In = "); Serial.println(mySensor.getPowerIn());
    Serial.print("Power_Out = "); Serial.println(mySensor.getPowerOut());
    Serial.println("------------------------------------");
  }

  // --- Task 4: ส่งข้อมูลไป ESP8266 ---
  if (currentMillis - lastSerialSend >= INTERVAL_SEND) {
      lastSerialSend = currentMillis;
      comm.sendSensorData(mySensor); // ส่ง Object sensor ให้ Comm จัดการดึงค่าไปส่ง
  }

}