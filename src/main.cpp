/**
 * ESP32 MQTT controller for the SBrick
 */
#include <Arduino.h>
#include "Shared.h"
#include "MQTTworker.h"
#include "BLEworker.h"

void setup()
{
  Serial.begin(115200);

  delay(10);

  setup_wifi();
  setupMQTT();
  setupBLE();
}

void loop()
{
  loopBLE();
  loopMQTT();

  delay(50); // Delay to keep processor happy?
}
