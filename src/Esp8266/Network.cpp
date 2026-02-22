#include "Network.h"
#include "secret.h"
#include <Arduino.h>

static Network* _net_instance = nullptr;

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  if (_net_instance) _net_instance->Callback(topic, payload, length);
}

Network::Network() : _client(_espClient) {
  _net_instance = this;
  _client.setServer(MQTT_SERVER, MQTT_PORT);
  _client.setCallback(mqttCallback);
  _timeClient = new NTPClient(_ntp, "asia.pool.ntp.org", 7*3600, 86400000*2);   // Init NTP Client (ยังไม่ start)  อัปเดต 2 วันครั้ง
}
void Network::ntp_setup() {
  _timeClient->begin();
  if(!_timeClient->update()){
    _timeClient->forceUpdate();
  }
  Serial.printf("Current Time: %s\n",_timeClient->getFormattedTime());
  }

void Network::connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Connecting WiFi"));

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start > 20000) { // timeout 20s
      break; // timeout 
    } 
  }
  if(WiFi.status() == WL_CONNECTED){
      Serial.print(F("\nWiFi Connected, IP: "));
      Serial.println(WiFi.localIP());
  }
}

void Network::connectMQTT() {
  if (!_client.connected()) {
    reconnectMQTT();
  }
}

void Network::reconnectMQTT() {
  // ใช้ static variable เพื่อจำค่าเวลาครั้งล่าสุดที่ลองเชื่อมต่อ (Non-blocking Timer)
  static unsigned long lastReconnectAttempt = 0;
  unsigned long now = millis();

  // เช็คว่าผ่านไป 5 วินาทีหรือยัง? ถ้ายังไม่ถึง ให้ข้ามไปเลย
  if (now - lastReconnectAttempt > 5000) {
    lastReconnectAttempt = now; // อัปเดตเวลาล่าสุด

    if (!_client.connected()) {
      Serial.print(F("Attempting MQTT connection..."));
      // Create a random client ID
      String clientId = MQTT_CLIENT;
      clientId += String(random(0xffff), HEX);
      
      // ลองเชื่อมต่อ (คำสั่ง connect )
      if (_client.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
        Serial.print(F("connected\n"));
        _client.subscribe("/Command");
        lastReconnectAttempt = 0; // รีเซ็ตเวลาเพื่อให้ครั้งหน้าพร้อมทำงานทันทีถ้าหลุดอีก
      } else {
        Serial.printf("failed, rc= %d\n",_client.state());
        Serial.print(F(" (try again in 5 seconds)\n"));
      }
    }
  }
}

void Network::loop_connect_MQTT() {
  // เช็คและพยายามเชื่อมต่อใหม่แบบ Non-blocking
  if (!_client.connected()) {
    reconnectMQTT();
  }
  _client.loop();
  _timeClient->update();
}

void Network::Publish_Sensor(float volatge_solar,float voltage_battery,float current, float temp, int power, bool buttonState) {

  char payload[256];

  // ดึงเวลาปัจจุบัน (Unix Timestamp)
  unsigned long epochTime = _timeClient->getEpochTime();

  // Send Data Json format
  int len = snprintf(payload, sizeof(payload), 
            "{\"Voltage_solar\":%.2f,\"Voltage_battery\":%.2f,\"Current\":%.2f,\"Temp\":%.2f,\"Power\":%d,\"button_state\":%u,\"time_ms\":%lu}",
             volatge_solar, voltage_battery, current, temp, power, buttonState ? 1 : 0, epochTime);
  
  _client.publish("/MyProject/sensor", payload, (unsigned int)len);
  Serial.printf("Published: %s \n",payload);
}

// Subscribe 
void Network::Callback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.printf("Message arrived [%c]",topic);
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.printf(" %s\n", message);
  
  if (String(topic) == "/MyProject/Command") {
    if (message == "ON" || message == "1") {
      Serial.print(F("Command: ON\n"));
    } else if (message == "OFF" || message == "0") {
      Serial.print(F("Command: OFF\n"));
    }
  }
}
