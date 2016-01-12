/******************************************************************************

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

******************************************************************************/
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

#ifndef SLIGHT_STEPPERMANAGER_TWI_H_
#define SLIGHT_STEPPERMANAGER_TWI_H_


class slight_StepperManager_TWI {
public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    slight_StepperManager_TWI();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // names and definitions

    enum general_state_mask_t {
        GSTATE_interruppt = B00000001,
        GSTATE_system_state_changed = B00000010,
        GSTATE_error_type_changed = B00000100,
        GSTATE_reserved1 = B00001000,
    };

    enum twi_state_t {
        TWI_STATE_success = 0,
        TWI_STATE_data_to_long = 1,
        TWI_STATE_rec_NACK_on_address = 2,
        TWI_STATE_rec_NACK_on_data = 3,
        TWI_STATE_other_error = 4,
        TWI_STATE_undefined = 99,
    };
    static void twi_state_print(Print &out, twi_state_t state);

    // enum class sysstate_t : uint8_t {      // c++ typesafe; arduino > 1.6.
    enum register_name_t {  // c
        REG_general_state,  // R/W contains interrupt flag
        REG_system_state,  // R system_state
        REG_error_type,  // R error_type
        REG_action_calibrate,  // W starts calibration
        REG_action_move_forward,  // W starts move forward
        REG_action_move_reverse,  // W starts move reverse
        REG_action_emergencystop,  // W starts emergencystop
        REG_setting_move_speed,  // R/W 2Byte
        REG_setting_move_acceleration,  // R/W 2Byte
        REG_setting_calibration_speed,  // R/W 2Byte
        REG_setting_calibration_acceleration,  // R/W 2Byte
        REG_setting_twi_event_target_address,  // R/W 1Byte
    };
    static void register_name_print(Print &out, register_name_t register_name);

    static const uint8_t TWI_ADDRESS_BROADCAST = 0;
    static const uint8_t TWI_ADDRESS_INVALID = 255;

    static bool twi_address_valid(uint8_t address);
};  // slight_StepperManager_TWI

#endif  // SLIGHT_STEPPERMANAGER_TWI_H_
