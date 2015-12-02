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

#include "slight_StepperManager.h"

#include <kissStepper.h>
#include <slight_ButtonInput.h>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
slight_StepperManager::slight_StepperManager(
    kissStepper &motor_ref,
    slight_ButtonInput &LimitSwitch_forward_ref,
    slight_ButtonInput &LimitSwitch_reverse_ref,
    const uint16_t motor_move_timeout_new = 5000,
    const int32_t calibration_limit_new = 12800,
    uint16_t calibration_speed_new = 50,
    uint16_t calibration_acceleration_new = 25,
    // uint16_t calibration_limit_threshold_new = 32
    uint16_t move_speed_new = 200,
    uint16_t move_acceleration_new = 50
) :
    motor(motor_ref),
    LimitSwitch_forward(LimitSwitch_forward_ref),
    LimitSwitch_reverse(LimitSwitch_reverse_ref),
    motor_move_timeout(motor_move_timeout_new),
    calibration_limit(calibration_limit_new)
{
    calibration_speed = calibration_speed_new;
    calibration_acceleration = calibration_acceleration_new;
    move_speed = move_speed_new;
    move_acceleration = move_acceleration_new;
    // calibration_limit_threshold = calibration_limit_threshold_new;

    error_type = ERROR_none;
    system_state = SYSSTATE_dirty;
    motor_move_started_timestamp = millis();
    motor_accel_state = 0;
    motor_move_state = 0;
    motor_isenabled = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// init & update
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void slight_StepperManager::init(Print &out) {
    // init motor things
    motor_init(out);
}

void slight_StepperManager::update() {
    // update motor things
    motor.work();
    motor_check_event();
    if (motor_move_state != 0) {
        system_check_motor_timeout();
    }
    system_state_update();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// LimitSwitchs functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void slight_StepperManager::LimitSwitch_onEvent(
    slight_ButtonInput *pInstance,
    uint8_t event
) {
    // Print &out = Serial
    // out.print(F("Instance ID:"));
    // out.println((*pInstance).getID());
    // out.print(F("Event: "));
    // (*pInstance).printEvent(out, event);
    // out.println();

    uint8_t limitswitch_id = (*pInstance).getID();

    // show event additional infos:
    switch (event) {
        // case slight_ButtonInput::event_StateChanged : {
        //     out.print(F("\t state: "));
        //     (*pInstance).printState(out);
        //     out.println();
        // } break;
        case slight_ButtonInput::event_Down : {
            // out.println(F("button pressed down!"));
            switch (limitswitch_id) {
                case 0: {
                    // limit switch 1
                    motor.stop();
                    switch (system_state) {
                        // case SYSSTATE_calibrating_forward: {
                        //     // set limit to current position
                        //     motor.forwardLimit = motor.getPos();
                        //     // start calibration reverse
                        //     system_state = SYSSTATE_calibrating_reverse;
                        //     motor.moveTo(motor.reverseLimit);
                        // } break;
                        case SYSSTATE_calibrating_reverse: {
                            // something went wrong
                            error_type = ERROR_limitswitch_wrong_dir;
                            system_error();
                        } break;
                        // case SYSSTATE_standby: {
                        //     // calibration is not exact enough.
                        //     system_state = SYSSTATE_dirty;
                        // } break;
                        default: {
                            // nothing to do here.
                        }
                    }  // switch system_state
                } break;
                case 1: {
                    // limit switch 2
                    motor.stop();
                    switch (system_state) {
                        case SYSSTATE_calibrating_forward: {
                            // something went wrong
                            error_type = ERROR_limitswitch_wrong_dir;
                            system_error();
                        } break;
                        // case SYSSTATE_standby: {
                        //     // calibration is not exact enough.
                        //     system_state = SYSSTATE_dirty;
                        // } break;
                        default: {
                            // nothing to do here.
                        }
                    }  // switch system_state
                } break;
            }  // end switch button_id
        } break;
        // case slight_ButtonInput::event_HoldingDown : {
        //     out.print(F("duration active: "));
        //     out.println((*pInstance).getDurationActive());
        // } break;
        case slight_ButtonInput::event_Up : {
            // out.println(F("up"));
            // react if motor is not moving but
            // mechanics are moving away from switch.
            switch (system_state) {
                case SYSSTATE_standby:
                case SYSSTATE_hold_forward:
                case SYSSTATE_hold_reverse: {
                    // system_state = SYSSTATE_dirty;
                    error_type = ERROR_mechanics_moved;
                    system_error();
                } break;
                default: {
                    // nothing to do here.
                }
            }  // switch system_state
        } break;
        // case slight_ButtonInput::event_Click : {
        //     // out.println(F("click"));
        // } break;
        // case slight_ButtonInput::event_ClickLong : {
        //     // out.println(F("click long"));
        // } break;
        // case slight_ButtonInput::event_ClickDouble : {
        //     out.println(F("click double"));
        // } break;
        // case slight_ButtonInput::event_ClickTriple : {
        //     out.println(F("click triple"));
        // } break;
        // case slight_ButtonInput::event_ClickMulti : {
        //     out.print(F("click count: "));
        //     out.println((*pInstance).getClickCount());
        // } break;
    }  // end switch
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// motor functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void slight_StepperManager::motor_init(Print &out) {
    out.println(F("setup stepper motor:"));
    // init motor
    out.println(F("\t set:"));
    out.println(F("\t     MICROSTEP_8"));
    out.println(F("\t     max speed 200 steps/s"));
    out.println(F("\t     acceleration 200 steps/s^2"));
    motor.begin(MICROSTEP_8, 200, 200);

    out.println(F("\t     limits:"));
    // int32_t limit = 12800;
    motor.forwardLimit = calibration_limit;
    motor.reverseLimit = -calibration_limit;
    // print current/new limits
    out.print(F("\t     * forwardLimit: "));
    out.print(motor.forwardLimit);
    out.println();
    out.print(F("\t     * reverseLimit: "));
    out.print(motor.reverseLimit);
    out.println();

    // motor.moveTo(6400);

    out.println(F("\t finished."));
}

void slight_StepperManager::motor_print_mode(Print &out, uint8_t mode) {
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


void slight_StepperManager::motor_check_event() {
    int8_t new_move_state = motor.getMoveState();
    if (new_move_state != motor_move_state) {
        // event!
        motor_move_state = new_move_state;
        if (callback_move_event) {
            callback_move_event();
        }
        system_state_check_motor_state_change();
    }
    int8_t new_accel_state = motor.getAccelState();
    if (new_accel_state != motor_accel_state) {
        // event!
        motor_accel_state = new_accel_state;
        if (callback_accelleration_event) {
            callback_accelleration_event();
        }
    }
    int8_t new_isenabled = motor.isEnabled();
    if (new_isenabled != motor_isenabled) {
        // event!
        motor_isenabled = new_isenabled;
        if (callback_enable_event) {
            callback_enable_event();
        }
    }
}


bool slight_StepperManager::motor_move_forward() {
    bool motor_started = false;
    // check for system_state
    // only move if standby or moving.
    switch (system_state) {
        case SYSSTATE_standby:
        case SYSSTATE_hold_forward:
        case SYSSTATE_hold_reverse:
        case SYSSTATE_moving_forward:
        case SYSSTATE_moving_reverse: {
            motor_started = motor_move_forward_raw();
        } break;
        default: {
            // if system has other state dont do something.
            motor_started = false;
        }
    }  // switch system_state
    return motor_started;
}

bool slight_StepperManager::motor_move_reverse() {
    bool motor_started = false;
    // check for system_state
    // only move if standby or moving.
    switch (system_state) {
        case SYSSTATE_standby:
        case SYSSTATE_hold_forward:
        case SYSSTATE_hold_reverse:
        case SYSSTATE_moving_forward:
        case SYSSTATE_moving_reverse: {
            motor_started = motor_move_reverse_raw();
        } break;
        default: {
            // if system has other state dont do something.
            motor_started = false;
        }
    }  // switch system_state
    return motor_started;
}

bool slight_StepperManager::motor_move_forward_raw() {
    bool motor_started = false;
    // check for limit switch
    if (
        LimitSwitch_forward.getState() ==
        slight_ButtonInput::state_Standby
    ) {
        // check if current LimitSwitch is active!
        // drive forward
        motor_move_started_timestamp = millis();
        motor.moveTo(motor.forwardLimit);
        motor_started = true;
    }
    return motor_started;
}

bool slight_StepperManager::motor_move_reverse_raw() {
    bool motor_started = false;
    // check for limit switch
    if (
        LimitSwitch_reverse.getState() ==
        slight_ButtonInput::state_Standby
    ) {
        // drive reverse
        motor_move_started_timestamp = millis();
        motor.moveTo(motor.reverseLimit);
        motor_started = true;
    }
    return motor_started;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// system functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// this is the public interface to start calibration
void slight_StepperManager::calibration_start() {
    system_state = SYSSTATE_calibrating_start;
}

void slight_StepperManager::calibration_limit_threshold_set(
    uint16_t calibration_limit_threshold_new
) {
    calibration_limit_threshold = calibration_limit_threshold_new;
}

uint16_t slight_StepperManager::calibration_limit_threshold_get() {
    return calibration_limit_threshold;
}

// public emergency stop
void slight_StepperManager::system_emergencystop() {
    motor.stop();
    motor.disable();
    // something went wrong
    error_type = ERROR_emergencystop;
    system_error();
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// internal calibration things

void slight_StepperManager::system_state_calibrating_start() {
    // reset limits
    motor.forwardLimit = calibration_limit;
    motor.reverseLimit = -calibration_limit;
    // reset motor positon
    motor.setPos(0);
    // backup current speed
    // move_speed = motor.getMaxSpeed();
    // move_acceleration = motor.getAccel();
    // not needed anymore. move_speed is hold internaly.
    // set speed to slow
    motor.setMaxSpeed(calibration_speed);
    motor.setAccel(calibration_acceleration);
    // reset error
    error_type = ERROR_none;
    // reset direction checks
    calibration_direction_forward_done = false;
    calibration_direction_reverse_done = false;
    // start calibration mode
    system_state = SYSSTATE_calibrating_check_next;
}

void slight_StepperManager::system_state_calibrating_check_next() {
    // check if both directions are done
    if (
        calibration_direction_forward_done &&
        calibration_direction_reverse_done
    ) {
        system_state = SYSSTATE_calibrating_finished;
    } else {
        // check if forward is todo
        if (calibration_direction_forward_done == false) {
            // check if forward is possible (limit switch)
            if (
                LimitSwitch_forward.getState() ==
                slight_ButtonInput::state_Standby
            ) {
                system_state = SYSSTATE_calibrating_forward_start;
            } else {
                system_state = SYSSTATE_calibrating_reverse_start;
            }
        } else {
            // check if reverse is todo
            if (calibration_direction_reverse_done == false) {
                // check if reverse is possible (limit switch)
                if (
                    LimitSwitch_reverse.getState() ==
                    slight_ButtonInput::state_Standby
                ) {
                    system_state = SYSSTATE_calibrating_reverse_start;
                } else {
                    // system_state = SYSSTATE_calibrating_forward_start;
                    error_type = ERROR_limitswitchs;
                    system_error();
                }
            }
        }  // else check forward todo
    }  // else check if both directions are done
}

void slight_StepperManager::system_state_calibrating_global_checks() {
    // check if motor is stopped
    if (motor_move_state == 0) {
        // something went wrong
        error_type = ERROR_motorstop;
        system_error();
    } else {
        // check timeout
        uint32_t move_duration =
            millis() - motor_move_started_timestamp;
        if (move_duration > motor_move_timeout) {
            motor.stop();
            error_type = ERROR_timeout;
            system_error();
        }
    }
}


void slight_StepperManager::system_state_calibrating_forward_start() {
    // start calibration
    if (motor_move_forward_raw()) {
        system_state = SYSSTATE_calibrating_forward;
    } else {
        error_type = ERROR_motorstart;
        system_error();
    }
}

void slight_StepperManager::system_state_calibrating_forward_checks() {
    // check this occurences:
    // * limit switch activated
    // * motor stopped (without limmit switch --> software limit)
    // * timeout --> optional second safety feature.
    if (
        LimitSwitch_forward.getState() ==
        slight_ButtonInput::state_Active
    ) {
        motor.stop();
        system_state = SYSSTATE_calibrating_forward_finished;
    } else {
        system_state_calibrating_global_checks();
    }
    // this check is done in the down event handler.
    // so only new down events are fireing this error.
    // if (
    //     LimitSwitch_reverse.getState() ==
    //     slight_ButtonInput::state_Active
    // ) {
    //     motor.stop();
    //     system_error();
    // }
}

void slight_StepperManager::system_state_calibrating_forward_finished() {
    // set limit to current position
    motor.forwardLimit = motor.getPos();
    calibration_direction_forward_done = true;
    // system_state = system_state_calibrating_reverse_start;
    system_state = SYSSTATE_calibrating_check_next;
}

void slight_StepperManager::system_state_calibrating_reverse_start() {
    // start calibration reverse
    if (motor_move_reverse_raw()) {
        system_state = SYSSTATE_calibrating_reverse;
    } else {
        error_type = ERROR_motorstart;
        system_error();
    }
}

void slight_StepperManager::system_state_calibrating_reverse_checks() {
    // check this occurences:
    // * limit switch activated
    // * motor stopped (without limmit switch --> software limit)
    // * timeout --> optional second safety feature.
    if (
        LimitSwitch_reverse.getState() ==
        slight_ButtonInput::state_Active
    ) {
        motor.stop();
        system_state = SYSSTATE_calibrating_reverse_finished;
    } else {
        system_state_calibrating_global_checks();
    }
    // this check is done in the down event handler.
    // so only new down events are fireing this error.
    // if (
    //     LimitSwitch_forward.getState() ==
    //     slight_ButtonInput::state_Active
    // ) {
    //     motor.stop();
    //     system_error();
    // }
}

void slight_StepperManager::system_state_calibrating_reverse_finished() {
    // set limit to current position
    motor.reverseLimit = motor.getPos();
    calibration_direction_reverse_done = true;
    // calibration finished
    system_state = SYSSTATE_calibrating_check_next;
}

void slight_StepperManager::system_state_calibrating_finished() {
    // set speed to original
    motor.setMaxSpeed(move_speed);
    motor.setAccel(move_acceleration);
    // all fine now ;-)
    // system_state = SYSSTATE_hold;
    // check where we are.
    if (
        (LimitSwitch_forward.getState() ==
        slight_ButtonInput::state_Active)
    ) {
        // all fine :-)
        system_state = SYSSTATE_hold_forward;
    } else {
        if (
            (LimitSwitch_reverse.getState() ==
            slight_ButtonInput::state_Active)
        ) {
            // all fine :-)
            system_state = SYSSTATE_hold_reverse;
        }
    }
    // cali
    // final check
    // if (error_type == ERROR_none) {
    //     system_state = SYSSTATE_hold;
    // } else {
    //     error_type = ERROR_calibrating;
    //     system_error();
    // }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// internal state maschine

void slight_StepperManager::system_state_update() {
    // check if state has changed.
    if (system_state_last != system_state) {
        system_state_last = system_state;
        system_event_callback();
    }

    switch (system_state) {
        case SYSSTATE_notvalid: {
            // nothing to do.
        } break;
        case SYSSTATE_dirty: {
            // wait
        } break;
        case SYSSTATE_error: {
            // wait
        } break;
        case SYSSTATE_standby: {
            // nothing to do.
        } break;
        case SYSSTATE_hold_forward: {
            // nothing to do.
        } break;
        case SYSSTATE_hold_reverse: {
            // nothing to do.
        } break;
        case SYSSTATE_moving_forward: {
            // nothing to do.
        } break;
        case SYSSTATE_moving_reverse: {
            // nothing to do.
        } break;
        // calibrating
        case SYSSTATE_calibrating_start: {
            system_state_calibrating_start();
        } break;
        case SYSSTATE_calibrating_check_next: {
            system_state_calibrating_check_next();
        } break;
        case SYSSTATE_calibrating_forward_start: {
            system_state_calibrating_forward_start();
        } break;
        case SYSSTATE_calibrating_forward: {
            system_state_calibrating_forward_checks();
        } break;
        case SYSSTATE_calibrating_forward_finished: {
            system_state_calibrating_forward_finished();
        } break;
        case SYSSTATE_calibrating_reverse_start: {
            system_state_calibrating_reverse_start();
        } break;
        case SYSSTATE_calibrating_reverse: {
            system_state_calibrating_reverse_checks();
        } break;
        case SYSSTATE_calibrating_reverse_finished: {
            system_state_calibrating_reverse_finished();
        } break;
        case SYSSTATE_calibrating_finished: {
            system_state_calibrating_finished();
        } break;
    }
}


void slight_StepperManager::system_state_check_motor_state_change() {
    switch (system_state) {
        // only change system_state whene moving or standby.
        case SYSSTATE_standby:
        case SYSSTATE_hold_forward:
        case SYSSTATE_hold_reverse:
        case SYSSTATE_moving_forward:
        case SYSSTATE_moving_reverse: {
            switch (motor_move_state) {
                case 0: {  // stop
                    if (motor.isEnabled()) {
                        // check if one of the Limit Switchs is reached.
                        // if not thats an error.
                        if (
                            (LimitSwitch_forward.getState() ==
                            slight_ButtonInput::state_Active)
                        ) {
                            // all fine :-)
                            system_state = SYSSTATE_hold_forward;
                        } else {
                            if (
                                (LimitSwitch_reverse.getState() ==
                                slight_ButtonInput::state_Active)
                            ) {
                                // all fine :-)
                                system_state = SYSSTATE_hold_reverse;
                            } else {
                                // thats an error
                                error_type = ERROR_motorstop;
                                system_error();
                            }
                        }
                    } else {
                        system_state = SYSSTATE_standby;
                    }
                } break;
                case 1: {  // forward
                    system_state = SYSSTATE_moving_forward;
                } break;
                case -1: {  // reverse
                    system_state = SYSSTATE_moving_reverse;
                } break;
            }  // switch end
        } break;
        default: {
            //
        }
    }  // switch system_state
}

void slight_StepperManager::system_check_motor_timeout() {
    // check timeout
    uint32_t move_duration = millis() - motor_move_started_timestamp;
    if (move_duration > motor_move_timeout) {
        motor.stop();
        error_type = ERROR_timeout;
        system_error();
    }
}

void slight_StepperManager::system_error() {
    motor.disable();
    system_state = SYSSTATE_error;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public system state & error type helpers

slight_StepperManager::error_t slight_StepperManager::error_type_get() {
    return error_type;
}

void slight_StepperManager::print_error(Print &out, error_t error) {
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

void slight_StepperManager::print_error(Print &out) {
    print_error(out, error_type);
}

slight_StepperManager::sysstate_t slight_StepperManager::system_state_get() {
    return system_state;
}

void slight_StepperManager::print_state(Print &out, sysstate_t state) {
    switch (state) {
        case SYSSTATE_notvalid: {
            out.print(F("notvalid"));
        } break;
        case SYSSTATE_standby: {
            out.print(F("standby"));
        } break;
        case SYSSTATE_hold_forward: {
            out.print(F("hold forward"));
        } break;
        case SYSSTATE_hold_reverse: {
            out.print(F("hold reverse"));
        } break;
        case SYSSTATE_moving_forward: {
            out.print(F("moving forward"));
        } break;
        case SYSSTATE_moving_reverse: {
            out.print(F("moving reverse"));
        } break;
        case SYSSTATE_dirty: {
            out.print(F("dirty"));
        } break;
        case SYSSTATE_error: {
            out.print(F("error"));
        } break;
        case SYSSTATE_calibrating_check_next: {
            out.print(F("cali check next"));
        } break;
        case SYSSTATE_calibrating_start: {
            out.print(F("cali start"));
        } break;
        case SYSSTATE_calibrating_forward_start: {
            out.print(F("cali forward start"));
        } break;
        case SYSSTATE_calibrating_forward: {
            out.print(F("cali forward"));
        } break;
        case SYSSTATE_calibrating_forward_finished: {
            out.print(F("cali forward finished"));
        } break;
        case SYSSTATE_calibrating_reverse_start: {
            out.print(F("cali reverse start"));
        } break;
        case SYSSTATE_calibrating_reverse: {
            out.print(F("cali reverse"));
        } break;
        case SYSSTATE_calibrating_reverse_finished: {
            out.print(F("cali reverse finished"));
        } break;
        case SYSSTATE_calibrating_finished: {
            out.print(F("cali finished"));
        } break;
    }
}

void slight_StepperManager::print_state(Print &out) {
    print_state(out, system_state);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public speed setters / getters

void slight_StepperManager::move_speed_set(
    uint16_t stepsPerSec
) {
    move_speed = stepsPerSec;
    // motor.setMaxSpeed(move_speed);
}

uint16_t slight_StepperManager::move_speed_get() {
    return move_speed;
}

void slight_StepperManager::move_acceleration_set(
    uint16_t stepsPerSecPerSec
) {
    move_acceleration = stepsPerSecPerSec;
    // motor.setAccel(move_acceleration);
}

uint16_t slight_StepperManager::move_acceleration_get() {
    return move_acceleration;
}


void slight_StepperManager::calibration_speed_set(
    uint16_t stepsPerSec
) {
    calibration_speed = stepsPerSec;
    // motor.setMaxSpeed(calibration_speed);
}

uint16_t slight_StepperManager::calibration_speed_get() {
    return calibration_speed;
}

void slight_StepperManager::calibration_acceleration_set(
    uint16_t stepsPerSecPerSec
) {
    calibration_acceleration = stepsPerSecPerSec;
    // motor.setAccel(calibration_acceleration);
}

uint16_t slight_StepperManager::calibration_acceleration_get() {
    return calibration_acceleration;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// public event setters

void slight_StepperManager::motor_move_event_set_callback(
    callback_t callback_function
) {
    callback_move_event = callback_function;
}

void slight_StepperManager::motor_acceleration_event_set_callback(
    callback_t callback_function
) {
    callback_accelleration_event = callback_function;
}

void slight_StepperManager::motor_enable_event_set_callback(
    callback_t callback_function
) {
    callback_enable_event = callback_function;
}

void slight_StepperManager::system_event_set_callback(
    callback_t callback_function
) {
    callback_system_event = callback_function;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// internal event callers

void slight_StepperManager::system_event_callback() {
    if (callback_system_event) {
        callback_system_event();
    }
}

// };  // namespace slight_StepperManager
