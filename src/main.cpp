#define VERSION "1"
#include "SarahHome.h"
#define HEATING_PIN 2
#define WATER_PIN  0
SarahHome sarahHome("hvac");

const char* mqttControlTopicFormat = "hvac/%s/%s/control/set";
char mqttHeatingControlTopic[100];
char mqttWaterControlTopic[100];
const char* mqttStatusTopicFormat = "hvac/%s/%s/status";

//Set heating on/off
void setHeating(boolean isOn) {
  digitalWrite(HEATING_PIN, !isOn);
}

//Set water on/off
void setWater(boolean isOn) {
  digitalWrite(WATER_PIN, !isOn);
}

//Callback is called on MQTT messages
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  if (length != 1) {
    return;
  }
  boolean on = (char)payload[0] == '1' ? true : false;
  boolean isHeating = String(topic).equals(mqttHeatingControlTopic);
  boolean isWater = String(topic).equals(mqttWaterControlTopic);

  if (isHeating) {
    setHeating(on);
  } else if (isWater) {
    setWater(on);
  }
}

void setup()
{
  Serial.begin(9600);
  //Initialise relay pins and set them off
  pinMode(HEATING_PIN, OUTPUT);
  pinMode(WATER_PIN, OUTPUT);
  setHeating(false);
  setWater(false);

  sarahHome.setup(VERSION);

  sprintf(mqttHeatingControlTopic, mqttControlTopicFormat, sarahHome.getNodeId().c_str(), "heating");
  sprintf(mqttWaterControlTopic, mqttControlTopicFormat, sarahHome.getNodeId().c_str(), "water");
  sarahHome.mqttClient.subscribe(mqttHeatingControlTopic);
  sarahHome.mqttClient.subscribe(mqttWaterControlTopic);
  sarahHome.mqttClient.setCallback(mqttCallback);
}

void loop() {
  sarahHome.loop();
}
