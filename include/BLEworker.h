#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4dc591b0-857c-41de-b5f1-15abda665b0c");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("02b8cbcc-0e25-4bda-8790-a15f53e6010f");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic[6];
static BLEAdvertisedDevice *myDevice[6];


static std::vector<BLEAdvertisedDevice> *myDevices = {};
static std::vector<BLERemoteCharacteristic> *pRemoteCharacteristics = {};

extern uint8_t motorAdrivingForward;
extern uint8_t motorBdrivingForward;
extern uint8_t motorCdrivingForward;
extern uint8_t motorDdrivingForward;

extern uint8_t motorAspeed;
extern uint8_t motorBspeed;
extern uint8_t motorCspeed;
extern uint8_t motorDspeed;

void loopBLE();
void sendBLE();
void setupBLE();
bool connectToServer();