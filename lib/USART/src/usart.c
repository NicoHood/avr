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

#if (USART_BUFFER_TX)
static volatile uint8_t usart_buffer_tx[USART_BUFFER_TX] = {};
static volatile uint8_t usart_buffer_tx_head = 0;
static volatile uint8_t usart_buffer_tx_tail = 0;

ISR(USART_UDRE_VECT)
{
    // Get next byte
    uint8_t c = usart_buffer_tx[usart_buffer_tx_tail];
    usart_buffer_tx_tail = ((uint8_t)(usart_buffer_tx_tail + (uint8_t)1) % (uint8_t)USART_BUFFER_TX);

    // Send byte and clear Tx flag. See comment in usart_putchar() for more details
    USART_UDR = c;
    USART_UCSRA |= (1 << USART_TXC);

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
    // The head is safed in a temporary variable to safe a bit more flash
    uint8_t head = usart_buffer_tx_head;
    if ((head == usart_buffer_tx_tail) && (USART_UCSRA & (1 << USART_UDRE)))
    {
        // Send byte and clear Tx flag. This Flag is required for the flush function
        // to work properly when this shortcut was used
        // Use an atomic block to ensure the flag gets cleared before the byte is already sent.
        // Also see https://github.com/arduino/Arduino/commit/0be4e8cd3cbc6216ff01cd83282b1231639f9b60
#ifndef USART_THREAD_SAFE
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
#endif
            USART_UDR = c;
            USART_UCSRA |= (1 << USART_TXC);
#ifndef USART_THREAD_SAFE
        }
#endif
    }
    else{
        // If buffer is full, wait for it to get emptied by interrupt
        uint8_t new_index = ((uint8_t)(head + (uint8_t)1) % (uint8_t)USART_BUFFER_TX);
        while (new_index == usart_buffer_tx_tail)
        {
            // Interrupts are disabled. Wait for empty transmit buffer, then start transmission
            if (!(SREG & (1 << SREG_I)))
            {
                while(!(USART_UCSRA & (1 << USART_UDRE)));
                USART_UDRE_VECT();
            }
        }

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

#if (USART_BUFFER_RX)
static volatile uint8_t usart_buffer_rx[USART_BUFFER_RX] = {};
static volatile uint8_t usart_buffer_rx_head = 0;
static volatile uint8_t usart_buffer_rx_tail = 0;

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
