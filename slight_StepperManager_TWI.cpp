/************************************************************************

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

*************************************************************************/
/******************************************************************************
    The MIT License (MIT)

    Copyright (c) 2015 Stefan Krüger

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
******************************************************************************/

/** Includes Core Arduino functionality **/
#if ARDUINO
    #if ARDUINO < 100
        #include <WProgram.h>
    #else
        #include <Arduino.h>
    #endif
#endif

#include "slight_StepperManager_TWI.h"
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
StM_TWI::slight_StepperManager_TWI() {
    // Nothing to do here...
    // only used as static class..
}


void StM_TWI::twi_state_print(Print &out, twi_state_t state) {
    switch (state) {
        case TWI_STATE_success: {
            out.print(F("success"));
        } break;
        case TWI_STATE_data_to_long: {
            out.print(F("data too long to fit in transmit buffer"));
        } break;
        case TWI_STATE_rec_NACK_on_address: {
            out.print(F("received NACK on transmit of address"));
        } break;
        case TWI_STATE_rec_NACK_on_data: {
            out.print(F("received NACK on transmit of data"));
        } break;
        case TWI_STATE_other_error: {
            out.print(F("other error"));
        } break;
        default: {
            out.print(F("??"));
        }
    }
}

void StM_TWI::register_name_print(Print &out, register_name_t register_name) {
    switch (register_name) {
        case REG_general_state: {
            out.print(F("general state"));
        } break;
        case REG_system_state: {
            out.print(F("system state"));
        } break;
        case REG_error_type: {
            out.print(F("error type"));
        } break;
        case REG_action_calibrate: {
            out.print(F("action calibrate"));
        } break;
        case REG_action_move_forward: {
            out.print(F("action move forward"));
        } break;
        case REG_action_move_reverse: {
            out.print(F("action move reverse"));
        } break;
        case REG_action_emergencystop: {
            out.print(F("action emergencystop"));
        } break;
        case REG_setting_move_speed: {
            out.print(F("setting move speed"));
        } break;
        case REG_setting_move_acceleration: {
            out.print(F("setting move acceleration"));
        } break;
        case REG_setting_calibration_speed: {
            out.print(F("setting calibration speed"));
        } break;
        case REG_setting_calibration_acceleration: {
            out.print(F("setting calibration acceleration"));
        } break;
        case REG_setting_twi_event_target_address: {
            out.print(F("setting twi event target address"));
        } break;
    }
}


bool StM_TWI::twi_address_valid(uint8_t address) {
    bool result = false;
    if (
        (address == TWI_ADDRESS_BROADCAST) ||
        (
            (7 < address) &&
            (address < 120)
        )
    ) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

// end slight_StepperManager_TWI
