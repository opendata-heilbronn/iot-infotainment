#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#include "config.h"

#define maxBuffer 80
WiFiClient client;


void setup() {
  // init the pins as input
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(3, INPUT);

  // start a wifi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // set up the ota service
  setupOTA();
}


void sendEmergencyStop(const char* button) {
  WiFiClientSecure client;
  // connectto host
  client.connect(host, httpsPort);

  StaticJsonBuffer<200> jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();

  JsonObject& sensor1 = array.createNestedObject();
  sensor1["sensorName"] = sensorName;
  sensor1["value"]= button;

  String sensorJson = String("POST /sensor HTTP/1.0\r\nHost: " + hostName + "\r\nContent-Type: application/json\r\nConnection: close\r\n");

  int len = array.measureLength();
  sensorJson += "Content-Length: ";
  sensorJson += len;
  sensorJson += "\r\n\r\n";
  array.printTo(sensorJson);
  client.print(sensorJson);
  client.stop();
}


void setupOTA()
{
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);
  // Hostname defaults to MAC address
  ArduinoOTA.setHostname(("Emergency-ESP " + WiFi.macAddress()).c_str());
  // No authentication by default
  //ArduinoOTA.setPassword(OTA_pass);
  ArduinoOTA.begin();
}

// the loop function runs over and over again forever
void loop() {
  ArduinoOTA.handle();

  // if button 3 is low, send the request and wait until the button is low againt
  if (!digitalRead(3)) {
    sendEmergencyStop("emergency");
  }


  // if button 1 is low, send the request and wait until the button is low againt
  if (!digitalRead(1)) {
    sendEmergencyStop("on");
  }


  // if button 0 is low, send the request and wait until the button is low againt
  if (!digitalRead(0)) {
    sendEmergencyStop("off");
  }
  delay(1);
}


