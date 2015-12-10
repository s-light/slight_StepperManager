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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
slight_StepperManager_TWI::slight_StepperManager_TWI() {
    // Nothing to do here...
    // only used as static class..
}


void slight_StepperManager_TWI::twi_state_print(Print &out, twi_state_t state) {
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

// end slight_StepperManager_TWI_States
