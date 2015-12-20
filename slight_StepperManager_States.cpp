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

#include "slight_StepperManager_States.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
slight_StepperManager_States::slight_StepperManager_States() {
    // Nothing to do here...
    // only used as static class..
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// motor helper

void slight_StepperManager_States::motor_print_mode(Print &out, uint8_t mode) {
    switch (mode) {
        case 1: {
            out.print(F("full step"));
        } break;
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128: {
            out.print(F("1/"));
            out.print(mode);
            out.print(F(" step"));
        } break;
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public system state & error type printing

void slight_StepperManager_States::error_type_print(Print &out, error_t error) {
    switch (error) {
        case ERROR_none: {
            out.print(F("none"));
        } break;
        case ERROR_timeout: {
            out.print(F("timeout"));
        } break;
        case ERROR_limitswitch_wrong_dir: {
            // out.print(F("LimitSw wrong dir"));
            out.print(F("LimitSwDirection"));
        } break;
        case ERROR_limitswitchs: {
            out.print(F("LimitSw blocked"));
        } break;
        case ERROR_motorstart: {
            out.print(F("motor start"));
        } break;
        case ERROR_motorstop: {
            out.print(F("motor stop"));
        } break;
        case ERROR_calibrating: {
            out.print(F("calibrating"));
        } break;
        case ERROR_mechanics_moved: {
            out.print(F("mechanics moved"));
        } break;
        case ERROR_emergencystop: {
            out.print(F("emergency stop"));
        } break;
    }
}

void slight_StepperManager_States::system_state_print(Print &out, sysstate_t state) {
    switch (state) {
        case STATE_notvalid: {
            out.print(F("notvalid"));
        } break;
        case STATE_standby: {
            out.print(F("standby"));
        } break;
        case STATE_hold_forward: {
            out.print(F("hold forward"));
        } break;
        case STATE_hold_reverse: {
            out.print(F("hold reverse"));
        } break;
        case STATE_moving_forward: {
            out.print(F("moving forward"));
        } break;
        case STATE_moving_reverse: {
            out.print(F("moving reverse"));
        } break;
        case STATE_dirty: {
            out.print(F("dirty"));
        } break;
        case STATE_error: {
            out.print(F("error"));
        } break;
        case STATE_calibrating_check_next: {
            out.print(F("cali check next"));
        } break;
        case STATE_calibrating_start: {
            out.print(F("cali start"));
        } break;
        case STATE_calibrating_forward_start: {
            out.print(F("cali forward start"));
        } break;
        case STATE_calibrating_forward: {
            out.print(F("cali forward"));
        } break;
        case STATE_calibrating_forward_finished: {
            out.print(F("cali forward finished"));
        } break;
        case STATE_calibrating_reverse_start: {
            out.print(F("cali reverse start"));
        } break;
        case STATE_calibrating_reverse: {
            out.print(F("cali reverse"));
        } break;
        case STATE_calibrating_reverse_finished: {
            out.print(F("cali reverse finished"));
        } break;
        case STATE_calibrating_finished: {
            out.print(F("cali finished"));
        } break;
    }
}

// end slight_StepperManager_States_States
