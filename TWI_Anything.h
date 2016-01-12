// Written by Nick Gammon
// May 2012
// original I2C_Anything
// http://www.gammon.com.au/forum/?id=10896&reply=8#reply8
// update by Nick Gammon 17.12.2013:
// http://forum.arduino.cc/index.php?topic=204969.msg1509976#msg1509976

#include <Arduino.h>
#include <Wire.h>

// updated version - know works inside onRequest
template <typename T> unsigned int TWI_writeAnything (const T& value) {
    uint8_t write_size;
    write_size = Wire.write((byte *) &value, sizeof (value));
    return write_size;
}  // end of I2C_writeAnything

// definitve copatible with Buffer_readAnything:
// template <typename T> unsigned int TWI_writeAnything(
//     const T& value
// ) {
//     size_t size = sizeof value;
//     uint8_t buffer[size];
//     const byte* p = (const byte*) &value;
//     size_t i;
//     for (i = 0; i < sizeof value; i++) {
//         buffer[i] = *p++;
//     }
//     return Wire.write(buffer, size);
// }  // end of Buffer_writeAnything

// original
// template <typename T> unsigned int I2C_writeAnything (const T& value) {
//     const byte * p = (const byte*) &value;
//     unsigned int i;
//     for (i = 0; i < sizeof value; i++)
//           Wire.write(*p++);
//     return i;
// }  // end of I2C_writeAnything

// write variant to be used inside onRequest
// http://forum.arduino.cc/index.php?topic=104732.msg1057635#msg1057635
// template <typename T> int I2C_singleWriteAnything (const T& value) {
//     size_t size = sizeof value;
//     byte vals[size];
//     const byte* p = (const byte*) &value;
//     for (size_t i = 0; i < sizeof value; i++) {
//       vals[i] = *p++;
//     }
//
//     Wire.write(vals, size);
//     return i;
// }  // end of I2C_singleWriteAnything

template <typename T> unsigned int TWI_readAnything(
    T& value
) {
    byte * p = (byte*) &value;
    size_t i;
    for (i = 0; i < sizeof value; i++)
          *p++ = Wire.read();
    return i;
}  // end of I2C_readAnything

template <typename T> unsigned int Buffer_writeAnything(
    T& value,
    uint8_t *buffer
) {
    const byte* p = (const byte*) &value;
    size_t i;
    for (i = 0; i < sizeof value; i++) {
        buffer[i] = *p++;
    }
    return i;
}  // end of Buffer_writeAnything

template <typename T> unsigned int Buffer_readAnything(
    T& value,
    uint8_t *buffer
) {
    byte * p = (byte*) &value;
    size_t i;
    for (i = 0; i < sizeof value; i++)
          *p++ = buffer[i];
    return i;
}  // end of Buffer_readAnything

template <typename T> unsigned int Buffer_readAnything(
    T& value,
    volatile uint8_t *buffer
) {
    byte * p = (byte*) &value;
    size_t i;
    for (i = 0; i < sizeof value; i++)
          *p++ = buffer[i];
    return i;
}  // end of Buffer_readAnything
