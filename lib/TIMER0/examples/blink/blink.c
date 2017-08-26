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
#include <avr/interrupt.h>
#include <assert.h>
#include "timer0.h"
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

void blink(void)
{
    // Blink led
    LED_INIT();
    while(true){
        delay(BLINK_SPEED);
        LED_ON();
        delay(BLINK_SPEED);
        LED_OFF();
    }
}

void blink2(void)
{
    // Blink led
    LED_INIT();
    while(true){
        // TODO rename to static_assert() when avr-libc >2.0.0 gets released
        // https://savannah.nongnu.org/bugs/?41689
        _Static_assert((BLINK_SPEED * 10) < ((uint32_t)1 << 16),
            "delayMicroseconds() only accepts 16bit delays. Please use a smaller BLINK_SPEED.");
        for(uint8_t i = 0; i < 100; i++)
        {
            delayMicroseconds((uint16_t)(BLINK_SPEED * 10UL));
        }
        LED_ON();
        for(uint8_t i = 0; i < 100; i++)
        {
            delayMicroseconds((uint16_t)(BLINK_SPEED * 10UL));
        }
        LED_OFF();
    }
}

void blink_without_delay(void)
{
    // Blink led without delay
    LED_INIT();
    while(true){
        static uint32_t prev_ms = 0;
        uint32_t curr_ms = millis();

        // Change led state if time expired
        if(curr_ms - prev_ms >= BLINK_SPEED)
        {
            prev_ms = curr_ms;
            LED_TOGGLE();
        }
    }
}

void blink_without_delay2(void)
{
    // Blink led without delay
    LED_INIT();
    while(true){
        static uint32_t prev_us = 0;
        uint32_t curr_us = micros();

        // Change led state if time expired
        if(curr_us - prev_us >= (BLINK_SPEED * 1000UL))
        {
            prev_us = curr_us;
            LED_TOGGLE();
        }
    }
}

int main(void)
{
    // Initialize timer and enable interrupts
    init_timer0();
    sei();

    // Choose blink function
    blink();
    //blink2();
    //blink_without_delay();
    //blink_without_delay2();
}
