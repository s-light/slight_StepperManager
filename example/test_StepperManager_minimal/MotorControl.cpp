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

    // Pins for Watterott SilentStepStick and Genuino micro on breadboard
    // debug full setup
    kissPinAssignments motor_pinAssignments(
         7,  // pinDir
         8,  // pinStep
        12  // pinEnable
        // 11,  // pinMS1
        // 10,  // pinMS2
        //  9   // pinMS3
    );
    // if config pins are unconnected board defaults to
    // 16uSteps    interpolated to 256 μsteps    stealthChop (quite run)

    // kissMicrostepConfig for Trinamic TMC2100
    // http://www.trinamic.com/products/integrated-circuits/stepper-power-driver/tmc2100
    // 3.1 CFG Pin Configuration (Page 9)
    // Table    1.    Stepping    Format
    //     CFG2 CFG1 μSTEP INTERPOLATION    MODE            kiss Mode
    //     0    0     1    N                spreadCycle     FULL_STEP
    //     0    1     2    N                spreadCycle     HALF_STEP
    //     1    Z     2    Y, 256 μsteps    spreadCycle     -
    //     1    0     4    N                spreadCycle     MICROSTEP_4
    //     1    1    16    N                spreadCycle     MICROSTEP_16
    //     1    Z     4    Y, 256 μsteps    spreadCycle     -
    //     Z    0    16    Y, 256 μsteps    spreadCycle     -
    //     Z    1     4    Y, 256 μsteps    stealthChop     -
    //     Z    Z    16    Y, 256 μsteps    stealthChop     -
    // Mapping to kissMicrostepConfig:
    // mode         F  H  μ  μ  μ  μ  μ  μ
    //              U  A  S  S  S  S  S  S
    //              L  L  t  t  t  t  t  t
    //              L  F  e  e  e  e  e  e
    //              S  S  p  p  p  p  p  p
    //              T  T                 1
    //              E  E        1  3  6  2
    //              P  P  4  8  6  2  4  8
    // MS1Config    0  1  0  0  1  0  0  0
    // MS2Config    0  0  1  0  1  0  0  0
    // MS3Config    0  0  0  0  0  0  0  0

    kissMicrostepConfig motor_microstepConfig(
        MICROSTEP_16,
        B01001000,  // MS1Config
        B00101000,  // MS2Config
        B00000000   // MS3Config
    );
    const uint8_t motor_max_microsteps_factor = 16;

    // instantiate the kissStepper_TriState
    kissStepper_TriState myStepperMotor(
        motor_pinAssignments,
        motor_microstepConfig
    );

    // number of full steps in one revolution of the motor
    const uint16_t motor_full_steps_revolution = 200;

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
            SCK,
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

    // uint16_t calibration_limit_threshold = motor_max_microsteps_factor;
    // uint16_t calibration_speed = 50;
    // uint16_t calibration_acceleration = 25;
    // uint16_t move_speed = 200;
    // uint16_t move_acceleration = 50;
    //
    // slight_StepperManager myStepperManager(
    //     myStepperMotor,
    //     LimitSwitchs[LimitSwitch_forward],
    //     LimitSwitchs[LimitSwitch_reverse],
    //     motor_move_timeout,
    //     calibration_limit,
    //     calibration_speed,
    //     calibration_acceleration,
    //     move_speed,
    //     move_acceleration
    // );

    slight_StepperManager myStepperManager(
        // kissStepper &motor_ref,
        myStepperMotor,
        // slight_ButtonInput &LimitSwitch_forward_ref,
        LimitSwitchs[LimitSwitch_forward],
        // slight_ButtonInput &LimitSwitch_reverse_ref,
        LimitSwitchs[LimitSwitch_reverse],
        // const uint16_t motor_move_timeout_new = 5000,
        motor_move_timeout,
        // const int32_t calibration_limit_new = 12800,
        calibration_limit,
        // uint16_t calibration_speed_new = 50,
        50,
        // uint16_t calibration_acceleration_new = 25,
        25,
        // uint16_t move_speed_new = 200,
        200,
        // uint16_t move_acceleration_new = 50
        50
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

    void init(Print &out) {
        LimitSwitchs_init();
        myStepperManager.init(out);
    }

    void update() {
        LimitSwitchs_update();
        myStepperManager.update();
    }



}  // namespace MotorControl
