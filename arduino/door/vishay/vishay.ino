#include <Wire.h>
#include "Adafruit_VCNL4010.h"

Adafruit_VCNL4010 vcnl;

void setup() {
  Serial.begin(115200);
  Serial.println("VCNL4010 test");

  Wire.begin();
  delay(1);
  Wire.beginTransmission(0x13);
  Wire.write(0x81);
  Wire.endTransmission();
  delay(1);
  Wire.requestFrom(0x13, 1);
  Serial.println(Wire.read(), 16);

  delay(20);

  if (! vcnl.begin(0x13)) {
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.println("Found VCNL4010");
}


void loop() {
  Serial.print("Ambient: "); Serial.println(vcnl.readAmbient());
  Serial.print("Proximity: "); Serial.println(vcnl.readProximity());
  delay(100);
}
