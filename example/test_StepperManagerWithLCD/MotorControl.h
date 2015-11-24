/******************************************************************************

    contains all functionality to control a Stepper Motor with LimitSwitchs
    that moves a mechanics from one end-position to the other end-position.

    idea:
        on power up calibrat your system once.
        than just give a command for one of the two target positions:
        * motor_move_forward();
        * motor_move_reverse();

        if something goes wrong the system stops the motor and
        disables the Stepper Driver. (motor is in free-run = no force)
        you can reset your system with recalibration.

    Input
        * LimitSwitch_forward
        * LimitSwitch_reverse
    Output
        * steps
        * direction
        * enable
        * microstep config 1
        * microstep config 2
        * microstep config 3

	libraries used:
        ~ Keep it Simple Stepper (kissStepper)
            Written by Rylee Isitt. September 21, 2015
            https://github.com/risitt/kissStepper
            License: GNU Lesser General Public License (LGPL) V2.1
        ~ slight_StepperManager
        ~ slight_ButtonInput
            written by stefan krueger (s-light),
                github@s-light.eu, http://s-light.eu, https://github.com/s-light/
            License: cc by sa, Apache License Version 2.0, MIT

    written by stefan krueger (s-light),
        github@s-light.eu, http://s-light.eu, https://github.com/s-light/

	changelog / history
        see git commit messages

	TO DO:
		~ enjoy your life ;-)

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


#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

/** Includes Core Arduino functionality **/
#if ARDUINO
    #if ARDUINO < 100
        #include <WProgram.h>
    #else
        #include <Arduino.h>
    #endif
#endif


#include <kissStepper.h>

#include <slight_StepperManager.h>
// #include <slight_ButtonInput.h>

// http://www.cplusplus.com/forum/articles/10627/
// forward declared dependencies
class slight_FaderLin;
class slight_ButtonInput;

// class MotorControl {
namespace MotorControl {

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // public functions
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void init ();
    void update ();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // public attributes
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    extern slight_StepperManager myStepperManager;
    extern kissStepper myStepperMotor;
};

#endif // MOTORCONTROL_H
