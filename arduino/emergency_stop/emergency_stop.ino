#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#include "config.h"

#define maxBuffer 80
WiFiClient client;

bool button0 = false;
bool button1 = false;
bool button3 = false;

void setup() {

  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(3, INPUT);

  if (digitalRead(3) == HIGH) {
    button3 = true;
  }
  if (digitalRead(0) == HIGH) {
    button0 = true;
  }
  if (digitalRead(1) == HIGH) {
    button1 = true;
  }
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  setupOTA();
}

void sendEmergencyStop(const char* SensorName) {
  WiFiClientSecure client;
  if (!client.connect(host, httpsPort)) {
    return;
  }

  StaticJsonBuffer<200> jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();

  JsonObject& sensor1 = array.createNestedObject();
  sensor1["sensorName"] = SensorName;

  String sensorJson = String("POST /sensor HTTP/1.0\r\nHost: " + hostName + "\r\nContent-Type: application/json\r\nConnection: close\r\n");

  int len = array.measureLength();
  sensorJson += "Content-Length: ";
  sensorJson += len;
  sensorJson += "\r\n\r\n";
  array.printTo(sensorJson);
  client.print(sensorJson);
  client.stop();
}

void readAndSendLocalEmergencyStop() {

  if (button3) {
    sendEmergencyStop("emergency");
  }
  if (button0) {
    sendEmergencyStop("off");
  }
  if (button1) {
    sendEmergencyStop("on");
  }
  button3 = false;
  button0 = false;
  button1 = false;
  delay(10);
}

unsigned long lastLocalSensorTime = 0;

bool otaInProgress = false;

void setupOTA()
{
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);
  // Hostname defaults to MAC address
  ArduinoOTA.setHostname(("Emergency-ESP " + WiFi.macAddress()).c_str());
  // No authentication by default
  //ArduinoOTA.setPassword(OTA_pass);
  ArduinoOTA.onStart([]() {
    otaInProgress = true;
  });
  ArduinoOTA.onEnd([]() {
    otaInProgress = false;
  });
  ArduinoOTA.onError([](ota_error_t error) {
    otaInProgress = false;
  });
  ArduinoOTA.begin();
}

// the loop function runs over and over again forever
void loop() {
  if (digitalRead(3) == HIGH) {
    button3 = true;
  }
  if (digitalRead(0) == HIGH) {
    button0 = true;
  }
  if (digitalRead(1) == HIGH) {
    button1 = true;
  }
  
  readAndSendLocalEmergencyStop();
  ArduinoOTA.handle();
  if (millis() > 60000) {
    ESP.deepSleep(4294967295);
  }
  delay(1);
}
