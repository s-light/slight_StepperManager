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
    const uint8_t TWI_address_own_new,
    const uint8_t TWI_address_extern_new
) :
    TWI_address_own(TWI_address_own_new),
    TWI_address_extern(TWI_address_extern_new)
{
    // set some initial states:
}

// init pointer to null
slight_StepperManager_TWI_Master *StM_TWI_Master::active_instance = NULL;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// init & update
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StM_TWI_Master::begin(Print &out) {
    out.println(F("setup TWI:"));

    out.print(F("\t join bus as slave with address "));
    out.print(TWI_address_own);
    out.println();
    Wire.begin(TWI_address_own);

    out.println(F("\t setup onReceive event handling."));
    Wire.onReceive(TWI_receive_event);

    out.println(F("\t finished."));
}

void StM_TWI_Master::update() {
    // handle longer things that are not done in isr
}

// void StM_TWI_Master::activate() {
void StM_TWI_Master::activate(slight_StepperManager_TWI_Master *instance) {
    // active_instance = &this;
    StM_TWI_Master::active_instance = instance;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI handling
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// extern has send some information.
// so we react on this.
// ISR!!!!
void StM_TWI_Master::TWI_receive_event(int rec_bytes) {
    if (active_instance != NULL) {
        uint8_t rec_size = rec_bytes;
        if (rec_bytes > 0) {
            // read general_state
            active_instance->received_general_state =
                (StM_TWI::register_name_t)Wire.read();

            // check for system_state
            if (rec_size > 1) {
                active_instance->received_system_state =
                    (StM_States::sysstate_t)Wire.read();

                // check for error_type
                if (rec_size > 2) {
                    active_instance->received_error_type =
                        (StM_States::error_t)Wire.read();
                }
            }
            active_instance->received_flag = true;
        }
    }  // if active_instance
}

// we got data from TWI controller
// that is normaly only a status update.
void StM_TWI_Master::handle_received() {
    if (received_flag) {
        // handle received things:

        // reset things:
        received_flag = false;
    }
}


// end slight_StepperManager_TWI_Master
