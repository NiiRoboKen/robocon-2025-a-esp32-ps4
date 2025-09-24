#include <Arduino.h>
#include <PS4Controller.h>

void setup() {
  Serial.begin(115200);
  PS4.begin("08:d1:f9:37:0c:26");
}

void loop() {
  if(!PS4.isConnected()) return;
}
