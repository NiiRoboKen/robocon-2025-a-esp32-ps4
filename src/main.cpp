#include <Arduino.h>
#include <PS4Controller.h>
#include "sbtp.hpp"
#include "sender.hpp"

//UART1 PIN
const int8_t SERIAL1_RX_PIN = 4;
const int8_t SERIAL1_TX_PIN = 5;

enum class State {
  NotConnected,
  ControlSteering,
  ControlSideArm,
  ControlFrontArm
};

State state = State::NotConnected;

void control_str();
void controlSideArm();
void controlFrontArm();

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, SERIAL1_RX_PIN, SERIAL1_TX_PIN);
  PS4.begin("e4:65:b8:7e:12:1c");
}

void loop() {
    if(!PS4.isConnected()) {
    state = State::NotConnected;
    return;
  }
  
  //ホームボタンでモード切り替え
  static bool is_home_btn = 1;
  static uint8_t state_number = 1;

  if(PS4.PSButton() && !is_home_btn) {
    is_home_btn = true;
    state_number++;
    if(state_number >= 4) state_number = 1;
    state = State(state_number);
  }
  else if(!PS4.PSButton() && is_home_btn) {
    is_home_btn = false;
  }

  switch (state){
    case State::ControlSteering:
      //独ステ制御
      static float rad = 0;
      PS4.setLed(255,0,255);
      PS4.sendToController();
      control_str();
      break;

    case State::ControlSideArm:
      //アーム制御
      PS4.setLed(0,255,255);
      PS4.sendToController();
      controlSideArm();
      break;

    case State::ControlFrontArm:
      //共有アーム
      PS4.setLed(255,0,0);
      PS4.sendToController();
      controlFrontArm();
      break;

    default:
      break;
  }
  delay(50);
}

void control_str(){
  //Serial.println(PS4.Charging());
  Serial.println(PS4.Touchpad());

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

void controlSideArm() {
  //ロジャー機構
  static bool is_roger_btn = false;
  if(PS4.R2Value() > 25 || PS4.L2Value() > 25){
    is_roger_btn = true;
    if(PS4.R2Value() > 25) {
      uint8_t duty = (double)(PS4.R2Value()) * 100 / 255;
      send::roger::move(duty, true);
    }
    else {
      uint8_t duty = (double)(PS4.L2Value()) * 100 / 255;
      send::roger::move(duty, false);
    }
  }
  else if(!(PS4.R2Value() > 25 || PS4.L2Value() > 25) && is_roger_btn) {
    send::roger::move(0, false);
    is_roger_btn = false;
  }

  //右アーム昇降
  static bool is_right_elevator_btn = false;
  if(abs(PS4.RStickY()) > 25){
    is_right_elevator_btn = true;
    bool is_up = PS4.RStickY() > 0;
    uint8_t duty = (double)(abs(PS4.RStickY()) ) * 100. / 127.;
    send::arm_up::moveRight(duty, is_up);
  }
  else if(abs(PS4.RStickY()) < 25 && is_right_elevator_btn){
    is_right_elevator_btn = false;
    send::arm_up::moveRight(0, false);
  }

  //左アーム昇降
  static bool is_left_elevator_btn = false;
  if(abs(PS4.LStickY()) > 25){
    is_left_elevator_btn = true;
    bool is_up = PS4.LStickY() > 0;
    uint8_t duty = (double)(abs(PS4.LStickY())) * 100. / 127.;
    send::arm_up::moveLeft(duty, is_up);
  }
  else if(abs(PS4.LStickY()) < 25 && is_left_elevator_btn){
    is_left_elevator_btn = false;
    send::arm_up::moveLeft(0, false);
  }
  
  //右アーム動作
  //展開
  static bool is_right_arm_open = false;
  if((PS4.Right() || PS4.Left()) && !is_right_arm_open) {
    is_right_arm_open = true;
    uint8_t open = PS4.Right() ? 1 : 0;
    send::arm::right(open, 1, 0, 0);
  }
  else if(!(PS4.Left() || PS4.Right()) && is_right_arm_open) {
    is_right_arm_open = false;
    send::arm::right(0, 0, 0, 0);
  }
  //掴む
  static bool is_right_arm_fold = false;
  if((PS4.Up() || PS4.Down()) && !is_right_arm_fold) {
    is_right_arm_fold = true;
    uint8_t up = PS4.Up() ? 1 : 0;
    send::arm::right(0,0,up,1);
  }
  else if(!(PS4.Up() || PS4.Down()) && is_right_arm_fold) {
    is_right_arm_fold = false;
    send::arm::right(0,0,0,0);
  }

  //左アーム動作
  //展開
  static bool is_left_arm_open = false;
  if((PS4.Circle() || PS4.Square()) && !is_left_arm_open) {
    is_left_arm_open = true;
    uint8_t open = PS4.Circle() ? 1 : 0;
    send::arm::left(open, 1, 0, 0);
  }
  else if(!(PS4.Circle() || PS4.Square()) && is_left_arm_open) {
    is_left_arm_open = false;
    send::arm::left(0, 0, 0, 0);
  }
  //掴む
  static bool is_left_arm_fold = false;
  if((PS4.Triangle() || PS4.Cross()) && !is_left_arm_fold) {
    is_left_arm_fold = true;
    uint8_t up = PS4.Triangle() ? 1 : 0;
    send::arm::left(0,0,up,1);
  }
  else if(!(PS4.Triangle() || PS4.Cross()) && is_left_arm_fold) {
    is_left_arm_fold = false;
    send::arm::left(0,0,0,0);
  }
}

void controlFrontArm() {
  //共有アーム右
  static bool is_arm_right_btn = false;
  if(PS4.R2Value() > 15 || abs(PS4.RStickY()) > 15){
    is_arm_right_btn = true;
    uint8_t duty1 = (double)(PS4.R2Value()) * 100 / 255;
    uint8_t duty2 = (double)(abs(PS4.RStickY())) * 100 / 127;
    uint8_t dir1 = PS4.Triangle() ? 1 : 0;
    uint8_t dir2 = (PS4.RStickY() > 0) ? 0 : 1;
    send::front_arm::moveRight(duty1, dir1, duty2, dir2);
  }
  else if(!(PS4.R2Value() > 15 || abs(PS4.RStickY()) > 15) && is_arm_right_btn) {
    send::front_arm::moveRight(0, 0, 0, 0);
    is_arm_right_btn = false;
  }
  //共有アーム左
  static bool is_arm_left_btn = false;
  if(PS4.L2Value() > 15 || abs(PS4.LStickY()) > 15){
    is_arm_left_btn = true;
    uint8_t duty1 = (double)(PS4.L2Value()) * 100 / 255;
    uint8_t duty2 = (double)(abs(PS4.LStickY())) * 100 / 127;
    uint8_t dir1 = PS4.Up() ? 1 : 0;
    uint8_t dir2 = (PS4.LStickY() > 0) ? 0 : 1;
    send::front_arm::moveLeft(duty1, dir1, duty2, dir2);
  }
  else if(!(PS4.L2Value() > 15 || PS4.LStickY() > 15) && is_arm_left_btn) {
    send::front_arm::moveLeft(0, 0, 0, 0);
    is_arm_left_btn = false;
  }

  //吸引
  static bool is_circle_btn = 1;
  static bool is_suction_on = false;
  if(PS4.Circle() && !is_circle_btn) {
    is_circle_btn = true;
    is_suction_on = !is_suction_on;
    send::front_arm::suction(is_suction_on);
  }
  else if(!PS4.Circle() && is_circle_btn) {
    is_circle_btn = false;
  }
}