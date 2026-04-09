#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "esp/secret.h"

class NetworkManager {
private:
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    WiFiUDP _ntpUDP;
    NTPClient _timeClient;

    const char* _ssid;
    const char* _password;
    const char* _mqttServer;
    int _mqttPort;
    const char* _mqttUser;
    const char* _mqttPass;
    const char* _mqttTopic;

    void connectWiFi();
    void connectMQTT();

public:
    // Constructor รับค่า Configuration
    NetworkManager(const char* ssid, const char* password, 
                   const char* mqttServer, int mqttPort, 
                   const char* mqttUser, const char* mqttPass, 
                   const char* mqttTopic);

    void begin();
    void loop(); // คอยเรียกในลูปเพื่อรักษาการเชื่อมต่อและอัปเดตเวลา

    // ส่งข้อมูล JSON ขึ้น MQTT
    bool publishData(JsonDocument& sensorData);
    
    // ดึงเวลาปัจจุบัน (Epoch Time)
    unsigned long getCurrentTime();
};

#endif