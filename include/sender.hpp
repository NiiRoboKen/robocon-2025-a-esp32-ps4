#pragma once

#include "sbtp.hpp"

namespace send
{   
    namespace steering {
        void reset() {
            uint8_t data[1] = {0x10};
            sendSBTP(data, 1);
            delay(80);
        }
        //duty 0 ~ 100 , theta -180 ~ 180, state 1 or 0
        void rotate_duty(uint8_t duty, int16_t theta, uint8_t state) {
            uint8_t data[5] = {0x11, duty, (uint8_t)(theta >> 8), (uint8_t)(theta & 0xFF), state};
            sendSBTP(data, 5);   
        }
        void rotate_robot(bool is_cw, uint8_t duty, uint8_t state) {
            uint8_t cw = is_cw ? 1 : 0;
            uint8_t data[4] = {0x12, cw, duty, state};
            sendSBTP(data, 4);
        }
    }
    namespace roger {
        void move(uint8_t duty, bool is_up) {
            uint8_t up = is_up ? 1 : 0;
            uint8_t data[3] = {0x20, duty, up};
            sendSBTP(data, 3);
        } 
        void moveRight(uint8_t duty, bool is_up) {
            uint8_t up = is_up ? 1 : 0;
            uint8_t data[3] = {0x21, duty, up};
            sendSBTP(data, 3);
        } 
        void moveLeft(uint8_t duty, bool is_up) {
            uint8_t up = is_up ? 1 : 0;
            uint8_t data[3] = {0x22, duty, up};
            sendSBTP(data, 3);
        } 
    }
    namespace arm {
        void right(uint8_t is_open, uint8_t is_open_move, uint8_t is_fold, uint8_t is_fold_move) {
            uint8_t data[5] = {0x31, is_open, is_open_move, is_fold, is_fold_move};
            sendSBTP(data, 5);
        }
        void left(uint8_t is_open, uint8_t is_open_move, uint8_t is_fold, uint8_t is_fold_move) {
            uint8_t data[5] = {0x32, is_open, is_open_move, is_fold, is_fold_move};
            sendSBTP(data, 5);
        }
    }
    namespace arm_up
    {
        void moveRight(uint8_t duty, bool is_up) {
            uint8_t up = is_up ? 1 : 0;
            uint8_t data[3] = {0x41, duty, up};
            sendSBTP(data, 3);
        } 
        void moveLeft(uint8_t duty, bool is_up) {
            uint8_t up = is_up ? 1 : 0;
            uint8_t data[3] = {0x42, duty, up};
            sendSBTP(data, 3);
        } 
    }
    namespace front_arm{
        void moveRight(uint8_t duty1, uint8_t dir1, uint8_t duty2, uint8_t dir2) {
            uint8_t data[5] = {0x51, duty1, dir1, duty2, dir2};
            sendSBTP(data,5);
        }
        void moveLeft(uint8_t duty1, uint8_t dir1, uint8_t duty2, uint8_t dir2) {
            uint8_t data[5] = {0x52, duty1, dir1, duty2, dir2};
            sendSBTP(data,5);
        }
        void suction(bool is_on) {
            uint8_t on = is_on ? 1 : 0;
            uint8_t data[2] = {0x60, on};
            sendSBTP(data, 2);
        }
        void suctionRight(bool is_on) {
            uint8_t on = is_on ? 1 : 0;
            uint8_t data[2] = {0x61, on};
            sendSBTP(data, 2);
        }
        void suctionLeft(bool is_on) {
            uint8_t on = is_on ? 1 : 0;
            uint8_t data[2] = {0x62, on};
            sendSBTP(data, 2);
        }

    }
}
