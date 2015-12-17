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

#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;

#include <Wire.h>
#include "slight_StepperManager_TWI.h"
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;
#include "slight_StepperManager_TWI_Master.h"
typedef slight_StepperManager_TWI_Master StM_TWI_Master;
// using StM_TWI_Master = slight_StepperManager_TWI_Master;

#include "TWI_Anything.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
StM_TWI_Master::slight_StepperManager_TWI_Master(
    const uint8_t TWI_address_target_new
) :
    TWI_address_target(TWI_address_target_new)
{
    // set some initial states:
}

// init pointer to null
slight_StepperManager_TWI_Master *StM_TWI_Master::active_instance = NULL;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// init & update
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Master::begin(Print &out) {
    // out.println(F("setup TWI:"));
    //
    // out.print(F("\t join bus as slave with address "));
    // out.print(TWI_address_own);
    // out.println();
    // Wire.begin(TWI_address_own);
    //
    // out.println(F("\t setup onReceive event handling."));
    // Wire.onReceive(TWI_receive_event);
    //
    // out.println(F("\t finished."));
    ready = true;
}

void StM_TWI_Master::update() {
    // handle longer things that are not done in isr
    handle_received();
}

// void StM_TWI_Master::activate() {
void StM_TWI_Master::activate(slight_StepperManager_TWI_Master *instance) {
    // active_instance = &this;
    StM_TWI_Master::active_instance = instance;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// event callback
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Master::event_callback_set(callback_t callback_function) {
    event_callback = callback_function;
}

void StM_TWI_Master::fire_event_callback() {
    if (event_callback) {
        event_callback(this);
    }
}


uint8_t StM_TWI_Master::general_state_get() {
    return general_state;
}

StM_States::sysstate_t StM_TWI_Master::system_state_get() {
    return system_state;
}

StM_States::error_t StM_TWI_Master::error_type_get() {
    return error_type;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI handling
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Master has send some information.
// so we react on this.
// ISR!!!!
void StM_TWI_Master::TWI_receive_event(int rec_bytes) {
    if (active_instance != NULL) {
        active_instance->handle_onReceive_ISR(rec_bytes);
    }  // if active_instance
}

void StM_TWI_Master::handle_onReceive_ISR(int rec_bytes) {
    if (active_instance != NULL) {
        uint8_t rec_size = rec_bytes;
        if (rec_bytes > 0) {
            // read general_state
            received_general_state = (StM_TWI::register_name_t)Wire.read();

            // check for system_state
            if (rec_size > 1) {
                received_system_state = (StM_States::sysstate_t)Wire.read();

                // check for error_type
                if (rec_size > 2) {
                    received_error_type = (StM_States::error_t)Wire.read();
                } else {
                    received_error_type = StM_States::ERROR_none;
                }
            }
            received_flag = true;
        }
    }  // if active_instance
}

// we got data from TWI controller
// that is normaly only a status update.
void StM_TWI_Master::handle_received() {
    if (received_flag) {
        // handle received things:
        general_state = received_general_state;
        system_state = received_system_state;
        error_type = received_error_type;
        // generate event...
        fire_event_callback();
        // reset things:
        received_flag = false;
    }
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI setter / getter / actions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

uint8_t StM_TWI_Master::general_state_read() {
    general_state = read_register_8bit(StM_TWI::REG_general_state);
    return general_state;
}

StM_States::sysstate_t StM_TWI_Master::system_state_read() {
    uint8_t temp;
    temp = read_register_8bit(StM_TWI::REG_system_state);
    system_state = (StM_States::sysstate_t)temp;
    return system_state;
}

StM_States::error_t StM_TWI_Master::error_type_read() {
    uint8_t temp;
    temp = read_register_8bit(StM_TWI::REG_error_type);
    error_type = (StM_States::error_t)temp;
    return error_type;
}


void StM_TWI_Master::calibrate() {
    write_register(StM_TWI::REG_action_calibrate);
}

void StM_TWI_Master::move_forward() {
    write_register(StM_TWI::REG_action_move_forward);
}

void StM_TWI_Master::move_reverse() {
    write_register(StM_TWI::REG_action_move_reverse);
}

void StM_TWI_Master::emergencystop() {
    write_register(StM_TWI::REG_action_emergencystop);
}


uint16_t StM_TWI_Master::settings_move_speed_read() {
    return read_register_16bit(StM_TWI::REG_setting_move_speed);
}

void StM_TWI_Master::settings_move_speed_write(uint16_t value) {
    write_register_16bit(
        StM_TWI::REG_setting_move_speed,
        value
    );
}

uint16_t StM_TWI_Master::settings_move_acceleration_read() {
    return read_register_16bit(StM_TWI::REG_setting_move_acceleration);
}

void StM_TWI_Master::settings_move_acceleration_write(uint16_t value) {
    write_register_16bit(
        StM_TWI::REG_setting_move_acceleration,
        value
    );
}

uint16_t StM_TWI_Master::settings_calibration_speed_read() {
    return read_register_16bit(StM_TWI::REG_setting_calibration_speed);
}

void StM_TWI_Master::settings_calibration_speed_write(uint16_t value) {
    write_register_16bit(
        StM_TWI::REG_setting_calibration_speed,
        value
    );
}

uint16_t StM_TWI_Master::settings_calibration_acceleration_read() {
    return read_register_16bit(StM_TWI::REG_setting_calibration_acceleration);
}

void StM_TWI_Master::settings_calibration_acceleration_write(uint16_t value) {
    write_register_16bit(
        StM_TWI::REG_setting_calibration_acceleration,
        value
    );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// basic read write operations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Master::write_register(uint8_t reg_name) {
    if (ready) {
        twi_state = StM_TWI::TWI_STATE_undefined;
        // set register
        Wire.beginTransmission(TWI_address_target);
        Wire.write(reg_name);
        twi_state = (StM_TWI::twi_state_t)Wire.endTransmission();
        if (twi_state == StM_TWI::TWI_STATE_success) {
            // all fine.
        } else {
            // print_transmission_state(Serial, twi_state);
        }
    }
}

void StM_TWI_Master::write_register(
    StM_TWI::register_name_t reg_name
) {
    write_register((uint8_t)reg_name);
}


void StM_TWI_Master::write_register_8bit(uint8_t reg_name, uint8_t value) {
    if (ready) {
        twi_state = StM_TWI::TWI_STATE_undefined;
        // set register
        Wire.beginTransmission(TWI_address_target);
        Wire.write(reg_name);
        Wire.write(value);
        twi_state = (StM_TWI::twi_state_t)Wire.endTransmission();
        if (twi_state == StM_TWI::TWI_STATE_success) {
            // all fine.
        } else {
            // print_transmission_state(Serial, twi_state);
        }
    }
}

void StM_TWI_Master::write_register_8bit(
    StM_TWI::register_name_t reg_name,
    uint8_t value
) {
    write_register_8bit((uint8_t)reg_name, value);
}

uint8_t StM_TWI_Master::read_register_8bit(uint8_t reg_name) {
    uint8_t result_value = 0;
    if (ready) {
        twi_state = StM_TWI::TWI_STATE_undefined;
        // set register
        Wire.beginTransmission(TWI_address_target);
        Wire.write(reg_name);
        twi_state = (StM_TWI::twi_state_t)Wire.endTransmission();
        if (twi_state == StM_TWI::TWI_STATE_success) {
            // read data
            Wire.requestFrom(TWI_address_target, sizeof(uint8_t));
            result_value = Wire.read();
        } else {
            // print_transmission_state(Serial, twi_state);
        }
    }
    return result_value;
}

uint8_t StM_TWI_Master::read_register_8bit(
    StM_TWI::register_name_t reg_name
) {
    return read_register_8bit((uint8_t)reg_name);
}


void StM_TWI_Master::write_register_16bit(uint8_t reg_name, uint16_t value) {
    if (ready) {
        twi_state = StM_TWI::TWI_STATE_undefined;
        // set register
        Wire.beginTransmission(TWI_address_target);
        Wire.write(reg_name);
        TWI_writeAnything(value);
        twi_state = (StM_TWI::twi_state_t)Wire.endTransmission();
        if (twi_state == StM_TWI::TWI_STATE_success) {
            // all fine.
        } else {
            // print_transmission_state(Serial, twi_state);
        }
    }
}

void StM_TWI_Master::write_register_16bit(
    StM_TWI::register_name_t reg_name,
    uint16_t value
) {
    write_register_16bit((uint8_t)reg_name, value);
}

uint16_t StM_TWI_Master::read_register_16bit(uint8_t reg_name) {
    uint16_t result_value = 0;
    if (ready) {
        twi_state = StM_TWI::TWI_STATE_undefined;
        // set register
        Wire.beginTransmission(TWI_address_target);
        Wire.write(reg_name);
        twi_state = (StM_TWI::twi_state_t)Wire.endTransmission();
        if (twi_state == StM_TWI::TWI_STATE_success) {
            // read data
            Wire.requestFrom(TWI_address_target, sizeof(uint16_t));
            TWI_readAnything(result_value);
        } else {
            // print_transmission_state(Serial, twi_state);
        }
    }
    return result_value;
}

uint16_t StM_TWI_Master::read_register_16bit(
    StM_TWI::register_name_t reg_name
) {
    return read_register_8bit((uint8_t)reg_name);
}



// end slight_StepperManager_TWI_Master
