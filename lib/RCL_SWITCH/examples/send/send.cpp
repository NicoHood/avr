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

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h> // sei()
#include "rcl_switch.h"
#include "board.h"

// Settings
#define PIN_RC 10
CRCLSwitch<PIN_RC> RCLSwitch;

int main(void)
{
    // Prepare pin for sending
    RCLSwitch.begin();

    // Setup hardware
    timer0_init();
    LED_INIT();

    // Enable global Interrupts and loop
    sei();

    while(true){
        static bool state = true;

        // Send input
        RCLSwitch.write(RCL_CHANNEL_1, RCL_BUTTON_A, state);

        // Change state
        state = !state;
        LED_TOGGLE();

        // Wait some time
        delay(3000);
    }
}
