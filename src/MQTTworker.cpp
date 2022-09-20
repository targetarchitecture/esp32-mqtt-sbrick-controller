#include <Arduino.h>
#include "MQTTworker.h"

const char *ssid = "the robot network";
const char *password = "isaacasimov";
const char *mqtt_server = "robotmqtt";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
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

  if (strcmp(topic, "/sbrick/adalovelace/motor/a") == 0)
  {
    motorAspeed = message.c_str();
  }
  else if (strcmp(topic, "/SBrick/adalovelace/motor/b") == 0)
  {
    motorBspeed = message.c_str();
  }
  else if (strcmp(topic, "/sbrick/adalovelace/motor/c") == 0)
  {
    motorCspeed = message.c_str();
  }
  else if (strcmp(topic, "/sbrick/adalovelace/motor/d") == 0)
  {
    motorDspeed = message.c_str();
  }
  if (strcmp(topic, "/sbrick/adalovelace/motor/1") == 0)
  {
    motorAspeed = message.c_str();
  }
  else if (strcmp(topic, "/SBrick/adalovelace/motor/2") == 0)
  {
    motorBspeed = message.c_str();
  }
  else if (strcmp(topic, "/sbrick/adalovelace/motor/3") == 0)
  {
    motorCspeed = message.c_str();
  }
  else if (strcmp(topic, "/sbrick/adalovelace/motor/4") == 0)
  {
    motorDspeed = message.c_str();
  }
  else if (strcmp(topic, "/sbrick/adalovelace/motor/stop") == 0)
  {
    motorAspeed = "0";
    motorBspeed = "0";
    motorCspeed = "0";
    motorDspeed = "0";
  }
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
      client.publish("/sbrick/adalovelace/", "connected");
      // ... and resubscribe
      client.subscribe("/sbrick/adalovelace/motor/#");
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
    client.publish("/sbrick/adalovelace/", payload.c_str());
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