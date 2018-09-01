/*
Copyright (c) 2018 NicoHood
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

#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "usart.h"
#include "board_leds.h"
#include "timer0.h"
#include "clap.h"

// Settings
#define BLINK_TIME 100UL

// Filestreams for stdio functions
static FILE UsartSerialStream;

int main(void)
{
    // Initialize usart, adc and enable global interrupts
    usart_init();
    timer0_init();
    clap_init();
    LED_INIT();
    sei();
    clap_enable();

    // Setup stdio functionallity
    usart_init_stream(&UsartSerialStream);
    stdin = &UsartSerialStream;
    stdout = &UsartSerialStream;

    // Print string
    printf_P(PSTR("#clap_test.c start\n"));

    // Echo incoming bytes
    while(true)
    {
        // Turn led off without delay
        static uint32_t led_time = 0;
        if (led_time) {
            uint32_t current_time = millis();
            uint32_t diff = current_time - led_time;
            if (diff > BLINK_TIME)
            {
                LED_OFF();
                led_time = 0;
            }
        }

        // Read clap and differenciate between a running and a finished session
        int8_t count = clap_read();
        if(count < 0){
            // Running session
            LED_ON();
            led_time = millis();
            printf_P(PSTR("clap@%d\n"), -count);
        }
        else if(count > 0){
            // Finished session
            printf_P(PSTR("clap=%d\n"), count);
        }
    }
}
