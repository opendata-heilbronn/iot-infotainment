#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoOTA.h>

#include "config.h"

#define maxBuffer 80
WiFiClient client;
#define SERIALOUT 0

DHT dht(D3, DHT22);

void setup() {
  pinMode(D1, INPUT);

  dht.begin();

  if (SERIALOUT) {
    Serial.begin(9800);
    Serial.println("Start Platform IO");
  }
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if (SERIALOUT) {
      Serial.print(".");
    }
  }

  if (SERIALOUT) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  setupOTA();
}

void sendDoorSensor(int doorState) {
  if (client.connect(server, 80)) { 
    //Serial.println(F("connected to server"));
    // Make a HTTP request:
    StaticJsonBuffer<200> jsonBuffer;
    JsonArray& array = jsonBuffer.createArray();

    JsonObject& sensor1 = array.createNestedObject();
    sensor1["sensorName"] = "cowo.door";
    sensor1["value"] = doorState;

    String sensorJson = String("POST /sensor HTTP/1.0\r\nHost: "+hostName+"\r\nContent-Type: application/json\r\nConnection: close\r\n");

    int len = array.measureLength();
    sensorJson += "Content-Length: ";
    sensorJson += len;
    sensorJson += "\r\n\r\n";
    array.printTo(sensorJson);

    client.print(sensorJson);
    client.stop();
    if (SERIALOUT) {
      Serial.println(sensorJson);
      Serial.println("finished");
    }
  }
}

void sendTempSensor(float temp, float humidity) {
  if (client.connect(server, 80)) { 
    //Serial.println(F("connected to server"));
    // Make a HTTP request:
    StaticJsonBuffer<200> jsonBuffer;
    JsonArray& array = jsonBuffer.createArray();

    JsonObject& sensor1 = array.createNestedObject();
    sensor1["sensorName"] = "cowo.door.temperature";
    sensor1["value"] = temp;

    JsonObject& sensor2 = array.createNestedObject();
    sensor2["sensorName"] = "cowo.door.humidity";
    sensor2["value"] = humidity;

    String sensorJson = String("POST /sensor HTTP/1.0\r\nHost: "+hostName+"\r\nContent-Type: application/json\r\nConnection: close\r\n");

    int len = array.measureLength();
    sensorJson += "Content-Length: ";
    sensorJson += len;
    sensorJson += "\r\n\r\n";
    array.printTo(sensorJson);

    client.print(sensorJson);
    client.stop();
    if (SERIALOUT) {
      Serial.println(sensorJson);
      Serial.println("finished");
    }
  }
}

int lastState;

void readAndSendLocalDoor() {

    if (digitalRead(D1) == LOW && lastState == 0) {
      sendDoorSensor(1);
      lastState = 1;
      Serial.println("Door open");
    }
    else if (digitalRead(D1) == HIGH && lastState == 1) {
      sendDoorSensor(0);
      lastState = 0;
      Serial.println("Door close");
    }
}

unsigned long lastLocalSensorTime = 0;

void readAndSendLocalSensor() {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();
    lastLocalSensorTime = millis();
    sendTempSensor(temperature, humidity);
}

bool otaInProgress = false;

void setupOTA()
{
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);
  // Hostname defaults to MAC address
  ArduinoOTA.setHostname(("Door-ESP " + WiFi.macAddress()).c_str());
  // No authentication by default
  //ArduinoOTA.setPassword(OTA_pass);
  ArduinoOTA.onStart([]() {
    Serial.println("[OTA] Start");
    otaInProgress = true;
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] End");
    otaInProgress = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("[OTA] Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("[OTA] Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("[OTA] Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("[OTA] Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("[OTA] End Failed");
    otaInProgress = false;
  });
  ArduinoOTA.begin();
  Serial.println("[OTA] ready");
}

// the loop function runs over and over again forever
void loop() {
  readAndSendLocalDoor(); 
  ArduinoOTA.handle();
  
  delay(500);
  
  long diff = lastLocalSensorTime - millis();
  if (abs(diff) > 10*1000) {
    readAndSendLocalSensor();
  }
}
