/**
   A BLE client example that is rich in capabilities.
   There is a lot new capabilities implemented.
   author unknown
   updated by chegewara
*/

#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4dc591b0-857c-41de-b5f1-15abda665b0c");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("02b8cbcc-0e25-4bda-8790-a15f53e6010f");

enum BLEHubChannel {
  A,
  B,
  C,
  D
};

// static boolean doConnect = false;
// static boolean connected = false;
// static boolean doScan = false;
// BLEClient* pClient = BLEDevice::createClient();
 BLERemoteCharacteristic* pRemoteCharacteristic;
//static BLEAdvertisedDevice* myDevice;


 //BLERemoteService* pRemoteService;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("onConnect");
  }

  void onDisconnect(BLEClient* pclient) {
    // connected = false;
    // doConnect = true;
    Serial.println("onDisconnect");
  }
};

bool connect() {

  BLEClient* pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(BLEAddress("bc:33:ac:ee:1d:81"));

  delay(10);

  // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService  = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");
}

void sendMessages() {

    Serial.println("sendMessages");

  uint8_t Arnd = random(0, 255);
  uint8_t Brnd = random(0, 255);
  uint8_t Crnd = random(0, 255);
  uint8_t Drnd = random(0, 255);

  // Construct drive command.
  uint8_t byteCmd[13] = {
    1,
    BLEHubChannel::A,
    true,
    Arnd,
    BLEHubChannel::B,
    true,
    Brnd,
    BLEHubChannel::C,
    true,
    Crnd,
    BLEHubChannel::D,
    true,
    Drnd
  };

  // Send drive command.;
  pRemoteCharacteristic->writeValue(byteCmd, sizeof(byteCmd), false);

  //delay(5000);

  // Serial.println("Disconnect");
  // pClient->disconnect();

 // return true;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("SBRICK Controller");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  // BLEScan* pBLEScan = BLEDevice::getScan();
  // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  // pBLEScan->setInterval(1349);
  // pBLEScan->setWindow(449);
  // pBLEScan->setActiveScan(true);
  // pBLEScan->start(5, false);

  //test();

  //connect();

}  // End of setup.

//const int8_t CMD_DRIVE = 1;



// This is the Arduino main loop function.
void loop() {

  connect();

  sendMessages();

  delay(5000);  // Delay a second between loops.
}  // End of loop