#include <Arduino.h>
#include "BLEworker.h"

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient *pclient)
  {
    sendMessage("BLE Connected");
  }

  void onDisconnect(BLEClient *pclient)
  {
    connected = false;

    sendMessage("BLE Disconnected");
  }
};

bool connectToServer()
{
  String msg = "Connecting to ";
  msg.concat(myDevice->getAddress().toString().c_str());
  sendMessage(msg);

  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);

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

    pClient->disconnect();
    return false;
  }

  sendMessage("Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristicCommand = pRemoteService->getCharacteristic(CommandUUID);

  if (pRemoteCharacteristicCommand == nullptr)
  {
    msg = "Failed to find our command characteristic UUID: ";
    msg.concat(CommandUUID.toString().c_str());
    sendMessage(msg);

    pClient->disconnect();
    return false;
  }
  else
  {
    Serial.println("Send voltage command.");

    std::vector<uint8_t> vec;
    vec.push_back(0x2C);
    vec.push_back(0x00);
    vec.push_back(0x01);
    vec.push_back(0x02);
    vec.push_back(0x03);
    vec.push_back(0x04);
    vec.push_back(0x05);
    vec.push_back(0x06);
    vec.push_back(0x07);

    if (pRemoteCharacteristicCommand->writeValue(vec, false) == false)
    {
      Serial.println("Send voltage command failed.");
    }

    Serial.println("Send voltage notification command.");

    vec.clear();
    vec.push_back(0x2E);
    vec.push_back(0x00);
    vec.push_back(0x01);
    vec.push_back(0x02);
    vec.push_back(0x03);
    vec.push_back(0x04);
    vec.push_back(0x05);
    vec.push_back(0x06);
    vec.push_back(0x07);

    if (pRemoteCharacteristicCommand->writeValue(vec, false) == false)
    {
      Serial.println("Send voltage notification command failed.");
    }
  }

  sendMessage("Found our notification characteristic");

  if (pRemoteCharacteristicCommand->canNotify())
  {
    Serial.println("subscribing");
    if (pRemoteCharacteristicCommand->subscribe(true, NotifyCallback) == false)
    {
      Serial.println("subscribe failed");
    }
    else
    {
      Serial.println("subscribed");
    }
  }

  connected = true;
  return true;
}

void NotifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify)
{
Serial.print("NimBLERemoteCharacteristic ");

String message = "";
for (unsigned int i = 0; i < length; i++)
{
  message = message + (char)pData[i];
}

Serial.println(message.c_str());
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice *advertisedDevice)
  {
    // String msg = "BLE Advertised Device found: ";
    // msg.concat(advertisedDevice->toString().c_str());
    // sendMessage(msg);

    if (strcmp(advertisedDevice->getName().c_str(), "adalovelace") == 0)
    {
      BLEDevice::getScan()->stop();

      myDevice = advertisedDevice;
      doConnect = true;
      doScan = true;

      sendMessage("Found adalovelace");
    } // Found our server

  } // onResult
};  // MyAdvertisedDeviceCallbacks

void setupBLE()
{
  sendMessage("Starting BLE...");

  BLEDevice::init("ESP32 SBrick Controller");

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
        1,                                      // 0
        0,                                      // 1
        (uint8_t) true,                         // 2
        (uint8_t)std::abs(motorAspeed.toInt()), // 3
        1,                                      // 4
        (uint8_t) true,                         // 5
        (uint8_t)std::abs(motorBspeed.toInt()), // 6
        2,                                      // 7
        (uint8_t) true,                         // 8
        (uint8_t)std::abs(motorCspeed.toInt()), // 9
        3,                                      // 10
        (uint8_t) true,                         // 11
        (uint8_t)std::abs(motorDspeed.toInt())  // 12
    };

    if (motorAspeed.toInt() < 0)
    {
      byteCmd[2] = (uint8_t) false;
    }
    if (motorBspeed.toInt() < 0)
    {
      byteCmd[5] = (uint8_t) false;
    }
    if (motorCspeed.toInt() < 0)
    {
      byteCmd[8] = (uint8_t) false;
    }
    if (motorDspeed.toInt() < 0)
    {
      byteCmd[11] = (uint8_t) false;
    }

    // Send drive command.;
    if (pRemoteCharacteristicCommand->writeValue(byteCmd, sizeof(byteCmd), false) == false)
    {
      Serial.println("Send drive command failed.");
    }

    // String msg = "";
    // for (size_t i = 0; i < 13; i++)
    // {
    //   msg = msg + std::to_string(byteCmd[i]).c_str();
    //   if (i < 12)
    //   {
    //     msg = msg + ".";
    //   }
    // }
    // if (strcmp(msg.c_str(), lastCommand.c_str()) != 0)
    // {
    //   sendMessage("/sbrick/adalovelace/command", msg);
    // }
    // lastCommand = msg.c_str();
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
      sendMessage("Connected to SBrick");
    }
    else
    {
      sendMessage("Failed to connect SBrick");
    }
    doConnect = false;
  }

  // will this work
  if (connected == false)
  {
    BLEDevice::getScan()->start(0);

    sendMessage("Restart scan - loopBLE");
  }
  else
  {
    sendBLE();
  }
} // End of loop
