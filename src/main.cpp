#include <Arduino.h>
#include <PS4Controller.h>
#include "sbtp.hpp"

const int8_t SERIAL1_RX_PIN = 4;
const int8_t SERIAL1_TX_PIN = 5;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);
  PS4.begin("08:d1:f9:37:0c:26");
}

void loop() {
  if(!PS4.isConnected()) return;
}
