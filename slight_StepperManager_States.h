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

#ifndef SLIGHT_STEPPERMANAGER_STATES_H_
#define SLIGHT_STEPPERMANAGER_STATES_H_


class slight_StepperManager_States {
public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    slight_StepperManager_States();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // system state

    enum error_t {  // c
        ERROR_none,
        ERROR_timeout,
        ERROR_limitswitch_wrong_dir,
        ERROR_limitswitchs,
        ERROR_motorstart,
        ERROR_motorstop,
        ERROR_calibrating,
        ERROR_mechanics_moved,
        ERROR_emergencystop,
    };
    static void error_type_print(Print&, error_t);

    // enum class sysstate_t : uint8_t {      // c++ typesafe; arduino > 1.6.
    enum sysstate_t {  // c
        STATE_notvalid,
        STATE_standby,
        STATE_hold_forward,
        STATE_hold_reverse,
        STATE_moving_forward,
        STATE_moving_reverse,
        STATE_dirty,
        STATE_error,
        STATE_calibrating_start =    30,
        STATE_calibrating_check_next,
        STATE_calibrating_forward_start,
        STATE_calibrating_forward,
        STATE_calibrating_forward_finished,
        STATE_calibrating_reverse_start,
        STATE_calibrating_reverse,
        STATE_calibrating_reverse_finished,
        STATE_calibrating_finished,
    };
    static void system_state_print(Print&, sysstate_t);

    static void motor_print_mode(Print &out, uint8_t mode);

};  // slight_StepperManager_States

#endif  // SLIGHT_STEPPERMANAGER_STATES_H_
