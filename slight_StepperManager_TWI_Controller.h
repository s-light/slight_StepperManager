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

#ifndef SLIGHT_STEPPERMANAGER_TWI_CONTROLLER_H_
#define SLIGHT_STEPPERMANAGER_TWI_CONTROLLER_H_

#include "slight_StepperManager_TWI.h"
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;

#include "slight_StepperManager.h"

class slight_StepperManager_TWI_Controller {
public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    slight_StepperManager_TWI_Controller(
        slight_StepperManager &myStManager_new,
        const uint8_t TWI_address_new,
        const uint8_t TWI_address_master_new
    );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // names and definitions

    void begin(Print &);
    void update();

    // void activate();
    static void activate(slight_StepperManager_TWI_Controller *controller);

    // void check_for_interrupt();

    // void check_for_interrupt();

protected:

    static slight_StepperManager_TWI_Controller * active_controller;
    // static slight_StepperManager_TWI_Controller * test;

    slight_StepperManager &myStManager;

    StM_TWI::register_name_t register_current;

    volatile StM_TWI::register_name_t received_register;
    static const uint8_t received_data_size_max = 32;
    volatile uint8_t received_data[received_data_size_max];
    volatile uint8_t received_data_size;
    volatile bool received_flag;

    const uint8_t TWI_address;
    const uint8_t TWI_address_master;

    static void TWI_request_event();
    static void TWI_receive_event(int received_bytes);

    void handle_received();
    void handle_action(StM_TWI::register_name_t action);
    void handle_register_new_data(
        StM_TWI::register_name_t register_name,
        volatile uint8_t *data
    );

};  // slight_StepperManager_TWI_Controller

#endif  // SLIGHT_STEPPERMANAGER_TWI_CONTROLLER_H_
