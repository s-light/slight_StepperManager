 /******************************************************************************

    test_StM_TWI_Master
        simple tests for StepperMotor usage over TWI
        debugout on usbserial interface: 115200baud

    hardware:
        Board:
            arduino compatible (ATmega32U4 or ATmega328p)
        TWI connection to second Arduino with Motordriver SW

    libraries used:
        ~ slight_StepperManager_TWI_Master
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
        17.12.2015 08:53 created
            (based on test__StepperManager_minimal.ino)

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
// #include <slight_ButtonInput.h>


// the following are only needed because the Arduino IDE don't get it ;-)
// they are not used in the code and
// the linker will hopefully optimize them away...
#include <slight_ButtonInput.h>
#include <kissStepper.h>
#include <kissStepper_TriState.h>
#include <slight_StepperManager.h>
#include <slight_StepperManager_TWI.h>
#include <slight_StepperManager_TWI_Controller.h>



#include <slight_StepperManager_States.h>
typedef slight_StepperManager_States StM_States;
// using StM_States = slight_StepperManager_States;
// #include "slight_StepperManager_TWI.h"
// typedef slight_StepperManager_TWI StM_TWI;
// // using StM_TWI = slight_StepperManager_TWI;
#include <Wire.h>
#include <slight_StepperManager_TWI_Master.h>
typedef slight_StepperManager_TWI_Master StM_TWI_Master;
// using StM_TWI_Master = slight_StepperManager_TWI_Master;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// function definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void print_systemevent(slight_StepperManager_TWI_Master *instance);

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
    out.println(F("| test_StM_TWI_Master.ino"));
    out.println(F("|   simple tests for StepperMotor usage over TWI"));
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

boolean infoled_state = 0;
const byte infoled_pin = 13;

unsigned long debugOut_LiveSign_TimeStamp_LastAction = 0;
const uint16_t debugOut_LiveSign_UpdateInterval = 1000; //ms

boolean debugOut_LiveSign_Serial_Enabled = 0;
boolean debugOut_LiveSign_LED_Enabled = 1;
boolean debugOut_printDisplay_onChange = 0;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Menu

// slight_DebugMenu(Stream &in_ref, Print &out_ref, uint8_t input_length_new);
slight_DebugMenu myDebugMenu(Serial, Serial, 15);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI StepperManager

const uint8_t TWI_address_own = 41;
const uint8_t TWI_address_target = 42;

StM_TWI_Master myStM_TWI_Master(
    TWI_address_target
);

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
            out.println(F("\t 'a': move acceleration write 'a500'"));
            out.println(F("\t 'A': move acceleration read"));
            out.println(F("\t 'q': move speed write 's500'"));
            out.println(F("\t 'Q': move speed read"));
            out.println(F("\t 'w': calibration acceleration write 'a500'"));
            out.println(F("\t 'W': calibration acceleration read"));
            out.println(F("\t 'e': calibration speed write 's500'"));
            out.println(F("\t 'E': calibration speed read"));
            out.println();
            out.println(F("\t 't': twi event target address write 't41'"));
            out.println(F("\t 'T': twi event target address read"));
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
            out.print(F("\t calibrate."));
            myStM_TWI_Master.calibrate();
            out.println();
        } break;
        case 'f': {
            out.print(F("\t forward."));
            myStM_TWI_Master.move_forward();
            out.println();
        } break;
        case 'r': {
            out.print(F("\t reverse."));
            myStM_TWI_Master.move_reverse();
            out.println();
        } break;
        case 'p': {
            out.print(F("\t print system event"));
            out.println();
            print_systemevent(&myStM_TWI_Master);
        } break;
        case 's': {
            out.print(F("\t emergency stop"));
            myStM_TWI_Master.emergencystop();
            out.println();
        } break;
        //---------------------------------------------------------------------
        case 'a': {
            out.print(F("\t move acceleration write "));
            uint16_t value = atoi(&command[1]);
            out.print(value);
            myStM_TWI_Master.settings_move_acceleration_write(value);
            out.println();
        } break;
        case 'A': {
            out.print(F("\t move acceleration:"));
            uint16_t value;
            value = myStM_TWI_Master.settings_move_acceleration_read();
            out.print(value);
            out.println();
        } break;
        case 'q': {
            out.print(F("\t move speed write "));
            uint16_t value = atoi(&command[1]);
            out.print(value);
            myStM_TWI_Master.settings_move_speed_write(value);
            out.println();
        } break;
        case 'Q': {
            out.print(F("\t move speed:"));
            uint16_t value;
            value = myStM_TWI_Master.settings_move_speed_read();
            out.print(value);
            out.println();
        } break;
        case 'w': {
            out.print(F("\t calibration acceleration write "));
            uint16_t value = atoi(&command[1]);
            out.print(value);
            myStM_TWI_Master.settings_calibration_acceleration_write(
                value
            );
            out.println();
        } break;
        case 'W': {
            out.print(F("\t calibration acceleration:"));
            uint16_t value;
            value = myStM_TWI_Master.settings_calibration_acceleration_read();
            out.print(value);
            out.println();
        } break;
        case 'e': {
            out.print(F("\t calibration speed write "));
            uint16_t value = atoi(&command[1]);
            out.print(value);
            myStM_TWI_Master.settings_calibration_speed_write(value);
            out.println();
        } break;
        case 'E': {
            out.print(F("\t calibration speed:"));
            uint16_t value;
            value = myStM_TWI_Master.settings_calibration_speed_read();
            out.print(value);
            out.println();
        } break;
        case 't': {
            out.print(F("\t twi event target address write "));
            uint8_t value = atoi(&command[1]);
            out.print(value);
            myStM_TWI_Master.settings_twi_event_target_address_write(value);
            out.println();
        } break;
        case 'T': {
            out.print(F("\t twi event target address :"));
            uint8_t value;
            value = myStM_TWI_Master.settings_twi_event_target_address_read();
            out.print(value);
            out.println();
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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// TWI StepperManager

void init_things(Print &out) {
    out.println(F("setup TWI:"));
    out.print(F("\t join bus as slave with address "));
    out.print(TWI_address_own);
    out.println();
    Wire.begin(TWI_address_own);

    out.println(F("\t setup onReceive event handling."));
    Wire.onReceive(wire_onReceive_ISR);
    out.println(F("\t finished."));


    out.println(F("setup TWI StepperManager:"));
    myStM_TWI_Master.begin(out);
    out.println(F("\t set twi event target address at slave to own"));
    // set twi event target to own address:
    // so we get all state information if they are changeing at the slave
    // if you want to disable the TWI event set the address to
    // TWI_NO_ADDRESS
    myStM_TWI_Master.settings_twi_event_target_address_write(TWI_address_own);
    out.println(F("\t set event callback"));
    myStM_TWI_Master.event_callback_set(
        print_systemevent
    );

    out.println(F("\t finished."));
}

void wire_onReceive_ISR(int rec_bytes) {
    myStM_TWI_Master.handle_onReceive_ISR(rec_bytes);
}

void print_systemevent(slight_StepperManager_TWI_Master *instance) {
    Serial.print(F("system_state: "));
    // myStM_TWI_Master.system_state_print(
    //     Serial,
    //     myStM_TWI_Master.system_state_get()
    // );
    instance->system_state_print(Serial);
    Serial.println();
    switch (instance->system_state_get()) {

        case StM_States::STATE_calibrating_finished: {
            // move arm to reverse position:
            instance->move_reverse();
        } break;
        case StM_States::STATE_error: {
            // print error
            Serial.print(F("error: "));
            // myStM_TWI_Master.error_type_print(
            //     Serial,
            //     myStM_TWI_Master.error_type_get()
            // );
            instance->error_type_print(Serial);
            Serial.println();
        } break;
        // case StM_States::STATE_calibrating_finished: {
        //     // print limit values to serial
        //     Serial.print(F(" *forwardLimit: "));
        //     Serial.print(myStM_TWI_Master.motor.forwardLimit);
        //     Serial.println();
        //     Serial.print(F(" *reverseLimit: "));
        //     Serial.print(myStM_TWI_Master.motor.reverseLimit);
        //     Serial.println();
        // } break;
        default: {
            // nothing to do here.
        } break;
    }  // end switch
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// other things..



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setup
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // initialise PINs

        //LiveSign
        pinMode(infoled_pin, OUTPUT);
        digitalWrite(infoled_pin, HIGH);

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
            while( (! Serial) && ( (millis() - timeStamp_Start) < 5000 ) ) {
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
    // setup TWI StepperManager
        init_things(Serial);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // show serial commands

        myDebugMenu.set_callback(menu_handle_Main);
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
        myStM_TWI_Master.update();

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
                    //set LED to HIGH
                    digitalWrite(infoled_pin, HIGH);
                } else {
                    //set LED to LOW
                    digitalWrite(infoled_pin, LOW);
                }
            }

        }

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // loop end
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// THE END
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
