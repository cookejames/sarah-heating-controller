#define VERSION "2.6"
#include "SarahHome.h"
#define HEATING_PIN 4
#define WATER_PIN  5
SarahHome sarahHome("hvac");

const char* mqttControlTopicFormat = "hvac/%s/%s/control/set";
char mqttHeatingControlTopic[100];
char mqttWaterControlTopic[100];
const char* mqttStatusTopicFormat = "hvac/%s/%s/status";
char mqttHeatingStatusTopic[100];
char mqttWaterStatusTopic[100];

//Set heating on/off
void setHeating(boolean isOn) {
  Serial.printf("Setting heating: %s\n", isOn ? "on" : "off");
  digitalWrite(HEATING_PIN, !isOn);
  sarahHome.mqttClient.publish(mqttHeatingStatusTopic, isOn ? "on" : "off", true);
}

//Set water on/off
void setWater(boolean isOn) {
  Serial.printf("Setting water: %s\n", isOn ? "on" : "off");
  digitalWrite(WATER_PIN, !isOn);
  sarahHome.mqttClient.publish(mqttWaterStatusTopic, isOn ? "on" : "off", true);
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
  }
  if (isWater) {
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

  //Setup topics and subscriptions
  sprintf(mqttHeatingControlTopic, mqttControlTopicFormat, sarahHome.getNodeId().c_str(), "heating");
  sprintf(mqttWaterControlTopic, mqttControlTopicFormat, sarahHome.getNodeId().c_str(), "water");
  sprintf(mqttHeatingStatusTopic, mqttStatusTopicFormat, sarahHome.getNodeId().c_str(), "heating");
  sprintf(mqttWaterStatusTopic, mqttStatusTopicFormat, sarahHome.getNodeId().c_str(), "water");
  sarahHome.subscribe(mqttHeatingControlTopic);
  sarahHome.subscribe(mqttWaterControlTopic);
  sarahHome.mqttClient.setCallback(mqttCallback);
}

void loop() {
  sarahHome.loop();
}
