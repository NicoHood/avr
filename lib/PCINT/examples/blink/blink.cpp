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

/*
The following pins are usable for PinChangeInterrupt:
Arduino Uno/Nano/Mini: All pins are usable
Arduino Mega: 10, 11, 12, 13, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64),
            A11 (65), A12 (66), A13 (67), A14 (68), A15 (69)
Arduino Leonardo/Micro: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI)
HoodLoader2: All (broken out 1-7) pins are usable
Attiny 24/44/84: All pins are usable
Attiny 25/45/85: All pins are usable
Attiny 13: All pins are usable
Attiny 441/841: All pins are usable
ATmega644P/ATmega1284P: All pins are usable
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
