#include <Arduino.h>
#include "MQTTworker.h"

const char *ssid = "the robot network";
const char *password = "isaacasimov";
const char *mqtt_server = "robotmqtt";

WiFiClient espClient;
PubSubClient client(espClient);

void setupWiFi()
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mdns_init(); // Add
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String message = "";
  for (unsigned int i = 0; i < length; i++)
  {
    message = message + (char)payload[i];
  }

  Serial.println(message.c_str());

  // auto value = atof(message.c_str());

  if (strcmp(topic, "lego/sbrick/adalovelace/a/motor") == 0)
  {
    motorAspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/b/motor") == 0)
  {
    motorBspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/c/motor") == 0)
  {
    motorCspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/d/motor") == 0)
  {
    motorDspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/1/motor") == 0)
  {
    motorAspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/2/motor") == 0)
  {
    motorBspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/3/motor") == 0)
  {
    motorCspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/4/motor") == 0)
  {
    motorDspeed = message.c_str();
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/all/stop") == 0)
  {
    motorAspeed = "0";
    motorBspeed = "0";
    motorCspeed = "0";
    motorDspeed = "0";
  }
  else if (strcmp(topic, "lego/sbrick/adalovelace/all/motor") == 0)
  {
    motorAspeed = message.c_str();
    motorBspeed = message.c_str();
    motorCspeed = message.c_str();
    motorDspeed = message.c_str();
  }

  // Serial.print("motorAspeed:");
  // Serial.println(motorAspeed);

  // Serial.print("motorBspeed:");
  // Serial.println(motorBspeed);

  // Serial.print("motorCspeed:");
  // Serial.println(motorCspeed);

  // Serial.print("motorDspeed:");
  // Serial.println(motorDspeed);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "SBrickController-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("lego/sbrick/adalovelace/", "connected");
      // ... and resubscribe
      client.subscribe("lego/sbrick/adalovelace/a/#");
      client.subscribe("lego/sbrick/adalovelace/b/#");
      client.subscribe("lego/sbrick/adalovelace/c/#");
      client.subscribe("lego/sbrick/adalovelace/d/#");
      client.subscribe("lego/sbrick/adalovelace/1/#");
      client.subscribe("lego/sbrick/adalovelace/2/#");
      client.subscribe("lego/sbrick/adalovelace/3/#");
      client.subscribe("lego/sbrick/adalovelace/4/#");
      client.subscribe("lego/sbrick/adalovelace/all/#");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setupMQTT()
{
  IPAddress ipaddr = MDNS.queryHost(mqtt_server, 5000 /* ms */);

  client.setServer(ipaddr, 1883);
  client.setCallback(callback);
}

void loopMQTT()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

void sendMessage(String payload)
{
  Serial.println(payload.c_str());

  if (client.connected())
  {
    client.publish("lego/sbrick/adalovelace/", payload.c_str());
  }
}

void sendMessage(String topic, String payload)
{
  Serial.println(payload.c_str());

  if (client.connected())
  {
    client.publish(topic.c_str(), payload.c_str());
  }
}