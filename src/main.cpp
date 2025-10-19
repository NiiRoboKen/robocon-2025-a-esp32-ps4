#include <Arduino.h>
#include <PS4Controller.h>
#include "sbtp.hpp"
#include "sender.hpp"

//UART1 PIN
const int8_t SERIAL1_RX_PIN = 4;
const int8_t SERIAL1_TX_PIN = 5;

enum class State {
  NotConnected,
  Mode1,
  Mode2
};

State state = State::NotConnected;

void control_str();
void controlSideArm();
void controlFrontArm();

void controlMode1();
void controlMode2();

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
  
  //ホームボタンでモード1に切り替え
  static bool is_home_btn = 1;

  if(PS4.PSButton() && !is_home_btn) {
    is_home_btn = true;
    state = State::Mode1;
  }
  else if(!PS4.PSButton() && is_home_btn) {
    is_home_btn = false;
  }

  //タッチパッドでモード2に切り替え
  static bool is_touchpad = 1;

  if(PS4.Touchpad() && !is_touchpad) {
    is_touchpad = true;
    state = State::Mode2;
  }
  else if(!PS4.PSButton() && is_touchpad) {
    is_touchpad = false;
  }

  switch (state){
    case State::Mode1:
      //
      static float rad = 0;
      PS4.setLed(255,0,255);
      PS4.sendToController();
      //control_str();
      controlMode1();
      break;

    case State::Mode2:
      //
      PS4.setLed(0,255,255);
      PS4.sendToController();
      //controlSideArm();
      controlMode2();
      break;

    default:
      break;
  }
  delay(50);
}

void controlMode1() {
  //独ステ
  //=================================================================
  int duty_str = PS4.L2Value() * 95 / 255;
  int duty_str_ro = (double)PS4.L2Value() * 30. / 255.;

  double x = PS4.LStickX();
  double y = PS4.LStickY();
  int stick_deg = atan2(y, x) / PI * 180;

  bool state = PS4.Triangle() | PS4.Circle();
  uint8_t state_number = state ? 1 : 0;

  if(PS4.Circle()) {
    duty_str = duty_str / 2;
  }

  if(PS4.Square()) send::steering::reset();
  else if(PS4.R1()) {
    send::steering::rotate_robot(true, duty_str_ro, state_number);
  }
  else if(PS4.L1()) {
    send::steering::rotate_robot(false, duty_str_ro, state_number);
  }
  else if(sqrt(x * x + y * y) > 125) {
    send::steering::rotate_duty(duty_str, (int16_t)stick_deg, state_number);
  }

  //吸引
  //=================================================================

  //右
  static bool is_option_btn = true;
  static bool is_right_suction = false;
  if(PS4.Options() && !is_option_btn) {
    is_option_btn = true;
    is_right_suction = !is_right_suction;
    send::front_arm::suctionRight(is_right_suction);
  }
  else if(!PS4.Options() && is_option_btn) {
    is_option_btn = false;
  }

  //左
  static bool is_share_btn = true;
  static bool is_left_suction = false;
  if(PS4.Share() && !is_share_btn) {
    is_share_btn = true;
    is_left_suction = !is_left_suction;
    send::front_arm::suctionLeft(is_left_suction);
  }
  else if(!PS4.Share() && is_share_btn) {
    is_share_btn = false;
  }

  //side Arm
  //=================================================================

  //展開 右
  static bool is_right_arm_open = false;
  if((PS4.Right() || PS4.Left()) && !is_right_arm_open) {
    is_right_arm_open = true;
    uint8_t open = PS4.Right() ? 1 : 0;
    send::arm::right(open, 1, 0, 0);
  }
  else if(!(PS4.Left() || PS4.Right()) && is_right_arm_open) {
    is_right_arm_open = false;
  }

  //展開 左
  static bool is_left_arm_open = false;
  if((PS4.Up() || PS4.Down()) && !is_left_arm_open) {
    is_left_arm_open = true;
    uint8_t open = PS4.Up() ? 1 : 0;
    send::arm::left(open, 1, 0, 0);
  }
  else if(!(PS4.Up() || PS4.Down()) && is_left_arm_open) {
    is_left_arm_open = false;
  }

  //ロジャー
  //=================================================================

  //昇降
  static bool is_roger_btn = false;
  if(abs(PS4.RStickY()) > 25){
    is_roger_btn = true;

    bool is_up = PS4.RStickY() > 0;
    int default_rate = (int)abs(PS4.RStickY());
    int right_rate = default_rate, left_rate = default_rate;

    if(PS4.RStickX() > 0) {
      right_rate -= PS4.RStickX() / 2;
      if(right_rate < 0) right_rate = 0;
    }
    else if(PS4.RStickX() < 0) {
      left_rate += PS4.RStickX() / 2;
      if(left_rate < 0) left_rate = 0;
    }

    uint8_t right_duty = (double)right_rate * 100. / 127.;
    uint8_t left_duty = (double)left_rate * 100. / 127.;

    send::roger::moveRight(right_duty, is_up);
    send::roger::moveLeft(left_duty, is_up);
  }
  else if(abs(PS4.RStickY()) < 25 && is_roger_btn){
    is_roger_btn = false;
    send::roger::moveRight(0, false);
    send::roger::moveLeft(0, false);
  }

}

void controlMode2() {
  //アーム昇降
  //=================================================================

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

  //アーム動作
  //=================================================================

  //掴む　右
  static bool is_right_arm_fold = false;
  if((PS4.Circle() || PS4.Square()) && !is_right_arm_fold) {
    is_right_arm_fold = true;
    uint8_t hold = PS4.Circle() ? 1 : 0;
    send::arm::right(0,0,hold,1);
  }
  else if(!(PS4.Circle() || PS4.Square()) && is_right_arm_fold) {
    is_right_arm_fold = false;
    send::arm::right(0,0,0,0);
  }

  //掴む　左
  static bool is_left_arm_hold = false;
  if((PS4.Right() || PS4.Left()) && !is_left_arm_hold) {
    is_left_arm_hold = true;
    uint8_t hold = PS4.Right() ? 1 : 0;
    send::arm::left(0,0,hold,1);
  }
  else if(!(PS4.Right() || PS4.Left()) && is_left_arm_hold) {
    is_left_arm_hold = false;
    send::arm::left(0,0,0,0);
  }

  //共有アーム
  //=================================================================

  //共有アーム右
  static bool is_front_arm_right_btn = false;
  if(PS4.R2Value() > 15){
    is_front_arm_right_btn = true;
    uint8_t duty1 = (double)(PS4.R2Value()) * 100 / 255;
    uint8_t dir1 = PS4.Triangle() ? 1 : 0;
    send::front_arm::moveRight(duty1, dir1, 0, 0);
  }
  else if(!(PS4.R2Value() > 15) && is_front_arm_right_btn) {
    send::front_arm::moveRight(0, 0, 0, 0);
    is_front_arm_right_btn = false;
  }
  //共有アーム左
  static bool is_front_arm_left_btn = false;
  if(PS4.L2Value() > 15){
    is_front_arm_left_btn = true;
    uint8_t duty1 = (double)(PS4.L2Value()) * 100 / 255;
    uint8_t dir1 = PS4.Up() ? 1 : 0;
    send::front_arm::moveLeft(duty1, dir1, 0, 0);
  }
  else if(!(PS4.L2Value() > 15)  && is_front_arm_left_btn) {
    send::front_arm::moveLeft(0, 0, 0, 0);
    is_front_arm_left_btn = false;
  }
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