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
#include <stdio.h> // printf()
#include <avr/pgmspace.h> // Progmem
#include "board.h"
#if defined(DMBS_MODULE_USART) && defined(DMBS_MODULE_USB_CDC_SERIAL)
#error "Only include one serial input DMBS module."
#elif defined(DMBS_MODULE_USART)
#include "usart.h"
#elif defined(DMBS_MODULE_USB_CDC_SERIAL)
#include "usb_cdc_serial.h"
#else
#error "Please include the USART or the USB_CDC_SERIAL DMBS module."
#endif
#include "infrared.h"

#if defined(CUSTOM_BOARD)
    // Pin 13 Arduino Uno
    #define LED_BUILTIN     13
    #define LED_INIT()      DDRB  |=  (1 << PB5)
    #define LED_ON()        PORTB |=  (1 << PB5)
    #define LED_OFF()       PORTB &= ~(1 << PB5)
    #define LED_TOGGLE()    PORTB ^=  (1 << PB5)
#endif

// Choose the IR protocol of your remote. See the other example for this.
CNec IRLremote;
//CPanasonic IRLremote;
//CHashIR IRLremote;

// Settings
// Attention: Check if you pin is a valid PCINT and if it is enabled in the makefile!
#define PIN_IR 10

// Filestreams for stdio functions
static FILE SerialStream;

int main(void)
{
    // Init peripherals
    LED_INIT();
    timer0_init();

    // Setup serial input stream
#if defined(DMBS_MODULE_USART)
    usart_init();
    usart_init_stream(&SerialStream);
#else
    USB_Init();
    usb_cdc_serial_init_stream(&SerialStream);
#endif

    stdin = &SerialStream;
    stdout = &SerialStream;

    // Start reading the remote. PinInterrupt or PinChangeInterrupt* will automatically be selected
    if (!IRLremote.begin<PIN_IR>())
    {
        puts_P(PSTR("You did not choose a valid pin."));
    }
    puts_P(PSTR("Startup"));

    // Enable global Interrupts and loop
    sei();

    while(true){
        // Check if we are currently receiving data
        //if (!IRLremote.receiving()) {
            // Run code that disables interrupts, such as some led strips
        //}

        // Check if new IR protocol data is available
        if (IRLremote.available())
        {
            // Light Led
            LED_ON();

            // Get the new data from the remote
            auto data = IRLremote.read();

            // Print the protocol data
            printf_P(PSTR("Address: %X\n Command: %X\n"), data.address, data.command);

            // Turn Led off after printing the data
            LED_OFF();
        }
    }
}
