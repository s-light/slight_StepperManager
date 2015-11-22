
#ifndef lcd_special_character_patterns_h
#define lcd_special_character_patterns_h

#include <Arduino.h>

// special character patterns:

uint8_t symbol_arrow_big_right[8] = {
    B01000,
    B01100,
    B01110,
    B01111,
    B01110,
    B01100,
    B01000,
};

uint8_t symbol_arrow_big_left[8] = {
    B00010,
    B00110,
    B01110,
    B11110,
    B01110,
    B00110,
    B00010,
};

uint8_t symbol_arrow_big_up[8] = {
    B00000,
    B00100,
    B00100,
    B01110,
    B01110,
    B11111,
    B00000,
};

uint8_t symbol_arrow_big_down[8] = {
    B00000,
    B11111,
    B01110,
    B01110,
    B00100,
    B00100,
    B00000,
};


uint8_t symbol_arrow_double_up[8] = {
    B00100,
    B01110,
    B11111,
    B00000,
    B00100,
    B01110,
    B11111,
};

uint8_t symbol_arrow_double_down[8] = {
    B11111,
    B01110,
    B00100,
    B00000,
    B11111,
    B01110,
    B00100,
};


uint8_t symbol_arrow_nextline[8] = {
    B00001,
    B00001,
    B00101,
    B01001,
    B11111,
    B01000,
    B00100,
};

uint8_t symbol_arrow_up[8] = {
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00100,
    B00100,
};

uint8_t symbol_arrow_down[8] = {
    B00100,
    B00100,
    B00100,
    B00100,
    B10101,
    B01110,
    B00100,
};

uint8_t symbol_arrow_right[8] = {
    B01000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B01000,
};

uint8_t symbol_arrow_left[8] = {
    B00010,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00010,
};



uint8_t symbol_pause[8] = {
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
};


uint8_t symbol_stop[8] = {
    B00000,
    B01110,
    B11111,
    B11111,
    B11111,
    B01110,
    B00000,
};


uint8_t symbol_Omega[8] = {
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01010,
    B11011,
};

uint8_t symbol_heart[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B11111,
    B01110,
    B00100,
};

uint8_t symbol_smiley[8] = {
    B00000,
    B10001,
    B00000,
    B00000,
    B10001,
    B01110,
    B00000,
};

uint8_t symbol_bell[8] = {
    B00100,
    B01110,
    B01110,
    B01110,
    B11111,
    B00100,
    B00000,
};

uint8_t symbol_note[8] = {
    B00100,
    B00110,
    B00101,
    B00101,
    B00100,
    B11100,
    B11100,
};


uint8_t symbol_empty[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
};

#endif // lcd_special_character_patterns_h
