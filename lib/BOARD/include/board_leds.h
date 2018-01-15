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
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRC  |=  (1 << PC7)
    #define LED_ON()        PORTC |=  (1 << PC7)
    #define LED_OFF()       PORTC &= ~(1 << PC7)
    #define LED_TOGGLE()    PINC  |=  (1 << PC7)

    #define RX_LED          17
    #define RX_LED_INIT()   DDRB  |=  (1 << PB0)
    #define RX_LED_TOGGLE() PINB  |=  (1 << PB0)

    #define TX_LED          30
    #define TX_LED_INIT()   DDRD  |=  (1 << PD5)
    #define TX_LED_TOGGLE() PIND  |=  (1 << PD5)

    // Arduino Leonardo and Micro have different pull-up/downs for RX/TX leds
#if defined(ARDUINO_LEONARDO)
    #define RX_LED_ON()     PORTB &= ~(1 << PB0)
    #define RX_LED_OFF()    PORTB |=  (1 << PB0)
    #define TX_LED_ON()     PORTD &= ~(1 << PD5)
    #define TX_LED_OFF()    PORTD |=  (1 << PD5)

#elif defined(ARDUINO_MICRO)
    #define RX_LED_ON()     PORTB |=  (1 << PB0)
    #define RX_LED_OFF()    PORTB &= ~(1 << PB0)
    #define TX_LED_ON()     PORTD |=  (1 << PD5)
    #define TX_LED_OFF()    PORTD &= ~(1 << PD5)
#endif

#elif defined(ARDUINO_UNO) || defined(ARDUINO_NANO) || defined(ARDUINO_MINI)
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRB  |=  (1 << PB5)
    #define LED_ON()        PORTB |=  (1 << PB5)
    #define LED_OFF()       PORTB &= ~(1 << PB5)
    #define LED_TOGGLE()    PINB  |=  (1 << PB5)

#else
    #pragma message "Board " STR(BOARD) " Leds not supported. Please define your own board specific leds"
#endif

#ifdef __cplusplus
}
#endif
