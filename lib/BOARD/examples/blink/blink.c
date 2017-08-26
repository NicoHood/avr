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

#include <stdbool.h>
#include <util/delay.h>
#include "board_leds.h"

#if defined(CUSTOM_BOARD)
    // Pin 13 Arduino Uno
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRB  |=  (1 << PB5)
    #define LED_ON()        PORTB |=  (1 << PB5)
    #define LED_OFF()       PORTB &= ~(1 << PB5)
    #define LED_TOGGLE()    PORTB ^=  (1 << PB5)
#endif

// Settings
#define BLINK_SPEED 100UL

int main(void)
{
    // Blink led
    LED_INIT();
    while(true){
        _delay_ms(BLINK_SPEED);
        LED_ON();
        _delay_ms(BLINK_SPEED);
        LED_OFF();
    }
}
