#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// #include <U8g2lib.h>
#include <secret.h>
class Network {
private:
  WiFiClient _espClient;
  PubSubClient _client;
  WiFiUDP _ntp;
  NTPClient* _timeClient; // ใช้ Pointer เพื่อจัดการ Object
  void reconnectMQTT();

public:
  Network();
  void ntp_setup();
  void connectWifi();      
  void connectMQTT();
  void loop_connect_MQTT();
  void Publish_Sensor(float volatge_solar,float voltage_battery,float current, float temp, int power, bool buttonState);
  
  void Callback(char* topic, uint8_t* payload, unsigned int length); // made public
};

#endif 