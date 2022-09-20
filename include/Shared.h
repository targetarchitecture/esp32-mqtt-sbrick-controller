#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <NimBLEDevice.h>

String motorAspeed = "0";
String motorBspeed = "0";
String motorCspeed = "0";
String motorDspeed = "0";

extern void sendMessage(String topic, String payload);
