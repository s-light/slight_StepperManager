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

#include "slight_StepperManager.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
StM_TWI_Con::slight_StepperManager_TWI_Controller(
    const slight_StepperManager &myStManager_new,
    const uint8_t TWI_address_new,
    const uint8_t TWI_address_master_new
) :
    myStManager(myStManager_new),
    TWI_address(TWI_address_new),
    TWI_address_master(TWI_address_master_new)
{
    // set some initial states:
    register_current = StM_TWI::REG_general_state;
    // set explicit to null
}

// init pointer to null
slight_StepperManager_TWI_Controller *StM_TWI_Con::active_controller = NULL;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// init & update
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Con::begin(Print &out) {
    out.println(F("setup TWI:"));

    out.print(F("\t join bus as slave with address "));
    out.print(TWI_address);
    out.println();
    Wire.begin(TWI_address);

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
}

// void StM_TWI_Con::activate() {
void StM_TWI_Con::activate(slight_StepperManager_TWI_Controller *controller) {
    // active_controller = &this;
    StM_TWI_Con::active_controller = controller;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI handling
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Master has requested some information.
// so we send hime something ;-)
// ISR!!!!
void StM_TWI_Con::TWI_request_event() {
    if (active_controller != NULL) {
        switch (active_controller->register_current) {
            // states
            case StM_TWI::REG_general_state: {
                // things to do
            } break;
            case StM_TWI::REG_system_state: {
                // things to do
            } break;
            case StM_TWI::REG_error_type: {
                // things to do
            } break;
            // actions
            case StM_TWI::REG_action_calibrate: {
                // things to do
            } break;
            case StM_TWI::REG_action_move_forward: {
                // things to do
            } break;
            case StM_TWI::REG_action_move_reverse: {
                // things to do
            } break;
            case StM_TWI::REG_action_emergencystop: {
                // things to do
            } break;
            // settings
            case StM_TWI::REG_setting_move_speed: {
                // things to do
            } break;
            case StM_TWI::REG_setting_move_acceleration: {
                // things to do
            } break;
            case StM_TWI::REG_setting_calibration_speed: {
                // things to do
            } break;
            case StM_TWI::REG_setting_calibration_acceleration: {
                // things to do
            } break;
        }  // switch register_name
    }  // if active_controller
}

// Master has send some information.
// so we react on this.
// ISR!!!!
void StM_TWI_Con::TWI_receive_event(int received_size) {
    if (active_controller != NULL) {
        // read information
        // read register
        StM_TWI::register_name_t reg;
        reg = (StM_TWI::register_name_t)Wire.read();
        // decide what to do:
        switch (active_controller->register_current) {
            // states - read only
            case StM_TWI::REG_general_state:
            case StM_TWI::REG_system_state:
            case StM_TWI::REG_error_type: {
                // just set register pointer to new value (prepare for read)
                active_controller->register_current = reg;
            } break;
            // actions - write only
            case StM_TWI::REG_action_calibrate:
            case StM_TWI::REG_action_move_forward:
            case StM_TWI::REG_action_move_reverse:
            case StM_TWI::REG_action_emergencystop: {
                // prepare action so next 'update' can handle this.

            } break;
            // settings - read & write
            case StM_TWI::REG_setting_move_speed:
            case StM_TWI::REG_setting_move_acceleration:
            case StM_TWI::REG_setting_calibration_speed:
            case StM_TWI::REG_setting_calibration_acceleration: {
                // set register pointer to new value.
                active_controller->register_current = reg;
                if (received_size > 1) {
                    // set register to new value.
                    //
                }
            } break;
        }  // switch register_name
    }  // if active_controller
}


// end slight_StepperManager_TWI_Controller
