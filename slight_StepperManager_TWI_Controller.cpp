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


#include <Wire.h>
#include "slight_StepperManager_TWI.h"
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;
#include "slight_StepperManager_TWI_Controller.h"
typedef slight_StepperManager_TWI_Controller StM_TWI_Con;
// using StM_TWI_Con = slight_StepperManager_TWI_Controller;

#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;

#include "TWI_Anything.h"
#include "slight_StepperManager.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
StM_TWI_Con::slight_StepperManager_TWI_Controller(
    slight_StepperManager &myStManager_new,
    const uint8_t TWI_address_own_new
) :
    myStManager(myStManager_new),
    TWI_address_own(TWI_address_own_new)
{
    // set some initial states:
    register_current = StM_TWI::REG_general_state;
    // set explicit to null
}

// init pointer to null
slight_StepperManager_TWI_Controller *StM_TWI_Con::active_instance = NULL;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// init & update
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Con::begin(Print &out) {
    out.println(F("setup StM_TWI_Con TWI:"));

    out.print(F("\t join bus as slave with address "));
    out.print(TWI_address_own);
    out.println();
    Wire.begin(TWI_address_own);

    out.println(F("\t setup onRequest event handling."));
    Wire.onRequest(TWI_request_event);

    out.println(F("\t setup onReceive event handling."));
    Wire.onReceive(TWI_receive_event);

    // out.println(F("\t activate this instance for TWI events."));
    // activate();
    out.println(F(
        "\t please manually activate this instance for TWI events !"
    ));

    out.println(F("\t finished."));
}

void StM_TWI_Con::update() {
    // handle longer things that are not done in isr
    handle_received();
}

// void StM_TWI_Con::activate() {
void StM_TWI_Con::activate(slight_StepperManager_TWI_Controller *controller) {
    // active_instance = &this;
    StM_TWI_Con::active_instance = controller;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// event / interrupt
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main StepperManager has done generated an event.
// we handle it here to relay to a twi master
void StM_TWI_Con::system_state_changed() {
    // handle event
    // StM_States::sysstate_t system_state
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI handling
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Master has requested some information.
// so we send hime something ;-)
// ISR!!!!
void StM_TWI_Con::TWI_request_event() {
    if (active_instance != NULL) {
        active_instance->handle_onRequest_ISR();
    }  // if active_instance
}

void StM_TWI_Con::handle_onRequest_ISR() {
    handle_request();
}

void StM_TWI_Con::handle_request() {
    // this funciton is called from inside a ISR !!!
    switch (register_current) {
        // ~~~~~~~~~~~~~~~~~~~~~
        // states
        case StM_TWI::REG_general_state: {
            TWI_writeAnything(
                register_general_state
            );
        } break;
        case StM_TWI::REG_system_state: {
            TWI_writeAnything(
                myStManager.system_state_get()
            );
        } break;
        case StM_TWI::REG_error_type: {
            TWI_writeAnything(
                myStManager.error_type_get()
            );
        } break;
        // ~~~~~~~~~~~~~~~~~~~~~
        // actions
        case StM_TWI::REG_action_calibrate:
        case StM_TWI::REG_action_move_forward:
        case StM_TWI::REG_action_move_reverse:
        case StM_TWI::REG_action_emergencystop: {
            // nothing to do.
        } break;
        // ~~~~~~~~~~~~~~~~~~~~~
        // settings
        case StM_TWI::REG_setting_move_speed: {
            TWI_writeAnything(
                myStManager.move_speed_get()
            );
        } break;
        case StM_TWI::REG_setting_move_acceleration: {
            TWI_writeAnything(
                myStManager.move_acceleration_get()
            );
        } break;
        case StM_TWI::REG_setting_calibration_speed: {
            TWI_writeAnything(
                myStManager.calibration_speed_get()
            );
        } break;
        case StM_TWI::REG_setting_calibration_acceleration: {
            TWI_writeAnything(
                myStManager.calibration_acceleration_get()
            );
        } break;
    }  // switch register_name
}


// Master has send some information.
// so we react on this.
// ISR!!!!
void StM_TWI_Con::TWI_receive_event(int rec_bytes) {
    if (active_instance != NULL) {
        active_instance->handle_onReceive_ISR(rec_bytes);
    }  // if active_instance
}

void StM_TWI_Con::handle_onReceive_ISR(int rec_bytes) {
        uint8_t rec_size = rec_bytes;
        if (rec_bytes > 0) {
            // read information
            // read register
            received_register = (StM_TWI::register_name_t)Wire.read();

            // set register pointer to new value (prepare for read & write)
            // this must be done in ISR to be ready for next TWI request.
            register_current = received_register;

            // memory received data bytes
            received_data_size = rec_size -1;
            if (rec_size > 1) {
                // copy data to instance data buffer
                // rec_size-1 because received_register is allready read.
                for (size_t index = 0; index < rec_size-1; index++) {
                    received_data[index] = Wire.read();
                }
            }
            received_flag = true;
        }
}

void StM_TWI_Con::handle_received() {
    if (received_flag) {
        // Serial.println(F("TWI_Con::handle_received:"));
        // handle received things:
        // decide what to do:
        switch (received_register) {
            // ~~~~~~~~~~~~~~~~~~~~~
            // states - read only
            case StM_TWI::REG_general_state:
            case StM_TWI::REG_system_state:
            case StM_TWI::REG_error_type: {
                // just set register pointer to new value (prepare for read)
                // Serial.println(F(
                //     "\tset register pointer to new value (prepare for read)."
                // ));
                // register_current = received_register;
                // nothing to do here.
            } break;
            // ~~~~~~~~~~~~~~~~~~~~~
            // actions - write only
            case StM_TWI::REG_action_calibrate:
            case StM_TWI::REG_action_move_forward:
            case StM_TWI::REG_action_move_reverse:
            case StM_TWI::REG_action_emergencystop: {
                // do action
                // Serial.println(F(
                //     "\thandle_action."
                // ));
                handle_action(received_register);
            } break;
            // ~~~~~~~~~~~~~~~~~~~~~
            // settings - read & write
            case StM_TWI::REG_setting_move_speed:
            case StM_TWI::REG_setting_move_acceleration:
            case StM_TWI::REG_setting_calibration_speed:
            case StM_TWI::REG_setting_calibration_acceleration: {
                // just set register pointer to new value
                // (prepare for read or write)
                // Serial.println(F(
                //     "\tset reg p to new value (prepare for read or write)"
                // ));
                // register_current = received_register;
                Serial.print(F("received_data_size "));
                Serial.println(received_data_size);
                Serial.print(F("received_data "));
                Serial.println(*received_data);
                Serial.print(F("received_data:"));
                for (
                    size_t data_index = 0;
                    data_index < received_data_size;
                    data_index++
                ) {
                    // volatile uint8_t* data_pointer;
                    // data_pointer = received_data + data_index;
                    Serial.print(received_data[data_index], DEC);
                    Serial.print(" ");
                }
                Serial.println();
                // check if write data is there
                if (received_data_size > 0) {
                    // set register to new value.
                    // Serial.println(F(
                    //     "\thandle_register_new_data"
                    // ));
                    handle_register_new_data(
                        received_register,
                        received_data,
                        received_data_size
                    );
                }
            } break;
        }  // switch register_name

        // reset things:
        received_flag = false;
        received_data_size = 0;
        // memset(received_data, 0, received_data_size_max);
        // memset does not work because of volatile..
        for (size_t index = 0; index < received_data_size_max; index++) {
            received_data[index] = 0;
        }
    }
}

void StM_TWI_Con::handle_action(StM_TWI::register_name_t action) {
    switch (action) {
        case StM_TWI::REG_action_calibrate: {
            myStManager.calibrate();
        } break;
        case StM_TWI::REG_action_move_forward: {
            myStManager.move_forward();
        } break;
        case StM_TWI::REG_action_move_reverse: {
            myStManager.move_reverse();
        } break;
        case StM_TWI::REG_action_emergencystop: {
            myStManager.emergencystop();
        } break;
        default: {
            // should never be the case ;-)
            // just is here to prevent compiler warnings about unhandled cases
        } break;
    }  // switch action
}

void StM_TWI_Con::handle_register_new_data(
    StM_TWI::register_name_t register_name,
    volatile uint8_t *data,
    volatile uint8_t data_size
) {
    switch (register_name) {
        case StM_TWI::REG_setting_move_speed: {
            uint16_t value = 0;
            Buffer_readAnything(value, data);
            myStManager.move_speed_set(value);
        } break;
        case StM_TWI::REG_setting_move_acceleration: {
            uint16_t value = 0;
            Buffer_readAnything(value, data);
            Serial.print(F("value "));
            Serial.println(value);
            myStManager.move_acceleration_set(value);
        } break;
        case StM_TWI::REG_setting_calibration_speed: {
            uint16_t value = 0;
            Buffer_readAnything(value, data);
            myStManager.calibration_speed_set(value);
        } break;
        case StM_TWI::REG_setting_calibration_acceleration: {
            uint16_t value = 0;
            Buffer_readAnything(value, data);
            myStManager.calibration_acceleration_set(value);
        } break;
        default: {
            // should never be the case ;-)
            // just is here to prevent compiler warnings about unhandled cases
        } break;
    }  // switch register_name
}


// end slight_StepperManager_TWI_Controller
