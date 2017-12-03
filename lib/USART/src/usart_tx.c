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

#if (USART_BUFFER_TX)
static volatile uint8_t usart_buffer_tx[USART_BUFFER_TX] = { 0 };
static uint8_t usart_buffer_tx_head = 0;
static volatile uint8_t usart_buffer_tx_tail = 0;

ISR(USART_UDRE_VECT)
{
    // Get next byte
    uint8_t c = usart_buffer_tx[usart_buffer_tx_tail];
    usart_buffer_tx_tail = ((uint8_t)(usart_buffer_tx_tail + (uint8_t)1) % (uint8_t)USART_BUFFER_TX);

    // Send byte and clear Tx flag.
    USART_UDR = c;

    // Diable usart tx interrupt if all bytes were transmitted
    if (usart_buffer_tx_head == usart_buffer_tx_tail)
    {
        USART_UCSRB &= ~(1 << USART_UDRIE);
    }
}

void usart_putchar(const char c)
{
#ifdef USART_THREAD_SAFE
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif

    // Send data directly if buffer is empty and transmit is ready
    // This improves performance on higher baudrates to avoid the ISR overhead
    // No atomic block is required, as only the tail gets incremented inside the ISR, head untouched
    if ((usart_buffer_tx_head == usart_buffer_tx_tail) && (USART_UCSRA & (1 << USART_UDRE)))
    {
        // Send byte. TXC bit will not be used here.
        // See: https://github.com/arduino/Arduino/commit/ccd8880a37261b53ae11c666de0a29d85c28ae36
        USART_UDR = c;
    }
    else{
        // If buffer is full, wait for it to get emptied by interrupt
        uint8_t new_index = ((uint8_t)(usart_buffer_tx_head + (uint8_t)1) % (uint8_t)USART_BUFFER_TX);
#ifdef USART_THREAD_SAFE
        if (new_index == usart_buffer_tx_tail)
        {
            // Interrupts are disabled. Wait for empty transmit buffer, then start transmission
            while(!(USART_UCSRA & (1 << USART_UDRE)));
            USART_UDRE_VECT();
        }
#else
        while (new_index == usart_buffer_tx_tail)
        {
            // Interrupts are disabled. Wait for empty transmit buffer, then start transmission
            if (!(SREG & (1 << SREG_I)))
            {
                while(!(USART_UCSRA & (1 << USART_UDRE)));
                USART_UDRE_VECT();
            }
        }
#endif

        // Safe new byte and enable interrupts again
        usart_buffer_tx[usart_buffer_tx_head] = c;
        usart_buffer_tx_head = new_index;
        USART_UCSRB |= (1 << USART_UDRIE);
    }

#ifdef USART_THREAD_SAFE
    }
#endif
}

void usart_flush(void)
{
    // Wait until all data inside the buffer was sent
    while (usart_buffer_tx_head != usart_buffer_tx_tail)
    {
        // Interrupts are disabled. Wait for empty transmit buffer, then start transmission
        if (!(SREG & (1 << SREG_I)))
        {
            while(!(USART_UCSRA & (1 << USART_UDRE)));
            USART_UDRE_VECT();
        }
    }

    // Wait for the final byte to flush
    while(!(USART_UCSRA & (1 << USART_UDRE)));
}

uint8_t usart_avail_write(void)
{
    return ((uint8_t)((uint8_t)USART_BUFFER_TX - usart_buffer_tx_head + \
        usart_buffer_tx_tail - (uint8_t)1) % (uint8_t)USART_BUFFER_TX);
}

#else
void usart_putchar(const char c)
{
#ifdef USART_THREAD_SAFE
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
#endif
        // Wait for empty transmit buffer, then start transmission
        while(!(USART_UCSRA & (1 << USART_UDRE)));
        USART_UDR = c;
#ifdef USART_THREAD_SAFE
    }
#endif
}

void usart_flush(void)
{
    // Wait for empty transmit buffer
    while(!(USART_UCSRA & (1 << USART_UDRE)));
}

uint8_t usart_avail_write(void)
{
    if (USART_UCSRA & (1 << USART_UDRE))
    {
        return 1;
    }
    return 0;
}
#endif

void usart_write(const uint8_t* buff, size_t len)
{
    // Write buffer to usart
    while(len--)
    {
        usart_putchar(*buff);
        buff++;
    }
}

void usart_write_P(const uint8_t* buff, size_t len)
{
    // Write buffer to usart
    while(len--)
    {
        usart_putchar(pgm_read_byte(buff));
        buff++;
    }
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
    while ((c = pgm_read_byte(s)))
    {
        usart_putchar(c);
        s++;
    }
    usart_putchar('\n');
}

int usart_fputc(char c, FILE *stream)
{
    usart_putchar(c);
    return 0;
}
