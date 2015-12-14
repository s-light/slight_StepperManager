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
        const uint8_t TWI_address_own_new,
        const uint8_t TWI_address_extern_new
    );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // names and definitions

    void begin(Print &);
    void update();

    // void activate();
    static void activate(slight_StepperManager_TWI_Master *instance);

 protected:
    // wrap
    static slight_StepperManager_TWI_Master * active_instance;

    // TWI things
    const uint8_t TWI_address_own;
    const uint8_t TWI_address_extern;

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

    // end
};  // slight_StepperManager_TWI_Master

#endif  // SLIGHT_STEPPERMANAGER_TWI_MASTER_H_
