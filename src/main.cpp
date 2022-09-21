/**
 * ESP32 MQTT controller for the SBrick
 */
#include <Arduino.h>
#include "Shared.h"
#include "MQTTworker.h"
#include "BLEworker.h"

#ifdef __cplusplus
extern "C"
{
#endif

  uint8_t temprature_sens_read();

#ifdef __cplusplus
}
#endif

void setup()
{
  Serial.begin(115200);

  delay(10);

  setupWiFi();
  setupMQTT();
  setupBLE();
}

void loop()
{
  loopBLE();
  loopMQTT();
  loopStats();

  delay(50); // Delay to keep processor happy?
}

void loopStats()
{
  if (millis() - time_now > 10000)
  {
    time_now = millis();

    auto temp_farenheit = temprature_sens_read();
    int temp_celsius = trunc((temp_farenheit - 32) / 1.8);

    sendMessage("/sbrick/adalovelace/stats/millis", std::to_string(time_now).c_str());
    sendMessage("/sbrick/adalovelace/stats/freeheap", std::to_string(ESP.getFreeHeap()).c_str());
    sendMessage("/sbrick/adalovelace/stats/temp", std::to_string(temp_celsius).c_str());
    sendMessage("/sbrick/adalovelace/stats/ip", WiFi.localIP().toString().c_str());
  }
}