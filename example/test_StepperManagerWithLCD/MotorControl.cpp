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

/** Includes Core Arduino functionality **/
#if ARDUINO
    #if ARDUINO < 100
        #include <WProgram.h>
    #else
        #include <Arduino.h>
    #endif
#endif

#include "MotorControl.h"


#include <kissStepper.h>

#include <slight_StepperManager.h>
#include <slight_ButtonInput.h>


namespace MotorControl {

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// StepperManager
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stepper Motor

    // Pins for DRV8825 and Genuino micro on breadboard
    kissPinAssignments motor_pinAssignments(
         7,  // pinDir
         8,  // pinStep
        12,  // pinEnable
        11,  // pinMS1
        10,  // pinMS2
         9   // pinMS3
    );

    // kissMicrostepConfig for TI DRV8825
    // http://www.ti.com/product/drv8825?qgpn=drv8825
    // 8.3.5 Microstepping Indexer
    // Table    1.    Stepping    Format
    //     MODE2 MODE1 MODE0 STEP MODE
    //     0     0     0     Full step (2-phase excitation) with 71% current
    //     0     0     1     1/2  step (1-2 phase excitation)
    //     0     1     0     1/4  step (W1-2 phase excitation)
    //     0     1     1      8   microsteps/step
    //     1     0     0     16   microsteps/step
    //     1     0     1     32   microsteps/step
    //     1     1     0     32   microsteps/step
    //     1     1     1     32   microsteps/step
    kissMicrostepConfig motor_microstepConfig(
        MICROSTEP_32,
        B01010101,  // MS1Config
        B00110011,  // MS2Config
        B00001111   // MS3Config
    );

    // instantiate the kissStepper
    kissStepper myStepperMotor(
        motor_pinAssignments,
        motor_microstepConfig
    );

    // number of full steps in one revolution of the motor
    const uint16_t motor_full_steps_revolution = 200;
    const uint8_t motor_max_microsteps_factor = 32;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Limit Switchs

    const uint16_t LimitSwitch_duration_Debounce      =   10;
    const uint16_t LimitSwitch_duration_HoldingDown   = 9000;
    const uint16_t LimitSwitch_duration_ClickSingle   =   30;
    const uint16_t LimitSwitch_duration_ClickLong     = 9000;
    const uint16_t LimitSwitch_duration_ClickDouble   =  200;

    const uint8_t LimitSwitch_forward = 0;
    const uint8_t LimitSwitch_reverse = 1;

    const uint8_t LimitSwitchs_COUNT = 2;

    boolean LimitSwitch_getInput(uint8_t id, uint8_t pin);
    void LimitSwitch_onEvent(slight_ButtonInput *pInstance, uint8_t event);

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

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stepper Manager

    // maximal amount of time (milliseconds) motor is allowed to continusly run
    const uint16_t motor_move_timeout = 8000;

    // set limit to what calibration will drive as min and max values:
    const int8_t calibration_limit_turns = 3;
    const int32_t calibration_limit =
        motor_max_microsteps_factor *
        motor_full_steps_revolution *
        calibration_limit_turns;
    const int16_t calibration_speed = 100;
    uint16_t calibration_limit_threshold = motor_max_microsteps_factor;


    slight_StepperManager myStepperManager(
        myStepperMotor,
        LimitSwitchs[LimitSwitch_forward],
        LimitSwitchs[LimitSwitch_reverse],
        motor_move_timeout,
        calibration_limit,
        calibration_speed,
        calibration_limit_threshold
    );

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// impementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// StepperManager
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stepper Motor
    // all handled inside of StepperManager

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Limit Switchs
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
        myStepperManager.LimitSwitch_onEvent(pInstance, event);
    }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stepper Manager

    void init() {
        LimitSwitchs_init();
        myStepperManager.init();
    }

    void update() {
        LimitSwitchs_update();
        myStepperManager.update();
    }



}  // namespace MotorControl
