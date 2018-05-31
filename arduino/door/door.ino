#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>

#include "config.h"

#define Open 1
#define Close 0

#define maxBuffer 80
WiFiClient client;
#define SERIALOUT 1

void setup() {
  pinMode(1, INPUT_PULLUP);

  if (SERIALOUT) {
    Serial.begin(9600);
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
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "api.grundid.de";
  Serial.print("requesting URL: ");
  Serial.println(url);
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

  if(SERIALOUT) {
    Serial.println("request sent");
    Serial.println(sensorJson);
    
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String line = client.readString();
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");
  }
  client.stop();
}

void sendTempSensor(float temp, float humidity) {
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "api.grundid.de";
  Serial.print("requesting URL: ");
  Serial.println(url);

  StaticJsonBuffer<200> jsonBuffer;
  JsonArray& array = jsonBuffer.createArray();

  JsonObject& sensor1 = array.createNestedObject();
  sensor1["sensorName"] = "cowo.door.temperature";
  sensor1["value"] = temp;

  JsonObject& sensor2 = array.createNestedObject();
  sensor2["sensorName"] = "cowo.door.humidity";
  sensor2["value"] = humidity;

  String sensorJson = String("POST /sensor HTTP/1.0\r\nHost: "+ hostName +"\r\nContent-Type: application/json\r\nConnection: close\r\n");

  int len = array.measureLength();
  sensorJson += "Content-Length: ";
  sensorJson += len;
  sensorJson += "\r\n\r\n";
  array.printTo(sensorJson);
  client.print(sensorJson);

  if(SERIALOUT) {
    Serial.println("request sent");
    Serial.println(sensorJson);
    
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    String line = client.readString();
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");
  }
  client.stop();
}

int lastState;

void readAndSendLocalDoor() {

    if (!digitalRead(1) && lastState == Close) {
      sendDoorSensor(Open);
      lastState = Open;
      Serial.println("Door open");
    }
    else if (digitalRead(1) && lastState == Open) {
      sendDoorSensor(Close);
      lastState = Close;
      Serial.println("Door close");
    }
}

unsigned long lastLocalSensorTime = 0;

void setupOTA()
{
  // Port defaults to 8266
  ArduinoOTA.setPort(8266);
  // Hostname defaults to MAC address
  ArduinoOTA.setHostname(("Door-ESP " + WiFi.macAddress()).c_str());
  // No authentication by default
  //ArduinoOTA.setPassword(OTA_pass);
  ArduinoOTA.begin();
}

// the loop function runs over and over again forever
void loop() {
  readAndSendLocalDoor(); 
  ArduinoOTA.handle();
  
  delay(50);
}
