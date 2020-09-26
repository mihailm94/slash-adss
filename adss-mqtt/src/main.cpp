#include <Arduino.h>
#include <WiFi.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println("Hello Slash Berlin");
  delay(1000);
}