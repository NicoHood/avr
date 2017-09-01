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
    // No need to disable it, as dynamic baudrate change is not supported
    //USART_UCSRA &= ~(1 << USART_U2X);
#endif

    USART_UCSRC = (USART_PARITY | USART_STOP_BITS | USART_DATA_BITS);

    // Determine UCSRB value based on selected compile-time options
    uint8_t USART_UCSRB_VAL = ((1 << USART_RXEN) | (1 << USART_TXEN));
#if defined(USART_URSEL)
    USART_UCSRB_VAL |= (1 << USART_URSEL);
#endif
#if (USART_BUFFER_RX)
    USART_UCSRB_VAL |= (1 << USART_RXCIE);
#endif
    USART_UCSRB = USART_UCSRB_VAL;
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
