/*
Copyright (c) 2017 NicoHood
See the readme for credit to other people.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

// Board variants
#if defined(ARDUINO_LEONARDO) || defined(ARDUINO_MICRO)

    // Mapping of analog pins as digital I/O
    // A6-A11 share with digital pins
    #define A0   (18)
    #define A1   (19)
    #define A2   (20)
    #define A3   (21)
    #define A4   (22)
    #define A5   (23)
    #define A6   (24)
    #define A7   (25)
    #define A8   (26)
    #define A9   (27)
    #define A10  (28)
    #define A11  (29)

    #define digitalPinToPCICR(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= A8 && (p) <= A10)) ? (&PCICR) : ((uint8_t *)0))
    #define digitalPinToPCICRbit(p) 0
    #define digitalPinToPCMSK(p)    ((((p) >= 8 && (p) <= 11) || ((p) >= 14 && (p) <= 17) || ((p) >= A8 && (p) <= A10)) ? (&PCMSK0) : ((uint8_t *)0))
    #define digitalPinToPCMSKbit(p) ( ((p) >= 8 && (p) <= 11) ? (p) - 4 : ((p) == 14 ? 3 : ((p) == 15 ? 1 : ((p) == 16 ? 2 : ((p) == 17 ? 0 : (p - A8 + 4))))))

#elif defined(ARDUINO_UNO) || defined(ARDUINO_NANO) || defined(ARDUINO_MINI)

    // Mapping of analog pins as digital I/O
    #define A0   (14)
    #define A1   (15)
    #define A2   (16)
    #define A3   (17)
    #define A4   (18)
    #define A5   (19)
    #define A6   (20)
    #define A7   (21)

    #define digitalPinToPCICR(p)    (((p) >= 0 && (p) <= 21) ? (&PCICR) : ((uint8_t *)0))
    #define digitalPinToPCICRbit(p) (((p) <= 7) ? 2 : (((p) <= 13) ? 0 : 1))
    #define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK2) : (((p) <= 13) ? (&PCMSK0) : (((p) <= 21) ? (&PCMSK1) : ((uint8_t *)0))))
    #define digitalPinToPCMSKbit(p) (((p) <= 7) ? (p) : (((p) <= 13) ? ((p) - 8) : ((p) - 14)))

#else
    #pragma message "Board " STR(BOARD) " Pins not supported. Please define your own board specific pins"
#endif

#ifdef __cplusplus
}
#endif
