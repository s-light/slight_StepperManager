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

#ifndef SLIGHT_STEPPERMANAGER_TWI_MASTER_H_
#define SLIGHT_STEPPERMANAGER_TWI_MASTER_H_


#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;

#include "slight_StepperManager_TWI.h"
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;


class slight_StepperManager_TWI_Master {
 public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    slight_StepperManager_TWI_Master(
        const uint8_t TWI_address_target_new
    );


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // public lib functions

    void begin(Print &);
    void update();

    // void activate();
    static void activate(slight_StepperManager_TWI_Master *instance);

    void handle_onReceive_ISR(int rec_bytes);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // callback
    typedef void (* callback_t) (slight_StepperManager_TWI_Master *instance);
    void event_callback_set(callback_t);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setter / getter / actions

    // twi
    uint8_t general_state_read();
    StM_States::sysstate_t system_state_read();
    StM_States::error_t error_type_read();

    // get local copy
    uint8_t general_state_get();
    StM_States::sysstate_t system_state_get();
    StM_States::error_t error_type_get();

    void calibrate();
    void move_forward();
    void move_reverse();
    void emergencystop();

    uint16_t settings_move_speed_read();
    void settings_move_speed_write(uint16_t value);
    uint16_t settings_move_acceleration_read();
    void settings_move_acceleration_write(uint16_t value);
    uint16_t settings_calibration_speed_read();
    void settings_calibration_speed_write(uint16_t value);
    uint16_t settings_calibration_acceleration_read();
    void settings_calibration_acceleration_write(uint16_t value);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // basic read write operations
    void write_register(StM_TWI::register_name_t reg_name);

    void write_register_8bit(StM_TWI::register_name_t reg_name, uint8_t value);
    uint8_t read_register_8bit(StM_TWI::register_name_t reg_name);

    void write_register_16bit(
        StM_TWI::register_name_t reg_name,
        uint16_t value
    );
    uint16_t read_register_16bit(StM_TWI::register_name_t reg_name);

 protected:
    // wrapper
    static slight_StepperManager_TWI_Master * active_instance;

    bool ready;

    // TWI things
    const uint8_t TWI_address_target;

    StM_TWI::twi_state_t twi_state;

    volatile uint8_t received_general_state;
    volatile StM_States::sysstate_t received_system_state;
    volatile StM_States::error_t received_error_type;
    volatile bool received_flag;

    static void TWI_receive_event(int received_bytes);
    void handle_received();

    // local copy of states
    uint8_t general_state;
    StM_States::sysstate_t system_state;
    StM_States::error_t error_type;

    callback_t event_callback;
    void fire_event_callback();

    // basic read write operations
    void write_register(uint8_t);

    void write_register_8bit(uint8_t, uint8_t);
    uint8_t read_register_8bit(uint8_t);
    void write_register_16bit(uint8_t, uint16_t);
    uint16_t read_register_16bit(uint8_t);
    // end
};  // slight_StepperManager_TWI_Master

#endif  // SLIGHT_STEPPERMANAGER_TWI_MASTER_H_
