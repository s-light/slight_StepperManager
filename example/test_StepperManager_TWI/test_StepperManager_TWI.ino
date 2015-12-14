 /******************************************************************************

    test__StepperManager_TWI
        TWI controlled StepperMotor
        debugout on usbserial interface: 115200baud

    hardware:
        Board:
            Arduino Leonardo (ATmega32U4)
            or
            Arduino UNO (ATmega328p)
            compatible
        motordriver:
            Pololu DRV8825 Stepper Motor Driver Carrier, High Current
            or
            Watterott SilentStepStick (Trinamic TMC2100)

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
        other information in the git commit messages
        18.11.2015 16:30 created
            (based on DebugMenu_Simple.ino & test_kissStepper.ino)
        01.12.2015 19:08 created
            (based on test_StepperManagerWithLCD.ino)
        10.12.2015 17:07 created
            (based on test_StepperManager_simple.ino)

    TO DO:
        ~ implement thinks :-)
        ~ enjoy the sun


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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Includes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// use "file.h" for files in same directory as .ino
// #include "file.h"
// use <file.h> for files in library directory
// #include <file.h>

#include <slight_DebugMenu.h>
#include <slight_ButtonInput.h>

#include <kissStepper.h>
#include <kissStepper_TriState.h>

#include <slight_StepperManager.h>
#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;

#include <slight_StepperManager_TWI.h>
typedef slight_StepperManager_TWI StM_TWI;
// using StM_TWI = slight_StepperManager_TWI;
#include <Wire.h>
#include <slight_StepperManager_TWI_Controller.h>
typedef slight_StepperManager_TWI_Controller StM_TWI_Con;
// using StM_TWI_Con = slight_StepperManager_TWI_Controller;

#include "MotorControl.h"
// this includes the namespace 'MotorControl'
// create alias for namespace
namespace MoCon = MotorControl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Info
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void sketchinfo_print(Print &out) {
    out.println();
    //             "|~~~~~~~~~|~~~~~~~~~|~~~..~~~|~~~~~~~~~|~~~~~~~~~|"
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|                       ^ ^                      |"));
    out.println(F("|                      (0,0)                     |"));
    out.println(F("|                      ( _ )                     |"));
    out.println(F("|                       \" \"                      |"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("| test__StepperManager_TWI.ino"));
    out.println(F("|   TWI slave for Stepper Control "));
    out.println(F("|"));
    out.println(F("| This Sketch has a debug-menu:"));
    out.println(F("| send '?'+Return for help"));
    out.println(F("|"));
    out.println(F("| dream on & have fun :-)"));
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println(F("|"));
    //out.println(F("| Version: Nov 11 2013  20:35:04"));
    out.print(F("| version: "));
    out.print(F(__DATE__));
    out.print(F("  "));
    out.print(F(__TIME__));
    out.println();
    out.println(F("|"));
    out.println(F("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    out.println();

    //out.println(__DATE__); Nov 11 2013
    //out.println(__TIME__); 20:35:04
}


// Serial.print to Flash: Notepad++ Replace RegEx
//     Find what:        Serial.print(.*)\("(.*)"\);
//     Replace with:    Serial.print\1\(F\("\2"\)\);



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// definitions (global)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Debug Output


const bool led_on = LOW;
const bool led_off = HIGH;

bool infoled_state = 0;
const byte led_info_pin = 13;

const uint8_t led_moving_pin = 6;
const uint8_t led_error_pin = A3;

unsigned long debugOut_LiveSign_TimeStamp_LastAction = 0;
const uint16_t debugOut_LiveSign_UpdateInterval = 1000; //ms

bool debugOut_LiveSign_Serial_Enabled = 0;
bool debugOut_LiveSign_LED_Enabled = 1;
bool debugOut_printDisplay_onChange = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu

// slight_DebugMenu(Stream &in_ref, Print &out_ref, uint8_t input_length_new);
slight_DebugMenu myDebugMenu(Serial, Serial, 15);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// functions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// debug things

// freeRam found at
// http://forum.arduino.cc/index.php?topic=183790.msg1362282#msg1362282
// posted by mrburnette
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu System

// Modes for Menu Switcher
const uint8_t menu_mode__MenuMain = 1;
const uint8_t menu_mode__MenuMotor = 2;
uint8_t menu_mode = menu_mode__MenuMain;

// SubMenu SetValues
void menu_handle_Motor(slight_DebugMenu *pInstance) {
    Print &out = pInstance->get_stream_out_ref();
    char *command = pInstance->get_command_current_pointer();
    // out.print("sub1 command: '");
    // out.print(command);
    // out.println("'");
    switch (command[0]) {
        case '?': {
            // help
            out.println(F("____________________________________________________________"));
            out.println();
            out.println(F("Help for Motor Commands:"));
            out.println();
            out.println(F("\t '?': this help"));
            out.println(F("\t 'x': exit SubMenu"));
            out.println();
            out.println(F("\t 'a': motor set acceleration 'a500'"));
            out.println(F("\t 'A': motor get acceleration"));
            out.println(F("\t 's': motor set MaxSpeed 's500'"));
            out.println(F("\t 'S': motor get MaxSpeed"));
            out.println(F("\t 'C': motor get current speed"));
            out.println();
            out.println(F("\t 'o': motor set mode"));
            out.println(F("\t 'O': motor get mode"));
            out.println(F("\t 'e': motor enable  'e1'"));
            out.println(F("\t 'e': motor disable 'e0'"));
            out.println(F("\t 'E': motor isEnabled?"));
            out.println(F("\t 'u': motor use standby 'u1'"));
            out.println(F("\t 'U': motor use standby ?"));
            out.println();
            out.println(F("\t 'm': motor moveTo 'm100000'"));
            out.println(F("\t 'M': motor get target"));
            out.println(F("\t 'p': motor set position 'p0'"));
            out.println(F("\t 'P': motor get position"));
            out.println(F("\t 'l': motor set limits 'l6400,-6400'"));
            out.println(F("\t 'L': motor get limits"));
            out.println();
            out.println(F("\t 'D': motor stop"));
            out.println(F("\t 'd': motor decelerate"));
            out.println(F("\t 'r': motor run forward"));
            out.println(F("\t 'R': motor run reverse"));
            out.println();
            out.println(F("____________________________________________________________"));
        } break;
        //---------------------------------------------------------------------
        case 'x': {
            out.println(F("exit Menu Motor."));
            menu_mode = menu_mode__MenuMain;
        } break;
        //---------------------------------------------------------------------
        case 'a': {
            out.print(F("\t set motor acceleration "));
            uint16_t acceleration = atoi(&command[1]);
            out.print(acceleration);
            MoCon::myStepperMotor.setAccel(acceleration);
            out.println();
        } break;
        case 'A': {
            out.print(F("\t motor get acceleration:"));
            uint16_t acceleration;
            acceleration = MoCon::myStepperMotor.getAccel();
            out.print(acceleration);
            out.println();
        } break;
        case 's': {
            out.print(F("\t motor set MaxSpeed "));
            uint16_t speed = atoi(&command[1]);
            out.print(speed);
            MoCon::myStepperMotor.setMaxSpeed(speed);
            out.println();
        } break;
        case 'S': {
            out.print(F("\t motor get MaxSpeed:"));
            uint16_t speed;
            speed = MoCon::myStepperMotor.getMaxSpeed();
            out.print(speed);
            out.println();
        } break;
        case 'C': {
            out.print(F("\t motor get current speed:"));
            uint16_t speed;
            speed = MoCon::myStepperMotor.getCurSpeed();
            out.print(speed);
            out.println();
        } break;
        //------------------------------------------
        case 'o': {
            out.print(F("\t motor set mode "));
            uint16_t mode = atoi(&command[1]);
            bool flag_value_valid = false;
            switch (mode) {
                case 1: {
                    MoCon::myStepperMotor.setDriveMode(FULL_STEP);
                    flag_value_valid = true;
                } break;
                case 2: {
                    MoCon::myStepperMotor.setDriveMode(HALF_STEP);
                    flag_value_valid = true;
                } break;
                case 4: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_4);
                    flag_value_valid = true;
                } break;
                case 8: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_8);
                    flag_value_valid = true;
                } break;
                case 16: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_16);
                    flag_value_valid = true;
                } break;
                case 32: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_32);
                    flag_value_valid = true;
                } break;
                case 64: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_64);
                    flag_value_valid = true;
                } break;
                case 128: {
                    MoCon::myStepperMotor.setDriveMode(MICROSTEP_128);
                    flag_value_valid = true;
                } break;
                default: {
                    out.print(F(" possibel values: [1, 2, 4, 8, 16, 32, 64, 128]"));
                }
            }
            if(flag_value_valid) {
                MoCon::myStepperManager.motor_print_mode(out, mode);
            }
            out.println();
        } break;
        case 'O': {
            out.print(F("\t motor get mode: "));
            uint16_t mode;
            mode = MoCon::myStepperMotor.getDriveMode();
            MoCon::myStepperManager.motor_print_mode(out, mode);
            out.println();
        } break;
        case 'e': {
            out.print(F("\t motor "));
            uint8_t enable = atoi(&command[1]);
            switch(enable) {
                case 0: {
                    out.print(F("disabled"));
                    MoCon::myStepperMotor.disable();
                } break;
                case 1: {
                    out.print(F("enabled"));
                    MoCon::myStepperMotor.enable();
                } break;
                default: {
                    out.print(F("????"));
                }
            } //end switch
            out.print(F(". "));

            out.print(enable);
            out.println();
        } break;
        case 'E': {
            out.print(F("\t motor is enabled: "));
            bool isEnabled;
            isEnabled = MoCon::myStepperMotor.isEnabled();
            if(isEnabled) {
                out.print(F("en"));
            } else {
                out.print(F("dis"));
            }
            out.print(F("abled. "));
            out.print(isEnabled);
            out.println();
        } break;
        case 'u': {
            out.print(F("\t motor use standby "));
            uint8_t standby = atoi(&command[1]);
            switch(standby) {
                case 1: {
                    out.print(F("true"));
                    MoCon::myStepperMotor.useStandby(true);
                } break;
                case 0: {
                    out.print(F("false"));
                    MoCon::myStepperMotor.useStandby(false);
                } break;
                default: {
                    out.print(F("????"));
                }
            } //end switch
            out.print(F(". "));
            out.print(standby);
            out.println();
        } break;
        case 'U': {
            out.print(F("\t motor use standby: "));
            uint8_t standby = MoCon::myStepperMotor.isUseStandby();
            switch(standby) {
                case 1: {
                    out.print(F("true"));
                } break;
                case 0: {
                    out.print(F("false"));
                } break;
                default: {
                    out.print(F("????"));
                }
            } //end switch
            out.print(F(". "));
            out.print(standby);
            out.println();
        } break;
        //------------------------------------------
        case 'm': {
            out.print(F("\t motor moveTo "));
            int32_t position = atoi(&command[1]);
            MoCon::myStepperMotor.moveTo(position);
            out.print(position);
            out.println();
        } break;
        case 'M': {
            out.print(F("\t motor get target:"));
            int32_t position = MoCon::myStepperMotor.getTarget();
            if(position == MoCon::myStepperMotor.forwardLimit){
                out.print(F("forwardLimit"));
            } else {
                if(position == MoCon::myStepperMotor.reverseLimit){
                    out.print(F("reverseLimit"));
                } else {
                    out.print(position);
                }
            }
            out.println();
        } break;
        case 'p': {
            out.print(F("\t motor set position "));
            int32_t position = atoi(&command[1]);
            MoCon::myStepperMotor.setPos(position);
            out.print(position);
            out.println();
        } break;
        case 'P': {
            out.print(F("\t motor position: "));
            int32_t position;
            position = MoCon::myStepperMotor.getPos();
            out.print(position);
            out.println();
        } break;
        case 'l': {
            out.print(F("\t motor set limits:"));
            out.println();
            // uint16_t distLimit = MoCon::myStepperMotor.fullStepVal * 200;
            // MoCon::myStepperMotor.forwardLimit = distLimit;
            // MoCon::myStepperMotor.reverseLimit = -distLimit;
            // get first limit
            int32_t limit_forward = atol(&command[1]);
            MoCon::myStepperMotor.forwardLimit = limit_forward;
            // defaults to negative of limit_forward
            int32_t limit_reverse = limit_forward * -1;

            // check if second limit is given.
            char* delimiter = strchr(&command[1], ',');
            if(delimiter != NULL) {
                limit_reverse = atol(delimiter + 1);
            } else {
                out.print(F("\t   --> no second parameter given. used negative first."));
                out.println();
            }
            MoCon::myStepperMotor.reverseLimit = limit_reverse;

            // print current/new limits
            out.print(F("\t   forwardLimit: "));
            out.print(MoCon::myStepperMotor.forwardLimit);
            out.println();
            out.print(F("\t   reverseLimit: "));
            out.print(MoCon::myStepperMotor.reverseLimit);
            out.println();
        } break;
        case 'L': {
            out.print(F("\t motor limits: "));
            out.println();
            out.print(F("\t   forwardLimit: "));
            out.print(MoCon::myStepperMotor.forwardLimit);
            out.println();
            out.print(F("\t   reverseLimit: "));
            out.print(MoCon::myStepperMotor.reverseLimit);
            out.println();
        } break;
        //------------------------------------------
        case 'D': {
            out.print(F("\t motor stop "));
            MoCon::myStepperMotor.stop();
            out.println();
        } break;
        case 'd': {
            out.print(F("\t motor decelerate "));
            MoCon::myStepperMotor.decelerate();
            out.println();
        } break;
        case 'r': {
            out.print(F("\t motor run forward "));
            MoCon::myStepperMotor.moveTo(MoCon::myStepperMotor.forwardLimit);
            out.println();
        } break;
        case 'R': {
            out.print(F("\t motor run reverse "));
            MoCon::myStepperMotor.moveTo(MoCon::myStepperMotor.reverseLimit);
            out.println();
        } break;
        //---------------------------------------------------------------------
        default: {
            if(strlen(command) > 0) {
                out.print(F("command '"));
                out.print(command);
                out.println(F("' not recognized. try again."));
                pInstance->get_command_input_pointer()[0] = '?';
                pInstance->set_flag_EOL(true);
            } else {
                // exit submenu
                // menu_mode = menu_mode__MainMenu;
                // or
                // only show help
                pInstance->get_command_input_pointer()[0] = '?';
                pInstance->set_flag_EOL(true);
            }
        }
    } //end switch
} // end Menu Motor


// Main Menu
void menu_handle_Main(slight_DebugMenu *pInstance) {
    Print &out = pInstance->get_stream_out_ref();
    char *command = pInstance->get_command_current_pointer();
    // out.print("command: '");
    // out.print(command);
    // out.println("'");
    switch (command[0]) {
        case 'h':
        case 'H':
        case '?': {
            // help
            out.println(F("____________________________________________________________"));
            out.println();
            out.println(F("Help for Commands:"));
            out.println();
            out.println(F("\t '?': this help"));
            out.println(F("\t 'i': sketch info"));
            out.println(F("\t 'y': toggle DebugOut livesign print"));
            out.println(F("\t 'Y': toggle DebugOut livesign LED"));
            out.println(F("\t 'x': tests"));
            out.println();
            out.println(F("\t 'c': calibrate"));
            out.println(F("\t 'f': forward"));
            out.println(F("\t 'r': reverse"));
            out.println(F("\t 'p': print system state"));
            out.println(F("\t 's': emergency stop"));
            out.println();
            out.println(F("\t 'motor': enter Menu Motor "));
            out.println();
            out.println(F("____________________________________________________________"));
        } break;
        case 'i': {
            sketchinfo_print(out);
        } break;
        case 'y': {
            out.println(F("\t toggle DebugOut livesign Serial:"));
            debugOut_LiveSign_Serial_Enabled = !debugOut_LiveSign_Serial_Enabled;
            out.print(F("\t debugOut_LiveSign_Serial_Enabled:"));
            out.println(debugOut_LiveSign_Serial_Enabled);
        } break;
        case 'Y': {
            out.println(F("\t toggle DebugOut livesign LED:"));
            debugOut_LiveSign_LED_Enabled = !debugOut_LiveSign_LED_Enabled;
            out.print(F("\t debugOut_LiveSign_LED_Enabled:"));
            out.println(debugOut_LiveSign_LED_Enabled);
        } break;
        //---------------------------------------------------------------------
        case 'x': {
            // get state
            out.println(F("__________"));
            out.println(F("Tests:"));

            out.println(F("nothing to do."));

            // uint16_t wTest = 65535;
            uint16_t wTest = atoi(&command[1]);
            out.print(F("wTest: "));
            out.print(wTest);
            out.println();

            out.print(F("1: "));
            out.print((byte)wTest);
            out.println();

            out.print(F("2: "));
            out.print((byte)(wTest>>8));
            out.println();

            out.println();

            out.println(F("__________"));
        } break;
        //---------------------------------------------------------------------
        case 'c': {
            out.print(F("\t forward."));
            MoCon::myStepperManager.calibration_start();
            out.println();
        } break;
        case 'f': {
            out.print(F("\t forward."));
            MoCon::myStepperManager.motor_move_forward();
            out.println();
        } break;
        case 'r': {
            out.print(F("\t reverse."));
            MoCon::myStepperManager.motor_move_reverse();
            out.println();
        } break;
        case 'p': {
            out.print(F("\t print system event"));
            out.println();
            print_systemevent();
        } break;
        case 's': {
            out.print(F("\t emergency stop"));
            MoCon::myStepperManager.system_emergencystop();
            out.println();
        } break;
        case 'l': {
            out.print(F("\t set limit threshold "));
            uint16_t threshold = atoi(&command[1]);
            out.print(threshold);
            out.println();
            MoCon::myStepperManager.calibration_limit_threshold_set(threshold);
        } break;
        //---------------------------------------------------------------------
        case '_': {
            out.print(F("\t DemoFadeTo "));
            // convert part of string to int
            // (up to first char that is not a number)
            uint8_t id = atoi(&command[1]);
            // convert single character to int representation
            // uint8_t id = &command[1] - '0';
            out.print(id);
            out.print(F(" : "));
            uint16_t value = atoi(&command[3]);
            out.print(value);
            out.println();
            //demo_fadeTo(id, value);
            out.println(F("\t demo for parsing values --> finished."));
        } break;
        //---------------------------------------------------------------------
        case 'm': {
            // Menu Motor
            if (
                (command[0] == 'm') &&
                (command[1] == 'o') &&
                (command[2] == 't') &&
                (command[3] == 'o') &&
                (command[4] == 'r')
            ) {
                // switch to submenu
                menu_mode = menu_mode__MenuMotor;
                // check if full command (with submenu command) is given:
                if ( command[5] != '\0' ) {
                    // full length command
                    // copy submenu command part to input buffer
                    char* input = pInstance->get_command_input_pointer();
                    strcpy(input, &command[5]);
                    pInstance->set_flag_EOL(true);
                } else {
                    // just change into the submenu and display help:
                    pInstance->get_command_input_pointer()[0] = '?';
                    pInstance->set_flag_EOL(true);
                }
            }
        } break;
        //---------------------------------------------------------------------
        default: {
            if(strlen(command) > 0) {
                out.print(F("command '"));
                out.print(command);
                out.println(F("' not recognized. try again."));
            }
            pInstance->get_command_input_pointer()[0] = '?';
            pInstance->set_flag_EOL(true);
        }
    } // end switch

    // end Command Parser
}


// Menu Switcher
void menu_switcher(slight_DebugMenu *pInstance) {
    switch (menu_mode) {
        case menu_mode__MenuMain: {
            menu_handle_Main(pInstance);
        } break;
        case menu_mode__MenuMotor: {
            menu_handle_Motor(pInstance);
        } break;
        default: {
            // something went wronge - so reset and show MainMenu
            menu_mode = menu_mode__MenuMain;
        }
    } // end switch menu_mode
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// motor info


void print_systemevent() {
    Serial.print(F("system_state: "));

    StM_States::sysstate_t system_state =
        MoCon::myStepperManager.system_state_get();

    // led info:
    switch (system_state) {
        case StM_States::STATE_notvalid: {
            // nothing to do.
        } break;
        case StM_States::STATE_dirty: {
            digitalWrite(led_error_pin, led_on);
            digitalWrite(led_moving_pin, led_off);
        } break;
        case StM_States::STATE_error: {
            digitalWrite(led_error_pin, led_on);
            digitalWrite(led_moving_pin, led_off);
        } break;
        case StM_States::STATE_standby: {
            // nothing to do.
        } break;
        case StM_States::STATE_hold_forward:
        case StM_States::STATE_hold_reverse: {
            digitalWrite(led_moving_pin, led_off);
        } break;
        case StM_States::STATE_moving_forward:
        case StM_States::STATE_moving_reverse: {
            digitalWrite(led_moving_pin, led_on);
        } break;
        // calibrating
        case StM_States::STATE_calibrating_start: {
            // nothing to do
        } break;
        case StM_States::STATE_calibrating_check_next:
        case StM_States::STATE_calibrating_forward_start: {
            // nothing to do
        } break;
        case StM_States::STATE_calibrating_forward: {
            digitalWrite(led_moving_pin, led_on);
        } break;
        case StM_States::STATE_calibrating_forward_finished: {
            digitalWrite(led_moving_pin, led_off);
        } break;
        case StM_States::STATE_calibrating_reverse_start: {
            // nothing to do
        } break;
        case StM_States::STATE_calibrating_reverse: {
            digitalWrite(led_moving_pin, led_on);
        } break;
        case StM_States::STATE_calibrating_reverse_finished: {
            digitalWrite(led_moving_pin, led_off);
        } break;
        case StM_States::STATE_calibrating_finished: {
            digitalWrite(led_moving_pin, led_off);
            digitalWrite(led_error_pin, led_off);
        } break;
    }

    // slight_StepperManager::print_state(Serial, system_state);
    MoCon::myStepperManager.print_state(Serial);
    Serial.println();
    switch (system_state) {
        case StM_States::STATE_error: {
            // print error
            Serial.print(F("error: "));
            // MoCon::myStepperManager.print_error(
            //     Serial,
            //     MoCon::myStepperManager.error_type_get()
            // );
            MoCon::myStepperManager.print_error(Serial);
            Serial.println();
        } break;
        case StM_States::STATE_calibrating_finished: {
            // print limit values to serial
            Serial.print(F(" *forwardLimit: "));
            Serial.print(MoCon::myStepperManager.motor.forwardLimit);
            Serial.println();
            Serial.print(F(" *reverseLimit: "));
            Serial.print(MoCon::myStepperManager.motor.reverseLimit);
            Serial.println();
        } break;
        default: {
            // nothing to do here.
        } break;
    }  // end switch
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI handling



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initialise PINs

        //LiveSign
        pinMode(led_info_pin, OUTPUT);
        digitalWrite(led_info_pin, led_off);
        // motor state helper
        pinMode(led_moving_pin, OUTPUT);
        digitalWrite(led_moving_pin, led_off);
        pinMode(led_error_pin, OUTPUT);
        digitalWrite(led_error_pin, led_off);

        // as of arduino 1.0.1 you can use INPUT_PULLUP

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initialise serial

        // for ATmega32U4 devices:
        #if defined (__AVR_ATmega32U4__)
            // wait for arduino IDE to release all serial ports after upload.
            delay(1000);
        #endif

        Serial.begin(115200);

        // for ATmega32U4 devices:
        #if defined (__AVR_ATmega32U4__)
            // Wait for Serial Connection to be Opend from Host or
            // timeout after 6second
            uint32_t timeStamp_Start = millis();
            while( (! Serial) && ( (millis() - timeStamp_Start) < 2000 ) ) {
                // nothing to do
            }
        #endif

        Serial.println();

        // Serial.print(F("# Free RAM = "));
        // Serial.println(freeRam());

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // print welcome

        sketchinfo_print(Serial);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setup motor
        MoCon::init(Serial);

        // MoCon::myStepperManager.motor_move_event_set_callback(
        //     display_motorstate_update
        // );
        // MoCon::myStepperManager.motor_acceleration_event_set_callback(
        //     display_motoraccel_update
        // );
        // MoCon::myStepperManager.motor_enable_event_set_callback(
        //     display_motorenabled_update
        // );
        MoCon::myStepperManager.system_event_set_callback(
            print_systemevent
        );

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // show serial commands

        myDebugMenu.set_callback(menu_switcher);
        myDebugMenu.begin(true);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // go

        Serial.println(F("Loop:"));

} /** setup **/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main loop
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void loop() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // menu input
        myDebugMenu.update();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Stepper Manager
        MoCon::update();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // debug output

        if (
            (millis() - debugOut_LiveSign_TimeStamp_LastAction) >
            debugOut_LiveSign_UpdateInterval
        ) {
            debugOut_LiveSign_TimeStamp_LastAction = millis();

            if ( debugOut_LiveSign_Serial_Enabled ) {
                Serial.print(millis());
                Serial.print(F("ms;"));
                Serial.print(F("  free RAM = "));
                Serial.println(freeRam());
            }

            if ( debugOut_LiveSign_LED_Enabled ) {
                infoled_state = ! infoled_state;
                if (infoled_state) {
                    //set LED to LOW = on
                    digitalWrite(led_info_pin, led_on);
                } else {
                    //set LED to HIGH = off
                    digitalWrite(led_info_pin, led_off);
                }
            }

        }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // loop end
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
