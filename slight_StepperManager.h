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

#ifndef SLIGHT_STEPPERMANAGER_H_
#define SLIGHT_STEPPERMANAGER_H_

#include <kissStepper.h>
#include <kissStepper_TriState.h>
#include <slight_ButtonInput.h>

#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;

class slight_StepperManager {
public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    // slight_StepperManager();

    // slight_StepperManager(
    //     kissStepper_TriState &motor_ref,
    //     slight_ButtonInput &LimitSwitch_forward_ref,
    //     slight_ButtonInput &LimitSwitch_reverse_ref,
    //     const uint16_t motor_move_timeout_new,
    //     const int32_t calibration_limit_new
    // );

    slight_StepperManager(
        kissStepper_TriState &motor_ref,
        slight_ButtonInput &LimitSwitch_forward_ref,
        slight_ButtonInput &LimitSwitch_reverse_ref,
        const uint16_t motor_move_timeout_new,
        const int32_t calibration_limit_new,
        uint16_t calibration_speed_new,
        uint16_t calibration_acceleration_new,
        // uint16_t calibration_limit_threshold_new,
        uint16_t move_speed_new,
        uint16_t move_acceleration_new
    );



    void init(Print &);
    void update();

    // callback
    typedef void (* callback_t) ();

    // callback function
    void motor_move_event_set_callback(callback_t);
    void motor_acceleration_event_set_callback(callback_t);
    void motor_enable_event_set_callback(callback_t);
    void system_event_set_callback(callback_t);

    // calibration
    void calibrate();
    void calibration_limit_threshold_set(uint16_t);
    uint16_t calibration_limit_threshold_get();

    // emergency stop
    void emergencystop();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // system state

    // StM_States::error_t error_type; now private
    StM_States::error_t error_type_get();
    // static void error_type_print(Print&, StM_States::error_t);
    void error_type_print(Print&);

    // sysstate_t system_state; now private
    StM_States::sysstate_t system_state_get();
    // static void system_state_print(Print&, StM_States::sysstate_t);
    void system_state_print(Print&);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // stepper motor

    kissStepper_TriState &motor;

    int8_t motor_accel_state;
    int8_t motor_move_state;
    int8_t motor_isenabled;

    static void motor_print_mode(Print &out, uint8_t mode);
    bool move_forward();
    bool move_reverse();
    bool motor_move_forward_raw();
    bool motor_move_reverse_raw();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // speeds
    void move_speed_set(uint16_t stepsPerSec);
    uint16_t move_speed_get();
    void move_acceleration_set(uint16_t stepsPerSecPerSec);
    uint16_t move_acceleration_get();

    void calibration_speed_set(uint16_t stepsPerSec);
    uint16_t calibration_speed_get();
    void calibration_acceleration_set(uint16_t stepsPerSecPerSec);
    uint16_t calibration_acceleration_get();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Limit Switchs

    slight_ButtonInput &LimitSwitch_forward;
    slight_ButtonInput &LimitSwitch_reverse;

    void LimitSwitch_onEvent(slight_ButtonInput *pInstance, uint8_t event);


protected:

    const uint16_t motor_move_timeout;
    const int32_t calibration_limit;
    uint32_t calibration_speed;
    uint32_t calibration_acceleration;
    uint16_t calibration_limit_threshold;
    uint16_t move_speed;
    uint16_t move_acceleration;

    bool calibration_direction_forward_done;
    bool calibration_direction_reverse_done;

    uint32_t motor_move_started_timestamp;

    StM_States::error_t error_type;
    StM_States::sysstate_t system_state;
    StM_States::sysstate_t system_state_last;

    callback_t callback_move_event;
    callback_t callback_accelleration_event;
    callback_t callback_enable_event;
    callback_t callback_system_event;


    void system_event_callback();

    void system_state_update();
    void system_state_check_motor_state_change();
    void system_check_motor_timeout();
    void system_error();

    void motor_init(Print &out);
    void motor_check_event();


    void system_state_calibrating_start();
    void system_state_calibrating_check_next();
    void system_state_calibrating_global_checks();
    void system_state_calibrating_forward_start();
    void system_state_calibrating_forward_checks();
    void system_state_calibrating_forward_finished();
    void system_state_calibrating_reverse_start();
    void system_state_calibrating_reverse_checks();
    void system_state_calibrating_reverse_finished();
    void system_state_calibrating_finished();

};  // slight_StepperManager

#endif  // SLIGHT_STEPPERMANAGER_H_
