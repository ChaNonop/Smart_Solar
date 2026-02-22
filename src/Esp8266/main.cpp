#include <Arduino.h>
#include <ESP8266Wifi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "esp/Network.h"
#include "esp/secret.h"
#include "esp/sd_card.h"

Network net;
SDCard sd;

void setup() {
    Serial.begin(115200)
}

void loop() {

}
