#include <Arduino.h>
#include <ArduinoJson.h>

#include "esp/secret.h" 
#include "esp/Network.h"
#include "esp/Sensor_reciver.h"

// กำหนดขา SoftwareSerial (RX, TX) ให้ตรงกับที่ต่อสายจาก Nano
// แนะนำให้ใช้ขา D5 (RX) และ D6 (TX) สำหรับ ESP8266
SensorReceiver nanoSerial(TX, RX);

// สร้าง Object จัดการ Network
// ส่งค่า Configuration จากไฟล์ secret.h เข้าไป
NetworkManager network(WIFI_SSID, WIFI_PASSWORD, 
                       MQTT_SERVER, MQTT_PORT, 
                       MQTT_USER, MQTT_PASS, 
                       "smart_solar/data"); // หัวข้อ MQTT ที่ต้องการส่งไป

void setup() {
  Serial.begin(115200);
  Serial.println(F("\n--- Starting Smart Solar Gateway ---"));

  // เริ่มการทำงานของส่วนรับข้อมูล (ตั้ง Baudrate ให้ตรงกับฝั่ง Nano คือ 9600)
  nanoSerial.begin(9600);

  network.begin();
}

void loop() {
  network.loop();

  JsonDocument sensorData;

  // 3. ตรวจสอบว่ามีข้อมูลส่งมาจาก Nano หรือไม่
  if (nanoSerial.receiveData(sensorData)) {
      // 4. ถ้ามีข้อมูลถูกต้อง ให้ทำการ Publish ขึ้น MQTT ทันที
      network.publishData(sensorData);
  }
}