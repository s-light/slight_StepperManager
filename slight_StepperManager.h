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

#ifndef SLIGHT_STEPPERMANAGER_H_
#define SLIGHT_STEPPERMANAGER_H_

namespace slight_StepperManager {

    // #include <kissStepper.h>

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager
    void init();
    void update();

    // callback
    typedef void (* callback_t) ();

    // callback function
    void motor_move_event_set_callback(callback_t);
    void motor_acceleration_event_set_callback(callback_t);
    void motor_enable_event_set_callback(callback_t);
    void system_event_set_callback(callback_t);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // system state

    enum error_t {  // c
        ERROR_none,
        ERROR_timeout,
        ERROR_limitswitch_wrong_dir,
        ERROR_motorstart,
        ERROR_motorstop,
        ERROR_calibrating,
    };
    extern error_t error_type;

    extern void print_error(Print&, error_t);

    enum sysstate_t {  // c
        SYSSTATE_notvalid,
        SYSSTATE_standby,
        SYSSTATE_moving,
        SYSSTATE_dirty_new,
        SYSSTATE_dirty,
        SYSSTATE_error_new,
        SYSSTATE_error,
        SYSSTATE_calibrating_start =    20,
        SYSSTATE_calibrating_forward,
        SYSSTATE_calibrating_forward_finished,
        SYSSTATE_calibrating_reverse,
        SYSSTATE_calibrating_reverse_finished,
        SYSSTATE_calibrating_finished,
    };

    extern sysstate_t system_state;

    extern void print_state(Print&, sysstate_t);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // stepper motor

    // number of full steps in one revolution of the motor
    extern const uint16_t motor_full_steps_revolution;

    // extern kissStepper motor;

    extern int8_t motor_accel_state;
    extern int8_t motor_move_state;
    extern int8_t motor_isenabled;

    extern void motor_print_mode(Print &out, uint8_t mode);
    extern bool motor_move_forward();
    extern bool motor_move_reverse();
    extern bool motor_isEnabled();
    extern bool motor_set_enable(bool);
    extern void system_start_calibration();

};  // end namespace slight_StepperManager

#endif  // SLIGHT_STEPPERMANAGER_H_
