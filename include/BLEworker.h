#pragma once
#include <Arduino.h>
#include <PubSubClient.h>
#include <NimBLEDevice.h>

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4dc591b0-857c-41de-b5f1-15abda665b0c");

// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("02b8cbcc-0e25-4bda-8790-a15f53e6010f");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

extern String motorAspeed;
extern String motorBspeed;
extern String motorCspeed;
extern String motorDspeed;

void loopBLE();
void sendBLE();
void setupBLE();
bool connectToServer();

extern void sendMessage(String payload);
extern void sendMessage(String topic, String payload);