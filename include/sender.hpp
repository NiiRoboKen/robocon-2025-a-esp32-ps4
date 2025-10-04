#pragma once

#include "sbtp.hpp"

namespace send
{   
    namespace steering {
        void reset() {
            uint8_t data[1] = {0x10};
            sendSBTP(data, 1);
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

}
