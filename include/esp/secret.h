// #ifndef SECRET_H
// #define SECRET_H

// // WiFi / MQTT configuration 
// #define WIFI_SSID    "Somwang"
// #define WIFI_PASSWORD "03143681"

// #define MQTT_SERVER  "broker.hivemq.com"
// #define MQTT_PORT    1883
// #define MQTT_CLIENT  "ESP32_My"
// #define MQTT_USER    ""
// #define MQTT_PASS    ""

// #endif 

// #ifndef SECRET_H
// #define SECRET_H

// ==========================================
// 1. ตั้งค่า WiFi บ้านของคุณ (แก้ตรงนี้)
// ==========================================
#define WIFI_SSID     "Somwang"     // ชื่อ WiFi 
#define WIFI_PASSWORD "03143681" // รหัสผ่าน WiFi

// ==========================================
// 2. ตั้งค่า HiveMQ Public Broker (ฟรี)
// ==========================================
#define MQTT_SERVER   "broker.hivemq.com"
#define MQTT_PORT     1883
#define MQTT_USER     "" 
#define MQTT_PASS     "" 

// ==========================================
// 3. ตั้งค่า Topic (ต้องให้ตรงกับ Node-RED)
// ==========================================
// แนะนำให้ใช้ชื่อเฉพาะตัว เพื่อไม่ให้ข้อมูลไปปนกับคนอื่นบนโลกที่ใช้ HiveMQ
// เช่น เปลี่ยน chanon เป็นชื่อคุณ
#define MQTT_TOPIC    "/MyProject/sensor" 

#endif
