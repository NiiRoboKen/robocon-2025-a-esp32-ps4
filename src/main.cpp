#include <Arduino.h>
#include <PS4Controller.h>
#include "sbtp.hpp"
#include "sender.hpp"

const int8_t SERIAL1_RX_PIN = 4;
const int8_t SERIAL1_TX_PIN = 5;

void control_str();

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);
  //PS4.begin("00:02:5b:00:a5:ac");
  PS4.begin("e4:65:b8:7e:12:1c");
}

void loop() {
  if(!PS4.isConnected()) return;
  control_str();
  delay(50);
}

void control_str(){
  int duty_str = PS4.L2Value() * 80 / 255;
  int duty_str_ro = (double)PS4.L2Value() * 30. / 255.;

  double x = PS4.LStickX();
  double y = PS4.LStickY();
  int stick_deg = atan2(y, x) / PI * 180;

  uint8_t state  = PS4.Triangle() ? 1 : 0;

  if(PS4.Circle()) send::steering::reset();
  else if(PS4.R1()) {
    send::steering::rotate_robot(true, duty_str_ro, state);
  }
  else if(PS4.L1()) {
    send::steering::rotate_robot(false, duty_str_ro, state);
  }
  else if(sqrt(x * x + y * y) > 125) {
    send::steering::rotate_duty(duty_str, (int16_t)stick_deg, state);
  }
}