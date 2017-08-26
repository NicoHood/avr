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

// More information about stdio and PROGMEM:
// http://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html
// http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html

#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "usart.h"

int main(void)
{
    // Initialize usart and enable global interrupts
    usart_init();
    sei();

    // Write directly to usarts
    usart_puts("Hello Usart!");
    usart_puts_P(PSTR("Hello Usart Progmem!"));

    // Setup stdio functionallity
    usart_init_stdout();
    usart_init_stdin();

    // Print string
    puts("Hello stdio!");
    printf("Printf can print HEX %X and DEC %d\n", 1337, 1337);

    // Safe RAM by using PROGMEM
    puts_P(PSTR("Hello stdio Progmem!"));
    printf_P(PSTR("Printf_P can also print HEX %X and DEC %d\n"), 1337, 1337);

    // Echo incoming bytes
    while(true)
    {
        int c = getchar();
        if(c >= 0)
        {
            putchar(c);
        }

        // Use delay here to test if serial RX buffer is working properly at high speeds
        //_delay_ms(10);
    }
}
