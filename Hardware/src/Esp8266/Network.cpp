#include "esp/Network.h"

// กำหนด Timezone สำหรับกรุงเทพฯ (GMT+7) = 7 * 60 * 60 = 25200 วินาที
const long utcOffsetInSeconds = 25200; 

NetworkManager::NetworkManager(const char* ssid, const char* password, 
                               const char* mqttServer, int mqttPort, 
                               const char* mqttUser, const char* mqttPass,
                               const char* mqttTopic)
    : _ssid(ssid), _password(password), 
      _mqttServer(mqttServer), _mqttPort(mqttPort),
      _mqttUser(mqttUser), _mqttPass(mqttPass), _mqttTopic(mqttTopic),
      _mqttClient(_wifiClient), 
      _timeClient(_ntpUDP, "pool.ntp.org", utcOffsetInSeconds, 86400000) // อัปเดตเวลาทุก 1 วัน
{
}

void NetworkManager::begin() {
    connectWiFi();
    
    _mqttClient.setServer(_mqttServer, _mqttPort);
    connectMQTT();

    _timeClient.begin();
}

void NetworkManager::connectWiFi() {
    Serial.print(F("Connecting to WiFi: "));
    Serial.println(_ssid);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println(F("\nWiFi Connected!"));
    Serial.print(F("IP Address: "));
    Serial.println(WiFi.localIP());
}

void NetworkManager::connectMQTT() {
    // พยายามเชื่อมต่อจนกว่าจะสำเร็จ
    while (!_mqttClient.connected()) {
        Serial.print(F("Connecting to MQTT... "));
        
        // สร้าง Client ID แบบสุ่ม
        String clientId = "SmartSolar-ESP8266-" + String(random(0xffff), HEX);
        
        if (_mqttClient.connect(clientId.c_str(), _mqttUser, _mqttPass)) {
            Serial.println(F("Connected!"));
        } else {
            Serial.print(F("Failed, rc="));
            Serial.print(_mqttClient.state());
            Serial.println(F(" Try again in 5 seconds"));
            delay(5000);
        }
    }
}

void NetworkManager::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    
    if (!_mqttClient.connected()) {
        connectMQTT();
    }
    
    _mqttClient.loop();
    _timeClient.update(); // อัปเดตเวลาจาก NTP
}

unsigned long NetworkManager::getCurrentTime() {
    return _timeClient.getEpochTime();
}

bool NetworkManager::publishData(JsonDocument& sensorData) {
    if (!_mqttClient.connected()) return false;

    // แทรกเวลาปัจจุบันเข้าไปใน JSON Payload
    sensorData["timestamp"] = getCurrentTime();

    // แปลง JSON เป็น String
    String payload;
    serializeJson(sensorData, payload);

    // ส่งขึ้น MQTT
    bool success = _mqttClient.publish(_mqttTopic, payload.c_str());
    
    if (success) {
        Serial.println(F("Data published successfully."));
        // Serial.println(payload); // เปิดคอมเมนต์นี้ถ้าอยากดูข้อมูลที่ส่งไป
    } else {
        Serial.println(F("Failed to publish data."));
    }

    return success;
}