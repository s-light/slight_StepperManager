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

#include "motor_system_interface.h"
#include "motor_system_config.h"

#include <kissStepper.h>
#include <slight_ButtonInput.h>

namespace motor_system {

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // prototyps
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void init();
    void update();

    // callback function
    void motor_move_event_set_callback(callback_t);
    void motor_acceleration_event_set_callback(callback_t);
    void motor_enable_event_set_callback(callback_t);
    void system_event_set_callback(callback_t);
    void system_event_callback();

    void motor_init(Print &out);

    void motor_print_mode(Print &out, uint8_t mode);
    bool motor_move_forward();
    bool motor_move_reverse();
    bool motor_isEnabled();
    bool motor_set_enable(bool);
    void system_start_calibration();
    void system_state_update();

    void print_state(Print&, sysstate_t);
    void print_error(Print&, error_t);

    boolean LimitSwitch_getInput(uint8_t id, uint8_t pin);
    void LimitSwitch_onEvent(slight_ButtonInput *pInstance, uint8_t event);
    void LimitSwitchs_init();
    void LimitSwitchs_update();


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // define & initialise
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    callback_t callback_move_event;
    callback_t callback_accelleration_event;
    callback_t callback_enable_event;
    callback_t callback_system_event;

    // enum class sysstate_t : uint8_t {      // c++ typesafe; arduino > 1.6.
    //     notvalid =             0;
    //     standby =              1;
    //     needscalibration =     2;
    //     calibrating =          10;
    //     calibrating_forward =  11;
    //     calibrating_reverse =  12;
    // };
    // enum sysstate_t {  // c
    //     SYSSTATE_notvalid =            0,
    //     SYSSTATE_standby =             1,
    //     SYSSTATE_dirty =               2,
    //     SYSSTATE_calibrating_forward = 10,
    //     SYSSTATE_calibrating_forward_finished = 11,
    //     SYSSTATE_calibrating_reverse = 12,
    //     SYSSTATE_calibrating_reverse_finished = 13,
    // }
    // const uint8_t SYSSTATE_error =                          3;
    // const uint8_t SYSSTATE_calibrating =                    10;
    // const uint8_t SYSSTATE_calibrating_forward =            11;
    // const uint8_t SYSSTATE_calibrating_forward_finished =   12;
    // const uint8_t SYSSTATE_calibrating_reverse =            13;

    error_t error_type = ERROR_none;

    sysstate_t system_state = SYSSTATE_dirty;

    uint32_t motor_move_started_timestamp = millis();

    int8_t motor_accel_state = 0;
    int8_t motor_move_state = 0;
    int8_t motor_isenabled = 0;

    // instantiate the kissStepper
    kissStepper motor(
        motor_pinAssignments,
        motor_microstepConfig
    );


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // implementation
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // LimitSwitchs

    // const uint16_t LimitSwitch_duration_Debounce      =   10;
    // const uint16_t LimitSwitch_duration_HoldingDown   = 9000;
    // const uint16_t LimitSwitch_duration_ClickSingle   =   30;
    // const uint16_t LimitSwitch_duration_ClickLong     = 9000;
    // const uint16_t LimitSwitch_duration_ClickDouble   =  200;

    // const uint8_t LimitSwitchs_COUNT = 2;

    slight_ButtonInput LimitSwitchs[LimitSwitchs_COUNT] = {
        slight_ButtonInput(
            LimitSwitch_forward,
            MOSI,
            LimitSwitch_getInput,
            LimitSwitch_onEvent,
            LimitSwitch_duration_Debounce,
            LimitSwitch_duration_HoldingDown,
            LimitSwitch_duration_ClickSingle,
            LimitSwitch_duration_ClickLong,
            LimitSwitch_duration_ClickDouble
        ),
        slight_ButtonInput(
            LimitSwitch_reverse,
            MISO,
            LimitSwitch_getInput,
            LimitSwitch_onEvent,
            LimitSwitch_duration_Debounce,
            LimitSwitch_duration_HoldingDown,
            LimitSwitch_duration_ClickSingle,
            LimitSwitch_duration_ClickLong,
            LimitSwitch_duration_ClickDouble
        )
    };

    void LimitSwitchs_init() {
        for (size_t index = 0; index < LimitSwitchs_COUNT; index++) {
            pinMode(LimitSwitchs[index].getPin(), INPUT_PULLUP);
            LimitSwitchs[index].begin();
        }
    }

    void LimitSwitchs_update() {
        for (size_t index = 0; index < LimitSwitchs_COUNT; index++) {
            LimitSwitchs[index].update();
        }
    }

    boolean LimitSwitch_getInput(uint8_t id, uint8_t pin) {
        // read input invert reading - button closes to GND.
        // check HWB
        // return ! (PINE & B00000100);
        return !digitalRead(pin);
    }

    void LimitSwitch_onEvent(slight_ButtonInput *pInstance, uint8_t event) {
        // Serial.print(F("Instance ID:"));
        // Serial.println((*pInstance).getID());
        // Serial.print(F("Event: "));
        // (*pInstance).printEvent(Serial, event);
        // Serial.println();

        uint8_t limitswitch_id = (*pInstance).getID();

        // show event additional infos:
        switch (event) {
            // case slight_ButtonInput::event_StateChanged : {
            //     Serial.print(F("\t state: "));
            //     (*pInstance).printState(Serial);
            //     Serial.println();
            // } break;
            case slight_ButtonInput::event_Down : {
                // Serial.println(F("button pressed down!"));
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
                                system_state = SYSSTATE_error_new;
                                error_type = ERROR_limitswitch_wrong_dir;
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
                                system_state = SYSSTATE_error_new;
                                error_type = ERROR_limitswitch_wrong_dir;
                            } break;
                            // case SYSSTATE_calibrating_reverse: {
                            //     // set limit to current position
                            //     motor.reverseLimit = motor.getPos();
                            // } break;
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
            //     Serial.print(F("duration active: "));
            //     Serial.println((*pInstance).getDurationActive());
            // } break;
            // case slight_ButtonInput::event_Up : {
            //     Serial.println(F("up"));
            // } break;
            // case slight_ButtonInput::event_Click : {
            //     // Serial.println(F("click"));
            //     // Serial.print(F("limitswitch_id: "));
            //     // Serial.println(limitswitch_id);
            //     switch (limitswitch_id) {
            //         case 0: {
            //         }break;
            //         case 1: {
            //         }break;
            //     }  // end switch limitswitch_id
            // } break;
            // case slight_ButtonInput::event_ClickLong : {
            //     // Serial.println(F("click long"));
            // } break;
            // case slight_ButtonInput::event_ClickDouble : {
            //     Serial.println(F("click double"));
            // } break;
            // case slight_ButtonInput::event_ClickTriple : {
            //     Serial.println(F("click triple"));
            // } break;
            // case slight_ButtonInput::event_ClickMulti : {
            //     Serial.print(F("click count: "));
            //     Serial.println((*pInstance).getClickCount());
            // } break;
        }  // end switch
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // motor

    void motor_init(Print &out) {
        out.println(F("setup stepper motor:"));
        // init motor
        out.println(F("\t set:"));
        out.println(F("\t     MICROSTEP_8"));
        out.println(F("\t     max speed 200 steps/s"));
        out.println(F("\t     acceleration 200 steps/s^2"));
        motor.begin(MICROSTEP_8, 200, 200);

        out.println(F("\t     limits:"));
        int32_t limit = 12800;
        motor.forwardLimit = limit;
        motor.reverseLimit = -limit;
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

    void motor_print_mode(Print &out, uint8_t mode) {
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


    void motor_check_event() {
        int8_t new_move_state = motor.getMoveState();
        if (new_move_state != motor_move_state) {
            // event!
            motor_move_state = new_move_state;
            if (callback_move_event) {
                callback_move_event();
            }
            // display_motorstate_update();
        }
        int8_t new_accel_state = motor.getAccelState();
        if (new_accel_state != motor_accel_state) {
            // event!
            motor_accel_state = new_accel_state;
            if (callback_accelleration_event) {
                callback_accelleration_event();
            }
            // display_motoraccel_update();
        }
        int8_t new_isenabled = motor.isEnabled();
        if (new_isenabled != motor_isenabled) {
            // event!
            motor_isenabled = new_isenabled;
            if (callback_enable_event) {
                callback_enable_event();
            }
            // display_motorenabled_update();
        }
    }


    bool motor_move_forward() {
        bool motor_started = false;
        // check for limit switch
        if (
            LimitSwitchs[LimitSwitch_forward].getState() ==
            slight_ButtonInput::state_Standby
        ) {
            // drive forward
            motor_move_started_timestamp = millis();
            motor.moveTo(motor.forwardLimit);
            motor_started = true;
        }
        return motor_started;
    }

    bool motor_move_reverse() {
        bool motor_started = false;
        // check for limit switch
        if (
            LimitSwitchs[LimitSwitch_reverse].getState() ==
            slight_ButtonInput::state_Standby
        ) {
            // drive reverse
            motor_move_started_timestamp = millis();
            motor.moveTo(motor.reverseLimit);
            motor_started = true;
        }
        return motor_started;
    }

    // bool motor_set_enable(bool enable = void) {
    bool motor_set_enable(bool enable) {
        // if (enable != void) {
        if (enable) {
            motor.enable();
        } else {
            motor.disable();
        }
        // }
        return motor.isEnabled();
    }

    bool motor_isEnabled() {
        return motor.isEnabled();
    }

    void system_start_calibration() {
        system_state = SYSSTATE_calibrating_start;
    }

    void system_state_calibrating_global_checks() {
        // check if motor is stopped
        if (motor_move_state == 0) {
            // something went wrong
            system_state = SYSSTATE_error_new;
            error_type = ERROR_motorstop;
        } else {
            // check timeout
            uint32_t move_duration =
                millis() - motor_move_started_timestamp;
            if (move_duration > motor_move_timeout) {
                system_state = SYSSTATE_error_new;
                error_type = ERROR_timeout;
            }
        }
    }

    void system_state_calibrating_forward_checks() {
        // check this occurences:
        // * limit switch activated
        // * motor stopped (without limmit switch --> software limit)
        // * timeout --> optional second safety feature.
        if (
            LimitSwitchs[LimitSwitch_forward].getState() ==
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
        //     LimitSwitchs[LimitSwitch_reverse].getState() ==
        //     slight_ButtonInput::state_Active
        // ) {
        //     motor.stop();
        //     system_state = SYSSTATE_error;
        // }

    }

    void system_state_calibrating_reverse_checks() {
        // check this occurences:
        // * limit switch activated
        // * motor stopped (without limmit switch --> software limit)
        // * timeout --> optional second safety feature.
        if (
            LimitSwitchs[LimitSwitch_reverse].getState() ==
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
        //     LimitSwitchs[LimitSwitch_forward].getState() ==
        //     slight_ButtonInput::state_Active
        // ) {
        //     motor.stop();
        //     system_state = SYSSTATE_error;
        // }
    }


    void system_state_update() {
        // TODO(s-light): implement calibration state mashine
        switch (system_state) {
            case SYSSTATE_notvalid: {
                // nothing to do.
            } break;
            case SYSSTATE_standby: {
                // nothing to do.
            } break;
            case SYSSTATE_moving: {
                // nothing to do.
            } break;
            case SYSSTATE_dirty_new: {
                // generate event to master so that user can be informed
                system_state = SYSSTATE_dirty;
                system_event_callback();
            } break;
            case SYSSTATE_dirty: {
                // generate event to master so that user can be informed
            } break;
            case SYSSTATE_error_new: {
                // generate event to master so that user can be informed
                system_state = SYSSTATE_error;
                system_event_callback();
            } break;
            case SYSSTATE_error: {
                // generate event to master so that user can be informed
            } break;
            case SYSSTATE_calibrating_start: {
                // reset limits
                motor.forwardLimit = calibration_limit;
                motor.reverseLimit = -calibration_limit;
                // reset error
                error_type = ERROR_none;
                // inform master of calibration start
                system_event_callback();
                // start calibration
                if (motor_move_forward()) {
                    system_state = SYSSTATE_calibrating_forward;
                    system_event_callback();
                } else {
                    system_state = SYSSTATE_error_new;
                    error_type = ERROR_motorstart;
                }
            } break;
            case SYSSTATE_calibrating_forward: {
                system_state_calibrating_forward_checks();
            } break;
            case SYSSTATE_calibrating_forward_finished: {
                // set limit to current position
                motor.forwardLimit = motor.getPos();
                // start calibration reverse
                if (motor_move_reverse()) {
                    system_state = SYSSTATE_calibrating_reverse;
                    system_event_callback();
                } else {
                    system_state = SYSSTATE_error_new;
                    error_type = ERROR_motorstart;
                }
            } break;
            case SYSSTATE_calibrating_reverse: {
                system_state_calibrating_reverse_checks();
            } break;
            case SYSSTATE_calibrating_reverse_finished: {
                // set limit to current position
                motor.forwardLimit = motor.getPos();
                // calibration finished
                system_state = SYSSTATE_calibrating_finished;
            } break;
            case SYSSTATE_calibrating_finished: {
                // set limit to current position
                system_event_callback();
                system_state = SYSSTATE_standby;
                // cali
                // final check
                // if (error_type == ERROR_none) {
                //     system_state = SYSSTATE_standby;
                // } else {
                //     system_state = SYSSTATE_error_new;
                //     error_type = ERROR_calibrating;
                // }
            } break;
        }
    }


    void print_error(Print &out, error_t error) {
        switch (error) {
            case ERROR_none: {
                out.print(F("none"));
            } break;
            case ERROR_timeout: {
                out.print(F("timeout"));
            } break;
            case ERROR_limitswitch_wrong_dir: {
                out.print(F("LiSw wrong dir"));
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
        }
    }

    void print_state(Print &out, sysstate_t state) {
        switch (state) {
            case SYSSTATE_notvalid: {
                out.print(F("notvalid"));
            } break;
            case SYSSTATE_standby: {
                out.print(F("standby"));
            } break;
            case SYSSTATE_moving: {
                out.print(F("moving"));
            } break;
            case SYSSTATE_dirty_new: {
                out.print(F("dirty new"));
            } break;
            case SYSSTATE_dirty: {
                out.print(F("dirty"));
            } break;
            case SYSSTATE_error_new: {
                out.print(F("error new"));
            } break;
            case SYSSTATE_error: {
                out.print(F("error"));
            } break;
            case SYSSTATE_calibrating_start: {
                out.print(F("cali start"));
            } break;
            case SYSSTATE_calibrating_forward: {
                out.print(F("cali forward"));
            } break;
            case SYSSTATE_calibrating_forward_finished: {
                out.print(F("cali forward finished"));
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


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // motor_system
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void init() {
        // init all subparts
        LimitSwitchs_init();
        motor_init(Serial);
    }

    void update() {
        // update all sub parts
        LimitSwitchs_update();
        motor.work();
        motor_check_event();
        system_state_update();
    }

    void motor_move_event_set_callback(callback_t callback_function) {
        callback_move_event = callback_function;
    }
    void motor_acceleration_event_set_callback(callback_t callback_function) {
        callback_accelleration_event = callback_function;
    }
    void motor_enable_event_set_callback(callback_t callback_function) {
        callback_enable_event = callback_function;
    }
    void system_event_set_callback(callback_t callback_function) {
        callback_system_event = callback_function;
    }
    void system_event_callback() {
        if (callback_system_event) {
            callback_system_event();
        }
    }

};  // namespace motor_system
