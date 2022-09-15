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
  }

  void onDisconnect(BLEClient *pclient)
  {
    // connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice[1]->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice); 
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic[1] = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pRemoteCharacteristic[1]->canRead())
  {
    std::string value = pRemoteCharacteristic[1]->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic[1]->canNotify())
    pRemoteCharacteristic[1]->subscribe(true,notifyCallback);

  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  /**
   * Called for each advertising BLE server.
   */

  /*** Only a reference to the advertised device is passed now
  void onResult(BLEAdvertisedDevice advertisedDevice) { **/
  void onResult(BLEAdvertisedDevice *advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice->toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    /********************************************************************************
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
********************************************************************************/
    // if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(serviceUUID)) {

    //  auto n = advertisedDevice->getName();

//     if (strcmp(advertisedDevice->getName().c_str(), "adalovelace") == 0)
//     //BBC micro:bit [tegat]
//     {
//       BLEDevice::getScan()->stop();
//       /*******************************************************************
//       myDevice = new BLEAdvertisedDevice(advertisedDevice);
// *******************************************************************/
//       myDevice[1] = advertisedDevice; /** Just save the reference now, no need to copy the object */
//       doConnect = true;
//       doScan = true;

//     } // Found our server

    if (strcmp(advertisedDevice->getName().c_str(), "BBC micro:bit [tegat]") == 0)
    {
      BLEDevice::getScan()->stop();
      /*******************************************************************
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
*******************************************************************/
      myDevice[2] = advertisedDevice; /** Just save the reference now, no need to copy the object */
      doConnect = true;
      doScan = true;

    } // Found our server


  }   // onResult
};    // MyAdvertisedDeviceCallbacks

void setupBLE()
{
  Serial.println("Starting BLE...");
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

    // Command sent 1,1,1,0,2,1,0,3,1,0,4,1,0,

    // Serial.print("motorDspeed: ");
    // Serial.println(motorDspeed);

    // Serial.print("Command sent");
    // for (int i = 0; i < 13; i++)
    // {
    //   Serial.print(byteCmd[i]);
    //   Serial.print(",");
    // }
    // Serial.println();

    // Send drive command.;
    pRemoteCharacteristic[1]->writeValue(byteCmd, sizeof(byteCmd), false);
  }
  // else
  // {
  //   Serial.println("Restart scan");
  //   BLEDevice::getScan()->start(0);
  // }
  // } else if (doScan) {
  //   BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  // }
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
      Serial.println("We are now connected to the BLE Server.");
    }
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // will this work
  if (connected == false)
  {
    Serial.println("Restart scan - loopBLE");
    BLEDevice::getScan()->start(0);
  }
  else
  {
    sendBLE();
  }

} // End of loop
