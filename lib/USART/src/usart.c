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

#include "usart.h"
#include "usart_private.h"

// Filestreams for stdout and stdin usage
static FILE usart_input = FDEV_SETUP_STREAM(NULL, usart_fgetc, _FDEV_SETUP_READ);
static FILE usart_output = FDEV_SETUP_STREAM(usart_fputc, NULL, _FDEV_SETUP_WRITE);
static FILE usart_error = FDEV_SETUP_STREAM(usart_fputc, NULL, _FDEV_SETUP_WRITE);
static FILE usart_io = FDEV_SETUP_STREAM(usart_fputc, usart_fgetc, _FDEV_SETUP_RW);

void usart_init(void)
{
    USART_UBRRH = UBRRH_VALUE;
    USART_UBRRL = UBRRL_VALUE;

#if (USE_2X)
    USART_UCSRA |= (1 << USART_U2X);
#else
    USART_UCSRA &= ~(1 << USART_U2X);
#endif

    USART_UCSRC = (USART_PARITY | USART_STOP_BITS | USART_DATA_BITS);
#if defined(USART_URSEL)
	USART_UCSRB = ((1 << USART_URSEL) | (1 << USART_RXEN) | (1 << USART_TXEN));
#else
	USART_UCSRB = ((1 << USART_RXEN) | (1 << USART_TXEN));
#endif
}

void usart_putchar(const char c)
{
	// Wait for empty transmit buffer, then start transmission
    while(!(USART_UCSRA & (1 << USART_UDRE)));
    USART_UDR = c;
}

void usart_puts(const char *s)
{
	// Loop through entire string and append a carriage return
	while (*s) {
		usart_putchar(*s);
		s++;
	}
	usart_putchar('\n');
}

void usart_puts_P(const char *s)
{
	// Loop through entire string and append a carriage return
	char c;
	while((c = pgm_read_byte(s)))
	{
		usart_putchar(c);
		s++;
	}
	usart_putchar('\n');
}

char usart_getchar(void)
{
	// Wait for new byte and return it
    while(!(USART_UCSRA & (1 << USART_RXC)));
    return USART_UDR;
}

int usart_fputc(char c, FILE *stream)
{
    usart_putchar(c);
    return 0;
}

int usart_fgetc(FILE *stream)
{
    return usart_getchar();
}

void usart_init_stdout(void)
{
	stdout = &usart_output;
}

void usart_init_stdin(void)
{
	stdin = &usart_input;
}

void usart_init_stderr(void)
{
	stderr = &usart_error;
}

void usart_init_stdio(void)
{
	stdin = &usart_io;
	stdout = &usart_io;
}
