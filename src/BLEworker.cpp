#include <Arduino.h>
#include "BLEworker.h"

void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char *)pData);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
    sendMessage("BLE Connected");
  }

  void onDisconnect(BLEClient *pclient)
  {
    // connected = false;
    Serial.println("onDisconnect");

    sendMessage("BLE Disconnected");
  }
};

bool connectToServer()
{
  String msg = "Connecting to ";
  msg.concat(myDevice->getAddress().toString().c_str());
  sendMessage(msg);

  //Serial.print("Forming a connection to ");
  //Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  //Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);
  //Serial.println(" - Connected to server");

  msg = "Connected to ";
  msg.concat(myDevice->getAddress().toString().c_str());
  sendMessage(msg);

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    msg = "Failed to find our service UUID: ";
    msg.concat(serviceUUID.toString().c_str());
    sendMessage(msg);

    // Serial.print("Failed to find our service UUID: ");
    // Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  // Serial.println(" - Found our service");

  sendMessage("Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    msg = "Failed to find our characteristic UUID: ";
    msg.concat(charUUID.toString().c_str());
    sendMessage(msg);

    // Serial.print("Failed to find our characteristic UUID: ");
    // Serial.println(charUUID.toString().c_str());

    pClient->disconnect();
    return false;
  }
  // Serial.println(" - Found our characteristic");

  sendMessage("Found our characteristic");

  // Read the value of the characteristic.
  // if (pRemoteCharacteristic->canRead())
  // {
  //   std::string value = pRemoteCharacteristic->readValue();
  //   Serial.print("The characteristic value was: ");
  //   Serial.println(value.c_str());
  // }

  if (pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->subscribe(true, notifyCallback);

  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice *advertisedDevice)
  {
    // Serial.print("BLE Advertised Device found: ");
    // Serial.println(advertisedDevice->toString().c_str());

    String msg = "BLE Advertised Device found: ";
    msg.concat(advertisedDevice->toString().c_str());
    sendMessage(msg);

    if (strcmp(advertisedDevice->getName().c_str(), "BBC micro:bit [tegat]") == 0)
    {
      BLEDevice::getScan()->stop();

      myDevice = advertisedDevice;
      doConnect = true;
      doScan = true;

      sendMessage("Found BBC micro:bit [tegat]");
    } // Found our server

  } // onResult
};  // MyAdvertisedDeviceCallbacks

void setupBLE()
{
  sendMessage("Starting BLE...");

  // Serial.println("Starting BLE...");

  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void sendBLE()
{
  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected)
  {
    // Construct drive command.
    uint8_t byteCmd[13] = {
        1,
        1,
        true,
        motorAspeed,
        2,
        true,
        motorBspeed,
        3,
        true,
        motorCspeed,
        4,
        true,
        motorDspeed};

    // Send drive command.;
    pRemoteCharacteristic->writeValue(byteCmd, sizeof(byteCmd), false);
  }
}

void loopBLE()
{
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true)
  {
    if (connectToServer())
    {
      // Serial.println("We are now connected to the BLE Server.");

      sendMessage("We are now connected to the BLE Server.");
    }
    else
    {
      // Serial.println("We have failed to connect to the server; there is nothin more we will do.");

      sendMessage("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // will this work
  if (connected == false)
  {
    // Serial.println("Restart scan - loopBLE");
    BLEDevice::getScan()->start(0);

    sendMessage("Restart scan - loopBLE");
  }
  else
  {
    sendBLE();
  }
} // End of loop
