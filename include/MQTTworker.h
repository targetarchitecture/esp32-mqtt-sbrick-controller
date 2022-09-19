#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

void setup_wifi();
void setupMQTT();
void loopMQTT();
void callback(char *topic, byte *payload, unsigned int length);
void sendMessage(String payload);
void reconnect();

extern uint8_t motorAdrivingForward;
extern uint8_t motorBdrivingForward;
extern uint8_t motorCdrivingForward;
extern uint8_t motorDdrivingForward;

extern uint8_t motorAspeed;
extern uint8_t motorBspeed;
extern uint8_t motorCspeed;
extern uint8_t motorDspeed;