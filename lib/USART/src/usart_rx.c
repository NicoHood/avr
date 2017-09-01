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

#if (USART_BUFFER_RX)
static volatile uint8_t usart_buffer_rx[USART_BUFFER_RX] = { 0 };
static volatile uint8_t usart_buffer_rx_head = 0;
static uint8_t usart_buffer_rx_tail = 0;

ISR(USART_RX_VECT)
{
    // Check for parity errors
#if (USART_PARITY != USART_PARITY_NO)
    if (USART_UCSRA & (1 << USART_UPE))
    {
        // Discard byte
        char discard = USART_UDR;
        return;
    }
#endif

    // Read byte
    char c = USART_UDR;

    // Discard data if buffer is full
    uint8_t new_index = ((uint8_t)(usart_buffer_rx_head + (uint8_t)1) % (uint8_t)USART_BUFFER_RX);
    if (new_index == usart_buffer_rx_tail)
    {
        return;
    }

    // Safe data and increment head
    usart_buffer_rx[usart_buffer_rx_head] = c;
    usart_buffer_rx_head = new_index;
}

int usart_getchar(void)
{
    int ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Check if more data is available
        if (usart_buffer_rx_head == usart_buffer_rx_tail)
        {
            ret = -1;
        }
        else{
            ret = usart_buffer_rx[usart_buffer_rx_tail];
            usart_buffer_rx_tail = ((uint8_t)(usart_buffer_rx_tail + (uint8_t)1) % (uint8_t)USART_BUFFER_RX);
        }
    }
    return ret;
}

int usart_peek(void)
{
    int ret;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Check if more data is available
        if (usart_buffer_rx_head == usart_buffer_rx_tail)
        {
            ret = -1;
        }
        else{
            ret = usart_buffer_rx[usart_buffer_rx_tail];
        }
    }
    return ret;
}

uint8_t usart_avail_read(void)
{
    // Return how many bytes are available for reading
    // No atomic block required as 1byte access is already atomic and the tail value won't change.
    return ((uint8_t)((uint8_t)USART_BUFFER_RX - (uint8_t)usart_buffer_rx_tail + \
        (uint8_t)usart_buffer_rx_head) % (uint8_t)USART_BUFFER_RX);
}

#else // !(USART_BUFFER_RX)
int usart_getchar(void)
{
    // Check for new byte and return it
    if (USART_UCSRA & (1 << USART_RXC))
    {
        return USART_UDR;
    }
    return -1;
}

int usart_peek(void)
{
    // Impossible without buffers
    return -1;
}

uint8_t usart_avail_read(void)
{
    // Check for new byte and return it
    if (USART_UCSRA & (1 << USART_RXC))
    {
        return 1;
    }
    return 0;
}
#endif

size_t usart_read(uint8_t* buff, size_t len)
{
    // Read data into preallocated buffer. len should normally not be > USART_BUFFER_RX
    // Should be user along with usart_avail_read() to determine available byte count.
    size_t count = 0;
    while(len--)
    {
        int c = usart_getchar();
        if (c < 0)
        {
            return count;
        }
        *buff = c;
        buff++;
        count++;
    }
    return count;
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
