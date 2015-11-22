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

#ifndef SLIGHT_STEPPERMANAGER_CONFIG_H_
#define SLIGHT_STEPPERMANAGER_CONFIG_H_

#include <kissStepper.h>

namespace slight_StepperManager {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // LimitSwitchs

    const uint16_t LimitSwitch_duration_Debounce      =   10;
    const uint16_t LimitSwitch_duration_HoldingDown   = 9000;
    const uint16_t LimitSwitch_duration_ClickSingle   =   30;
    const uint16_t LimitSwitch_duration_ClickLong     = 9000;
    const uint16_t LimitSwitch_duration_ClickDouble   =  200;

    const uint8_t LimitSwitch_forward = 0;
    const uint8_t LimitSwitch_reverse = 1;

    const uint8_t LimitSwitchs_COUNT = 2;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // stepper motor

    // number of full steps in one revolution of the motor
    const uint16_t motor_full_steps_revolution = 200;

    // maximal amount of time (milliseconds) motor is allowed to continusly run
    const uint16_t motor_move_timeout = 5000;

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

    // set limit to what calibration will drive as min and max values:
    // 32* turns
    const int32_t calibration_limit_turns = 2;
    const int32_t calibration_limit =
        32 *
        motor_full_steps_revolution *
        calibration_limit_turns;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // xxx



};  // end namespace slight_StepperManager

#endif  // SLIGHT_STEPPERMANAGER_CONFIG_H_
