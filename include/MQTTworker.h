#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
  #include<ESPmDNS.h>



void setup_wifi();
void setupMQTT();
void loopMQTT();
void callback(char *topic, byte *payload, unsigned int length);
void sendMessage(String payload);
void reconnect();

extern String motorAspeed;
extern String motorBspeed;
extern String motorCspeed;
extern String motorDspeed;