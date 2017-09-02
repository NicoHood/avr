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

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h> // sei()
#include <util/delay.h> // _delay_ms()
#include "pcint.h"
#include "board.h"
#include "fastpin.h"

#if defined(CUSTOM_BOARD)
    // Pin 13 Arduino Uno
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRB  |=  (1 << PB5)
    #define LED_ON()        PORTB |=  (1 << PB5)
    #define LED_OFF()       PORTB &= ~(1 << PB5)
    #define LED_TOGGLE()    PORTB ^=  (1 << PB5)
#endif

// Settings
// Attention: Check if you pin is a valid PCINT and if it is enabled in the makefile!
#define PIN_PCINT 7

void blink(void)
{
    // Switch led state
    LED_TOGGLE();
}

int main(void)
{
    // Init and test led
    LED_INIT();
    blink();
    _delay_ms(1000);
    blink();

    // Set pin to INPUT_PULLUP with FastPin library
    FastPin<PIN_PCINT> pin;
    pin.setInput();
    pin.hi();

    // Attach the new PinChangeInterrupt and enable event function below
    attachPCINT(digitalPinToPCINT(PIN_PCINT), blink, CHANGE);

    // Enable global Interrupts and loop
    sei();
    while(true);
}
