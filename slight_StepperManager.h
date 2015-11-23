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

#include <kissStepper.h>
#include <slight_ButtonInput.h>

class slight_StepperManager {
public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // slight_StepperManager

    // slight_StepperManager();

    // slight_StepperManager(
    //     kissStepper &motor_ref,
    //     slight_ButtonInput &LimitSwitch_forward_ref,
    //     slight_ButtonInput &LimitSwitch_reverse_ref,
    //     const uint16_t motor_move_timeout_new,
    //     const int32_t calibration_limit_new
    // );

    slight_StepperManager(
        kissStepper &motor_ref,
        slight_ButtonInput &LimitSwitch_forward_ref,
        slight_ButtonInput &LimitSwitch_reverse_ref,
        const uint16_t motor_move_timeout_new,
        const int32_t calibration_limit_new,
        const int16_t calibration_speed_new,
        uint16_t calibration_limit_threshold_new
    );



    void init();
    void update();

    // callback
    typedef void (* callback_t) ();

    // callback function
    void motor_move_event_set_callback(callback_t);
    void motor_acceleration_event_set_callback(callback_t);
    void motor_enable_event_set_callback(callback_t);
    void system_event_set_callback(callback_t);

    // calibration
    void calibration_start();
    void calibration_limit_threshold_set(uint16_t);
    uint16_t calibration_limit_threshold_get();

    // emergency stop
    void system_emergencystop();

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
        ERROR_emergencystop,
    };
    error_t error_type;
    void print_error(Print&, error_t);

    // enum class sysstate_t : uint8_t {      // c++ typesafe; arduino > 1.6.
    enum sysstate_t {  // c
        SYSSTATE_notvalid,
        SYSSTATE_standby,
        SYSSTATE_hold,
        SYSSTATE_moving_forward,
        SYSSTATE_moving_reverse,
        SYSSTATE_dirty,
        SYSSTATE_error,
        SYSSTATE_calibrating_start =    20,
        SYSSTATE_calibrating_check_next,
        SYSSTATE_calibrating_forward_start,
        SYSSTATE_calibrating_forward,
        SYSSTATE_calibrating_forward_finished,
        SYSSTATE_calibrating_reverse_start,
        SYSSTATE_calibrating_reverse,
        SYSSTATE_calibrating_reverse_finished,
        SYSSTATE_calibrating_finished,
    };
    sysstate_t system_state;
    void print_state(Print&, sysstate_t);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // stepper motor

    kissStepper &motor;

    int8_t motor_accel_state;
    int8_t motor_move_state;
    int8_t motor_isenabled;

    void motor_print_mode(Print &out, uint8_t mode);
    bool motor_move_forward();
    bool motor_move_reverse();
    bool motor_move_forward_raw();
    bool motor_move_reverse_raw();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Limit Switchs

    slight_ButtonInput &LimitSwitch_forward;
    slight_ButtonInput &LimitSwitch_reverse;

    void LimitSwitch_onEvent(slight_ButtonInput *pInstance, uint8_t event);


private:

    const uint16_t motor_move_timeout;
    const int32_t calibration_limit;
    const int32_t calibration_speed;
    uint16_t calibration_limit_threshold;
    int32_t speed_backup;

    bool calibration_direction_forward_done;
    bool calibration_direction_reverse_done;

    uint32_t motor_move_started_timestamp;

    sysstate_t system_state_last;

    callback_t callback_move_event;
    callback_t callback_accelleration_event;
    callback_t callback_enable_event;
    callback_t callback_system_event;


    void system_event_callback();

    void system_state_update();
    void system_state_check_motor_state_change();


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

};  // end namespace slight_StepperManager

#endif  // SLIGHT_STEPPERMANAGER_H_
