
#include <Arduino.h>
#include "MQTTworker.h"

WiFiClient espClient;
PubSubClient client(espClient);

const char *ssid = "the robot network";
const char *password = "isaacasimov";
const char *mqtt_server = "robotmqtt";

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

  auto value = atof(message.c_str());

  if (strcmp(topic, "/SBrick/adalovelace/motor/1") == 0)
  {
    motorAspeed = (uint8_t)value;
  }
  else if (strcmp(topic, "/SBrick/adalovelace/motor/2") == 0)
  {
    motorBspeed = (uint8_t)value;
  }
  else if (strcmp(topic, "/SBrick/adalovelace/motor/3") == 0)
  {
    motorCspeed = (uint8_t)value;
  }
  else if (strcmp(topic, "/SBrick/adalovelace/motor/4") == 0)
  {
    motorDspeed = (uint8_t)value;
  }
  else if (strcmp(topic, "/SBrick/adalovelace/stop") == 0)
  {
    motorAspeed = 0;
    motorBspeed = 0;
    motorCspeed = 0;
    motorDspeed = 0;
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
      client.publish("/SBrick/adalovelace/", "hello world");
      // ... and resubscribe
        client.subscribe("/SBrick/#");
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loopMQTT()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // unsigned long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  //   ++value;
  //   snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
  //   Serial.print("Publish message: ");
  //   Serial.println(msg);
  //   client.publish("outTopic", msg);
  // }
}
